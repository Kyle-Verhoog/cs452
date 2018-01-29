#ifndef K2_METRIC_H
#define K2_METRIC_H
#include <bwio.h>
#include <syscalls.h>
#include <ts7200.h>

#define STRING_64 "012345678901234567890123456789012345678901234567890123456789012"

//All Tasks
void K2InitMetricTask();
void Sender4();
void Receiver4();
void Sender64();
void Receiver64();


#endif /* K2_METRIC_H */
