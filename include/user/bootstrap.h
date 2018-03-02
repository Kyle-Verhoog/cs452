#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <io.h>
#include <user/syscalls.h>
#include <user/clockserver.h>
#include <user/ioserver.h>
#include <user/nameserver.h>
#include <user/terminal/terminal_manager.h>
#include <user/ui/timer_interface.h>
#include <user/train/sensor_manager.h>
#include <user/train/switch_manager.h>

void Bootstrap();
#endif
