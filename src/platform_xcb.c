#include <roa/platform.h>
#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <assert.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Types
 */

struct roa_platform_ctx {
        xcb_connection_t *con;
        xcb_window_t win;

        xcb_intern_atom_cookie_t protocols_cookie;
        xcb_intern_atom_reply_t *protocols_reply;
        xcb_intern_atom_cookie_t delete_cookie;
        xcb_intern_atom_reply_t *delete_reply;

        int width;
        int height;

        uint64_t start_ms;
        uint64_t last_ms;
        uint64_t delta_ms;
};

/* -------------------------------------------------------------------------- */
/* Internal Helpers
 */

#include <sys/time.h>

static uint64_t
roa_clock_now_ms()
{
        struct timeval tv;
        gettimeofday(&tv, 0);
        
        uint64_t milli = (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL);
        return milli;
}

/* -------------------------------------------------------------------------- */
/* Public Interface
 */

struct roa_platform_ctx*
roa_platform_create(
        const struct roa_platform_desc *desc)
{
        assert(desc && "Need a description");

        struct roa_platform_ctx *ctx;
        ctx = malloc(sizeof(*ctx));
        memset(ctx, 0, sizeof(*ctx));

        /* Open the connection to the X server */
        xcb_connection_t *connection = xcb_connect(NULL, NULL);

        /* Get the first screen */
        const xcb_setup_t      *setup  = xcb_get_setup(connection);
        xcb_screen_iterator_t   iter   = xcb_setup_roots_iterator(setup);
        xcb_screen_t           *screen = iter.data;

        /* Create the window */
        xcb_window_t window = xcb_generate_id(connection);

        uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t value_list[2] = {0};
        value_list[0] = screen->black_pixel;
        value_list[1] =
                    XCB_EVENT_MASK_KEY_RELEASE |
                    XCB_EVENT_MASK_KEY_PRESS |
                    XCB_EVENT_MASK_EXPOSURE |
                    XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                    XCB_EVENT_MASK_POINTER_MOTION |
                    XCB_EVENT_MASK_BUTTON_PRESS |
                    XCB_EVENT_MASK_BUTTON_RELEASE;

        int half_width = screen->width_in_pixels / 2;
        int half_height = screen->height_in_pixels / 2;

        int x = half_width - (desc->width / 2);
        int y = half_height - (desc->height / 2);

        __builtin_printf("%d x %d\n", x, y);

        xcb_create_window(
                connection,                    /* Connection          */
                XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                window,                        /* window Id           */
                screen->root,                  /* parent window       */
                x, y,                          /* x, y                */
                desc->width, desc->height,     /* width, height       */
                1,                            /* border_width        */
                XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                screen->root_visual,           /* visual              */
                value_mask, //XCB_CW_EVENT_MASK,
                value_list); //&val); 

        const char *title = desc->title ? desc->title : "Republic Of Almost";

        xcb_change_property(
                connection,
                XCB_PROP_MODE_REPLACE,
                window,
                XCB_ATOM_WM_NAME,
                XCB_ATOM_STRING,
                8,
                strlen(title), 
                title);

        /* Need to hook up the close button so we know when the user hits the
         * [x] on the window
         */

        xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(
                connection,
                1,
                12,
                "WM_PROTOCOLS");

        xcb_intern_atom_reply_t *protocols_reply = xcb_intern_atom_reply(
                connection,
                protocols_cookie,
                0);

        xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(
                connection,
                0,
                16,
                "WM_DELETE_WINDOW");

        xcb_intern_atom_reply_t *delete_reply = xcb_intern_atom_reply(
                connection,
                delete_cookie,
                0);

         xcb_change_property(
                connection,
                XCB_PROP_MODE_REPLACE,
                window,
                (*protocols_reply).atom,
                4,
                32,
                1,
                &(*delete_reply).atom);

        /* Map the window on the screen */

         xcb_map_window(connection, window);

        /* Resize the window 
         * I'm not super sure why this doesn't seem to work when we create
         * the window.
         */
        
        uint16_t mask = 0;
                mask |= XCB_CONFIG_WINDOW_X;
                mask |= XCB_CONFIG_WINDOW_Y;
                mask |= XCB_CONFIG_WINDOW_WIDTH;
                mask |= XCB_CONFIG_WINDOW_HEIGHT;

        const uint32_t values[] = {
                x,    /* x */
                y,    /* y */
                desc->width, /* width */
                desc->height   /* height */
        };

        xcb_configure_window(connection, window, mask, values);

        /* Make sure commands are sent before we pause so that the window 
         * gets shown
         */
        xcb_flush (connection);

        /* Setup context and return 
         */

        *ctx = (struct roa_platform_ctx) {
                .con              = connection,
                .win              = window,
                .protocols_cookie = protocols_cookie,
                .protocols_reply  = protocols_reply,
                .delete_cookie    = delete_cookie,
                .delete_reply     = delete_reply,
                .width            = desc->width,
                .height           = desc->height,
                .start_ms         = roa_clock_now_ms(),
                .last_ms          = roa_clock_now_ms(),
                .delta_ms         = 0,
        };

        return ctx;
}

void
roa_platform_destroy(
        const struct roa_platform_ctx *ctx)
{
        xcb_disconnect(ctx->con);
}

uint64_t
roa_platform_poll(
        struct roa_platform_ctx *ctx,
        const struct roa_platform_poll_desc *desc)
{
        uint64_t events = 0;
        events |= (!ctx->win * ROA_PLATFORM_WINDOW_CLOSED);

        /* Time
         */

        uint64_t now = roa_clock_now_ms();
        uint64_t dt = now - ctx->last_ms;
        ctx->delta_ms = dt;
        ctx->last_ms = now;

        /* Poll the XCB messages until dry
         */

        xcb_generic_event_t *evt = 0;

        while((evt = xcb_poll_for_event(ctx->con))  !=  NULL) {
                switch(evt->response_type & ~0x80) {
                case XCB_EXPOSE: {
                        /* Unsure if this is needed 
                        xcb_client_message_event_t client_message;

                        client_message.response_type = XCB_CLIENT_MESSAGE;
                        client_message.format = 32;
                        client_message.window = ctx->win;
                        client_message.type = XCB_ATOM_NOTICE;

                        xcb_send_event(ctx->con, 0, ctx->win,
                                0, (char *) &client_message);
                        
                        xcb_flush(ctx->con);
                        */

                        break;
                }
                case XCB_CONFIGURE_NOTIFY: {
                        xcb_configure_notify_event_t *cfg = NULL;
                        cfg = (xcb_configure_notify_event_t*)evt;

                        ctx->width = (int)cfg->width;
                        ctx->height = (int)cfg->height;

                        events |= ROA_PLATFORM_WINDOW_SIZE;
                        break;
                }
                case XCB_CREATE_NOTIFY: {
                        printf("Destroy\n");
                        break;
                }
                case XCB_CLIENT_MESSAGE: {
                        xcb_client_message_event_t *msg = NULL;
                        msg = (xcb_client_message_event_t*)evt;

                        if(msg->data.data32[0] == ctx->delete_reply->atom ) {
                                xcb_unmap_window(ctx->con, ctx->win);
                                xcb_destroy_window(ctx->con, ctx->win);
                                xcb_flush(ctx->con);
                                ctx->win = 0;
                        }
                        break;
                }
                default:
                        break;
                }

                free(evt);
        }

        return events;
}

/* -------------------------------------------------------------------------- */
/* Platform Properties
 */

void
roa_platform_screen_size(
        struct roa_platform_ctx *ctx,
        int *out_x,
        int *out_y)
{
        assert(ctx);
        assert(out_x);
        assert(out_y);

        *out_x = ctx->width;
        *out_y = ctx->height;
}

uint64_t
roa_platform_ms_delta(
        struct roa_platform_ctx *ctx)
{
        assert(ctx);

        return ctx->delta_ms;
}

uint64_t
roa_platform_ms_running(
        struct roa_platform_ctx *ctx)
{
        assert(ctx);

        return ctx->last_ms - ctx->start_ms;
}

/* -------------------------------------------------------------------------- */
/* Native Types
 * Connections to the native layer.
 */

uintptr_t
roa_platform_details_xcb_window(
        struct roa_platform_ctx *ctx)
{
        assert(ctx);
        return (uintptr_t)ctx->win;
}

uintptr_t
roa_platform_details_xcb_connection(
        struct roa_platform_ctx *ctx)
{
        assert(ctx);
        return (uintptr_t)ctx->con;
}

