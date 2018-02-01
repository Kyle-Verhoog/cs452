#ifndef SYSTEM_H
#define SYSTEM_H
/**
 * Modified based off of freeBSD source
 */

#include <lib/va_arg.h>

#ifdef DEBUG
#include <io.h>
#endif

#include <defines.h>

#define STR_HELPER(x) #x

#define STR(x) STR_HELPER(x)

#define __builtin_expect(a, b) (a)

#define __predict_false(exp) __builtin_expect((exp), 0)

#define is_set(macro) is_set_(macro)
#define macrotest_1 ,
#define is_set_(value) is_set__(macrotest_##value)
#define is_set__(comma) is_set___(comma 1, 0)
#define is_set___(_, v, ...) v


#ifdef DEBUG
#define KASSERT(exp) \
  do {                                                 \
    if (__predict_false(!(exp))) {                     \
      PRINTF(                                          \
          "\033[31m"                                   \
          "ASSERTION '"STR(exp)"' FAILED <%s:%d>\r\n"  \
          "\033[0m",                                   \
          __FILE__,                                    \
          __LINE__                                     \
              );                                       \
      KABORT();                                        \
    }                                                  \
  } while (0)
#else
#define KASSERT(exp) {}
#endif


// TODO: figure out a way to exit cleanly??
#define KABORT() \
  __asm__( \
           ".extern kernel_stack_base;"  \
           "ldr r1, =kernel_stack_base;" \
           "ldr r1, [r1];"               \
           "sub r1, r1, #72;"            \
           "ldmfd r1, {sp, pc};"         \
         );


extern void kpanic(const char *fmt, ...);

#endif /* SYSTEM_H */
