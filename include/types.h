#ifndef TYPES_H
#define TYPES_H

typedef int bool;
typedef signed int int32_t;
typedef unsigned int uint32_t;

#ifndef X86
typedef uint32_t size_t;
#else
#include <stddef.h>
#endif

typedef enum InterruptEvent{
	IE_TC1UI = 4,
	IE_TC3UI = 51
} InterruptEvent;

typedef int32_t tid_t;
#define TID_ID(tid) (tid & 0xffff)
typedef short int tid_id_t;
typedef short int tid_ver_t;

#endif /* TYPES_H */
