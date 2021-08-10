#ifndef INCLUDED_0C45A081_7ECC_47A2_B928_4E34F404171C
#define INCLUDED_0C45A081_7ECC_47A2_B928_4E34F404171C

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Types and Identifiers
 */

struct roa_platform_ctx;

enum {
        ROA_PLATFORM_WINDOW_SIZE = 1 << 0,
        ROA_PLATFORM_WINDOW_CLOSED = 1 << 1,
};

/* -------------------------------------------------------------------------- */
/* Context Lifetime 
 */

struct roa_platform_desc {
        const char * title;
        int width;
        int height;
};

struct roa_platform_ctx*
roa_platform_create(
        const struct roa_platform_desc *desc);

void
roa_platform_destroy(
        const struct roa_platform_ctx *ctx);

struct roa_platform_poll_desc {
        int width;
        int height;
};

uint64_t
roa_platform_poll(
        struct roa_platform_ctx *ctx,
        const struct roa_platform_poll_desc *desc);

/* -------------------------------------------------------------------------- */
/* Platform Properties
 */

void
roa_platform_screen_size(
        struct roa_platform_ctx *ctx,
        int *out_x,
        int *out_y);

uint64_t
roa_platform_ms_delta(
        struct roa_platform_ctx *ctx);

uint64_t
roa_platform_ms_running(
        struct roa_platform_ctx *ctx);

/* -------------------------------------------------------------------------- */
/* Native Types
 * Connections to the native layer.
 */

#ifdef __linux
uintptr_t
roa_platform_details_xcb_window(
        struct roa_platform_ctx *ctx);

uintptr_t
roa_platform_details_xcb_connection(
        struct roa_platform_ctx *ctx);

#endif

#ifdef __cplusplus
} /* extern */
#endif

#endif

