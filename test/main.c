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
                
                printf("Time %lu, Delta %lu\n",
                        roa_platform_ms_running(ctx),
                        roa_platform_ms_delta(ctx));

                /* Slow things down so its not 2000 fps
                 */

                #ifdef __linux__
                /* 60 FPS == 16000 microseconds. */
                usleep(16000);
                #endif
        };

        roa_platform_destroy(ctx);

        return 0;
}
