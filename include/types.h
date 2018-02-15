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

#define INTERRUPT_SIZE 1024

typedef enum InterruptEvent{
	IE_TC1UI = 4,
	IE_TC3UI = 51,
  IE_UART2 = 54,
  IE_UART2_BASE = 540, // Note: these values are special! Do not change!!
  IE_UART2_RX   = 541,
  IE_UART2_TX   = 542,
  IE_UART2_RT   = 543,
  IE_UART2_MI   = 544,
  IE_UART3 = 55,
  IE_UART3_BASE = 550, // Note: these values are special! Do not change!!
  IE_UART3_RX   = 551,
  IE_UART3_TX   = 552,
  IE_UART3_RT   = 553,
  IE_UART3_MI   = 554,
} InterruptEvent;

#define IE_UART_RX_OFFSET 1
#define IE_UART_TX_OFFSET 2
#define IE_UART_RT_OFFSET 3
#define IE_UART_MI_OFFSET 4

typedef int32_t tid_t;
#define TID_ID(tid) (tid & 0xffff)
typedef short int tid_id_t;
typedef short int tid_ver_t;

#endif /* TYPES_H */
