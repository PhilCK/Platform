#ifndef INCLUDED_85B195F3_EEC9_49F2_B3CB_EF7FFF6CDE0E
#define INCLUDED_85B195F3_EEC9_49F2_B3CB_EF7FFF6CDE0E

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Opaque Types and Other Bits
 */

struct plt_ctx;

#define PLT_VER_MAJOR 0
#define PLT_VER_MINOR 0
#define PLT_VER_PATCH 0

enum plt_events {
        PLT_EVT_WIN_EXISTS = 1 << 0,
};

/* -------------------------------------------------------------------------- */
/* Lifetime
 */

struct plt_ctx*
plt_create();


void
plt_destroy(
        struct plt_ctx **ctx);

uint64_t
plt_update(
        struct plt_ctx *ctx); 

/* -------------------------------------------------------------------------- */
/* Window
 */

void
plt_win_size(
        struct plt_ctx *ctx,
        int *w,
        int *h);

void
plt_win_fullscreen(
        struct plt_ctx *ctx,
        int *f);

/* -------------------------------------------------------------------------- */
/* Platform
 */

#ifdef _WIN32
#ifndef _WINDEF_
struct HWND__; // Forward or never
typedef struct HWND__* HWND;
#endif

HWND
plt_win_hwnd();
#endif

#ifdef __cplusplus
}
#endif

#endif
