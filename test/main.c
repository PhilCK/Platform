#include <assert.h>
#include <roa/platform.h>
#include <stddef.h>
#include <xcb/xcb.h>
#include <stdio.h>

int
main()
{
        printf("ROA Platform Test\n");

        struct roa_ctx *ctx = roa_ctx_create(&(struct roa_ctx_desc) {
                .width = 1280,
                .height = 800,
                .title = "ROA Test",
        });

        uint64_t evt = 0;

        while(!(evt & ROA_PLATFORM_WINDOW_CLOSED)) {
                evt = roa_ctx_poll(ctx, NULL);
        };

        roa_ctx_destroy(ctx);

        return 0;
}
