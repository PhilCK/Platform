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

        uint32_t val = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_RESIZE_REDIRECT;

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
                XCB_CW_EVENT_MASK,
                &val); 

        /* Map the window on the screen */
        xcb_map_window (connection, window);


        /* Make sure commands are sent before we pause so that the window gets shown */
        xcb_flush (connection);

        /* Setup context and return 
         */

        *ctx = (struct roa_ctx) {
                .con = connection,
                .win = window,
        };

        ctx->con = connection;
        ctx->win = window;

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

        while((evt = xcb_wait_for_event(ctx->con))  !=  NULL) {
                switch(evt->response_type & ~0x80) {
                case XCB_EXPOSE: {
                        break;
                }
                case XCB_RESIZE_REQUEST: {
                        xcb_resize_request_event_t *resize = NULL;
                        resize = (xcb_resize_request_event_t*)evt;

                        ctx->width = (int)resize->width;
                        ctx->height = (int)resize->height;

                        __builtin_printf("%dx%d\n", ctx->width, ctx->height);
                }
                default:
                        break;
                }
        }

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
