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

struct roa_platform_properties {
        int width;
        int height;
        uint64_t delta_ms;
        uint64_t app_running_ms;
};

void
roa_platform_properties(
        struct roa_platform_ctx *ctx,
        struct roa_platform_properties *out_props);

struct roa_platform_native_properties {
        #ifdef __linux__
        uintptr_t xcb_window;
        uintptr_t xcb_connection;
        #else
        int unused;
        #endif
};

void
roa_platform_native_properties(
        struct roa_platform_ctx *ctx,
        struct roa_platform_native_properties *out_props);

#ifdef __cplusplus
} /* extern */
#endif

#endif

