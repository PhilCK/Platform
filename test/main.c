#include <assert.h>
#include <roa/platform.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#endif

int
main()
{
        printf("ROA Platform Test\n");

        struct roa_platform_ctx *ctx = roa_platform_create(
                &(struct roa_platform_desc) {
                        .width = 0,
                        .height = 0,
                        .title = "ROA Test",
                }
        );

        for(;;) {
                struct roa_platform_poll_desc new_frame = {
                        .width = 1280,
                        .height = 800,
                };

                uint64_t evts = roa_platform_poll(ctx, NULL);

                /* Quit Event */
                if(evts & ROA_PLATFORM_WINDOW_CLOSED) {
                        break;
                }

                struct roa_platform_properties prop = {0};
                roa_platform_properties(ctx, &prop);
                
                printf("Time: %lu(ms), Delta: %lu(ms) Res: %dx%d\n",
                        prop.app_running_ms,
                        prop.delta_ms,
                        prop.width,
                        prop.height);

                /* Slow things down so its not 2000 fps
                 */

                #ifdef __linux__
                /* 30 FPS == 32000 microseconds. */
                usleep(32000);
                #endif
        };

        roa_platform_destroy(ctx);

        return 0;
}

