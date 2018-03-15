#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <io.h>
#include <user/syscalls.h>
#include <user/clockserver.h>
#include <user/ioserver.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <user/logger.h>
#include <user/init/track_data.h>
#include <user/ui/train_track.h>
#include <user/ui/timer_interface.h>
#include <user/ui/num_procs.h>
#include <user/ui/mem_usage.h>
#include <user/ui/idle_time.h>
#include <user/train/railway_manager.h>

#include <stoppingcalibration_test.h>	//TODO: REMOVE THIS
#include <stop_at_test.h>	//TODO: REMOVE THIS

void Bootstrap();
#endif
