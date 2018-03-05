#ifndef LOGGER_H
#define LOGGER_H

#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <user/ui/idle_time.h>

#define LOG_CLEAR_COUNT LOG_HEIGHT - 2

void Logger();
#endif
