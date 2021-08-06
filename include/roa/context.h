#ifndef INCLUDED_0C45A081_7ECC_47A2_B928_4E34F404171C
#define INCLUDED_0C45A081_7ECC_47A2_B928_4E34F404171C

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Types
 */

struct roa_ctx;

/* -------------------------------------------------------------------------- */
/* Lifetime
 */

struct roa_ctx_desc {
        const char * title;
        int width;
        int height;
};

struct roa_ctx*
roa_ctx_create(
        const struct roa_ctx_desc *desc);

void
roa_ctx_destroy(
        const struct roa_ctx *ctx);

/* -------------------------------------------------------------------------- */
/* Platform Interface 
 */

enum {
        ROA_EVT_WINDOW_SIZE = 1 << 0,
};

struct roa_ctx_poll_desc
{
        int width;
        int height;
};

uint64_t
roa_ctx_poll(
        struct roa_ctx *ctx,
        const struct roa_ctx_poll_desc *desc);

void
roa_ctx_screen_size(
        struct roa_ctx *ctx,
        int *out_x,
        int *out_y);

#ifdef __cplusplus
} /* extern */
#endif

#endif

