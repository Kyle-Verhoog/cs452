#ifndef DEFINES_H
#define DEFINES_H

#define LOG_COM 1

#ifndef NULL
#define NULL  0
#endif
#define false 0
#define true  1

//Kernel Defines
#define UND_ENTRY 0x24
#define SWI_ENTRY 0x28
#define PRE_ENTRY 0x2c
#define DAT_ENTRY 0x30
#define IRQ_ENTRY 0x38

#define IRQ_STACK_BASE 0x100000
#define KERNEL_STACK_BASE 0x01000000 //0x01000000

#define USER_STACK_BASE 0x02000000
// #define USER_STACK_SIZE 0xf0000  //1 MB User stacks
// #define USER_STACK_SIZE 0x78000  // 512 KB User stacks
#define USER_STACK_SIZE 0x3c000  // 256 KB User stacks


//Kernel Modes
#define USER_MODE 16
#define IRQ_MODE 18
#define KERNEL_MODE 19
#define SYSTEM_MODE 31

// The number of tasks in the system
#define MAX_TASK  128
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
#define TERMINAL_MANAGER_ID 10
#define LOGGER_ID 11
#define WAITING_ROOM_ID 18
#define REPRESENTER_ID 19
#define TRAIN_PROVIDER_ID 20
#define TRAIN_PUBLISHER_ID 21
#define SWITCH_PROVIDER_ID 22
#define SWITCH_PUBLISHER_ID 23
#define SENSOR_PROVIDER_ID 26
#define SENSOR_PUBLISHER_ID 27
#define VIRTUAL_PROVIDER_ID 28
#define VIRTUAL_PUBLISHER_ID 29

#define RESERVATION_MANAGER_ID 30

//For Debugging
#define STOPPING_CALIBRATION_ID 60
#define STOP_AT_SERVER_ID 61

#define INT_MAX 2147483647

//Commands
#define COMMAND_SIZE 256

// Interrupt defines
#define VIC1_ENABLED VIC_TC1UI
#define VIC2_ENABLED VIC_TC3UI | VIC_INT_UART1 | VIC_INT_UART2


// Priorities

#define PRI_TRAIN_DRIVER 15

#endif // DEFINES_H

