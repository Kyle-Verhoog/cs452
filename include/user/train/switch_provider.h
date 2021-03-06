#ifndef SWITCH_PROVIDER_H
#define SWITCH_PROVIDER_H

#include <ts7200.h>
#include <defines.h>
#include <train_defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <writerservice.h>
#include <track_data.h>
#include <events.h>

void SwitchProvider();
void init_switch();


typedef struct SWSubscribe{
	SW_Request swr;
	RawEvent re;
}SWSubscribe;


#define TURN_SOLENOID_OFF 32

#define SWITCH_TABLE_ROW 3
#define SWITCH_TABLE_COL 1
#define SWITCH_TABLE_STRING \
"+--------+--------+\n\r\
| SWITCH | STATUS |\n\r\
+-----------------+\n\r\
|  1     |        |\n\r\
|  2     |        |\n\r\
|  3     |        |\n\r\
|  4     |        |\n\r\
|  5     |        |\n\r\
|  6     |        |\n\r\
|  7     |        |\n\r\
|  8     |        |\n\r\
|  9     |        |\n\r\
| 10     |        |\n\r\
| 11     |        |\n\r\
| 12     |        |\n\r\
| 13     |        |\n\r\
| 14     |        |\n\r\
| 15     |        |\n\r\
| 16     |        |\n\r\
| 17     |        |\n\r\
| 18     |        |\n\r\
+-----------------+\n\r\
| 0x99   |        |\n\r\
| 0x9A   |        |\n\r\
| 0x9B   |        |\n\r\
| 0x9C   |        |\n\r\
+--------+--------+"

#endif //SWITCH_PROVIDER_H
