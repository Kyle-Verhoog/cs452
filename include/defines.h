#ifndef DEFINES_H
#define DEFINES_H

#define LOG_COM 0

#ifndef NULL
#define NULL  0
#endif
#define false 0
#define true  1

//Kernel Defines
#define SWI_ENTRY 0x28
#define IRQ_ENTRY 0x38

#define IRQ_STACK_BASE 0x100000
#define KERNEL_STACK_BASE 0x01000000 //0x01000000

#define USER_STACK_BASE 0x02000000
#define USER_STACK_SIZE 0xf0000  //1 MB User stacks

//Kernel Modes
#define USER_MODE 16
#define IRQ_MODE 18
#define KERNEL_MODE 19 
#define SYSTEM_MODE 31 

// The number of tasks in the system
#define MAX_TASK  32
#define MAX_PRIORITY 32
#define NS_MAX_ENTRY 64
#define CSQ_MAX_SIZE MAX_TASK
#define IRQ_MAX_SIZE MAX_TASK


// User Task ID defines
#define CLOCKSERVER_ID 2
#define IOSERVER_UART1_RX_ID 3
#define IOSERVER_UART1_TX_ID 4
#define IOSERVER_UART2_RX_ID 5
#define IOSERVER_UART2_TX_ID 6
#define READERSERVICE_UART2_ID 7
#define WRITERSERVICE_UART1_ID 8
#define WRITERSERVICE_UART2_ID 9
#define RAILWAY_MANAGER_ID 20
#define TRAIN_MANAGER_ID 21
#define SWITCH_MANAGER_ID 22
#define SENSOR_MANAGER_ID 23

//Commands
#define COMMAND_SIZE 256

#endif // DEFINES_H

