#ifndef TEST_H
#define TEST_H

#include <io.h>
#include <assert.h>

#define PTEST_START(msg) { PRINTF("| %-20s |", msg); }
#define PTEST_END() PRINTF(" ✓ |\n")
#endif /* TEST_H */
