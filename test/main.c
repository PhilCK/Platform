#include <roa/platform.h>
#include <assert.h>

int
main()
{
        struct plt_ctx *ctx = 0;

        ctx = plt_create();
        assert(ctx);

        uint64_t evts = 0;
        while(evts = plt_update(ctx), evts) {
               /* checks evts */
        };

        plt_destroy(&ctx);
        assert(ctx == 0);


        return 0;
}
