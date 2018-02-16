#ifndef UI_TEST_H
#define UI_TEST_H

#include <io.h>
#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/clockserver.h>
#include <ioserver.h>
#include <user/test/test_defines.h>
#include <readerservice.h>
#include <writerservice.h>
#include <timer_interface.h>

//Train Services
#include <railway_manager.h>
#include <train_manager.h>
#include <switch_manager.h>
#include <sensor_manager.h>

void UITest();

#endif //UI_TEST_H
