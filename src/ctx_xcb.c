#include <roa/context.h>
#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#include <assert.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Types
 */

struct roa_ctx {
        xcb_connection_t *con;
        xcb_window_t win;

        int width;
        int height;
};

/* -------------------------------------------------------------------------- */
/* Public Interface
 */

struct roa_ctx*
roa_ctx_create(
        const struct roa_ctx_desc *desc)
{
        assert(desc && "Need a description");

        struct roa_ctx *ctx;
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

        //uint32_t val = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_RESIZE_REDIRECT;
        
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

        xcb_create_window(
                connection,                    /* Connection          */
                XCB_COPY_FROM_PARENT,          /* depth (same as root)*/
                window,                        /* window Id           */
                screen->root,                  /* parent window       */
                0, 0,                          /* x, y                */
                desc->width, desc->height,     /* width, height       */
                1,                            /* border_width        */
                XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                screen->root_visual,           /* visual              */
                value_mask, //XCB_CW_EVENT_MASK,
                value_list); //&val); 

        /* Map the window on the screen */
        xcb_map_window (connection, window);


        /* Make sure commands are sent before we pause so that the window gets shown */
        xcb_flush (connection);

        /* Setup context and return 
         */

        *ctx = (struct roa_ctx) {
                .con = connection,
                .win = window,
                .width = desc->width,
                .height = desc->height,
        };

        return ctx;
}

void
roa_ctx_destroy(
        const struct roa_ctx *ctx)
{
        xcb_disconnect(ctx->con);
}

/* -------------------------------------------------------------------------- */
/* Platform Interface 
 */

uint64_t
roa_ctx_poll(
        struct roa_ctx *ctx,
        const struct roa_ctx_poll_desc *desc)
{
        xcb_generic_event_t *evt = 0;

        while((evt = xcb_poll_for_event(ctx->con))  !=  NULL) {
                switch(evt->response_type & ~0x80) {
                case XCB_EXPOSE: {
                        xcb_client_message_event_t client_message;

                        client_message.response_type = XCB_CLIENT_MESSAGE;
                        client_message.format = 32;
                        client_message.window = ctx->win;
                        client_message.type = XCB_ATOM_NOTICE;

                        xcb_send_event(ctx->con, 0, ctx->win,
                                0, (char *) &client_message);
                        
                        xcb_flush(ctx->con);

                        __builtin_printf("xpose\n");
                        
                        break;
                }
                case XCB_CONFIGURE_NOTIFY: {
                        xcb_configure_notify_event_t *cfg = NULL;
                        cfg = (xcb_configure_notify_event_t*)evt;

                        ctx->width = (int)cfg->width;
                        ctx->height = (int)cfg->height;
                }
                default:
                        break;
                }

                free(evt);
        }

        xcb_map_window(ctx->con, ctx->win);

        return 0;
}

void
roa_ctx_screen_size(
        struct roa_ctx *ctx,
        int *out_x,
        int *out_y)
{
        assert(ctx);
        assert(out_x);
        assert(out_y);

        *out_x = ctx->width;
        *out_y = ctx->height;
}

uintptr_t
roa_platform_details_xcb_window(
        struct roa_ctx *ctx)
{
        assert(ctx);
        return (uintptr_t)ctx->win;
}

uintptr_t
roa_platform_details_xcb_connection(
        struct roa_ctx *ctx)
{
        assert(ctx);
        return (uintptr_t)ctx->con;
}

