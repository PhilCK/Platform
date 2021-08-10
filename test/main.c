#include <assert.h>
#include <roa/platform.h>
#include <stddef.h>
#include <xcb/xcb.h>
#include <stdio.h>

int
main()
{
        printf("ROA Platform Test\n");

        struct roa_platform_ctx *ctx = roa_platform_create(
                &(struct roa_platform_desc) {
                        .width = 1280,
                        .height = 800,
                        .title = "ROA Test",
                }
        );

        for(;;) {
                struct roa_platform_poll_desc new_frame = {
                        .width = 1280,
                        .height = 800,
                };

                uint64_t evts = roa_platform_poll(ctx, &new_frame);

                /* Quit Event */
                if(evts & ROA_PLATFORM_WINDOW_CLOSED) {
                        break;
                }
        };

        roa_platform_destroy(ctx);

        return 0;
}
