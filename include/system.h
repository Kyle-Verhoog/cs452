/**
 * Modified based off of freeBSD source
 */

#include <bwio.h>
#include <defines.h>

#define STR_HELPER(x) #x

#define STR(x) STR_HELPER(x)

#define __builtin_expect(a, b) (a)

#define __predict_false(exp) __builtin_expect((exp), 0)

#define KASSERTF(exp, msg) if (DEBUG) do {         \
  if (__predict_false(!(exp))) {                   \
    bwprintf(                                      \
      LOG_COM,                                     \
      "\033[31m"                                   \
      "ASSERTION '"STR(exp)"' FAILED <%s:%d>\r\n"  \
      "\033[0m",                                   \
      __FILE__,                                    \
      __LINE__                                     \
    );                                             \
    kpanic msg;                                    \
  }                                                \
} while (0)


#define KASSERT(exp) if (DEBUG) do {               \
  if (__predict_false(!(exp))) {                   \
    bwprintf(                                      \
      LOG_COM,                                     \
      "\033[31m"                                   \
      "ASSERTION '"STR(exp)"' FAILED <%s:%d>\r\n"  \
      "\033[0m",                                   \
      __FILE__,                                    \
      __LINE__                                     \
    );                                             \
    KABORT();                                      \
  }                                                \
} while (0)


// TODO: figure out a way to exit cleanly??
#define KABORT() for(;;);


extern void kpanic(const char *fmt, ...);
