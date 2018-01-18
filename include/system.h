/**
 * Modified based off of freeBSD source
 */

#include <bwio.h>

#define LOG_COM 1

#define STR_HELPER(x) #x

#define STR(x) STR_HELPER(x)

void  kassert_panic(const char *fmt, ...);

#define __builtin_expect(a, b) (a)

#define __predict_false(exp) __builtin_expect((exp), 0)

#define KASSERT(exp) do {       \
  if (__predict_false(!(exp)))   \
    bwprintf(LOG_COM, "ASSERT FAILED <%s:%d>\r\n", __FILE__, __LINE__);           \
} while (0)

#define KASSERTF(exp, msg) do {       \
  if (__predict_false(!(exp))) {      \
    bwprintf(LOG_COM, "ASSERT FAILED <%s:%d>\r\n", __FILE__, __LINE__);           \
    kassert_panic msg;              \
  }                                 \
} while (0)


#define KERNEL_END()
  __asm__("KERNEL_END: ");
