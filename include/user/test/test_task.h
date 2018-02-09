#ifndef TEST_TASKS_H
#define TEST_TASKS_H

#include <types.h>
#include <kernel/syscalls.h>
#include <io/io.h>
#include <user/test/test_defines.h>
#include <taskid_test.h>
#include <user/test/taskid_test.h>
#include <user/test/clockserver_test.h>
#include <user/test/context_switch_test.h>
#include <user/test/nameserver_test.h>
#include <schedule_test.h>

void TestTask();

#endif /*TEST_TASKS_H*/

