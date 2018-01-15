#ifndef KERNEL_H
#define KERNEL_H

#include <bwio.h>
#include <ts7200.h>

//Data Structures
#include <circularbuffer.h>

//Debug - set DEBUG_ON through the gcc option (-D DEBUG_ON)
#ifdef DEBUG_ON
	#include <debug.h>
#endif /* DEBUG_ON */

#define KERNEL_ENTRY 0x28

#endif /* KERNEL_H */
