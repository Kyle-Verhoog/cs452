#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include <defines.h>
#include <lib/circular_buffer.h>

#ifndef X86
#include <user/syscalls.h>
#else
#include <assert.h>
#define assertf(cond, ...) assert(cond)
#endif

#include <lib/train/train_defines.h>
#include <lib/train/track_node.h>
#include <lib/train/switch.h>


typedef struct pos_event {
  track_node *pos;   // position
  int off;           // dist offset from pos
  int ts;            // timestamp of the event
} pos_event;

#define PREV_POS_LIST_SIZE 16
CIRCULAR_BUFFER_DEC(pp_list, pos_event, PREV_POS_LIST_SIZE);


typedef struct swi {
  int state;
  int conf;
} swi;

typedef struct train {
  // speed model
  int num;               // train num
  int gear;              // train gear setting
  pos_event next_pos;
  pos_event curr_pos;
  pp_list   prev_pos;     // previous positions train has visited
} train;

#define INC_TR_LIST_SIZE NUM_TRAINS
CIRCULAR_BUFFER_DEC(inc_tr_list, train *, INC_TR_LIST_SIZE);

typedef struct estimator {
  train train[NUM_TRAINS];
  swi sw[SWITCH_SIZE];
  inc_tr_list inc_tr[SENSOR_SIZE]; // trains incoming to a sensor
  int tmap[TRAIN_MAX];
  int ntrains;                     // number of trains being tracked
  int last_ts;                     // timestamp of the last estimate update
} estimator;


void est_init(estimator *estr);

int est_last_ts(estimator *est);

int est_add_tr(estimator *est, int tr_num, pos_event *pe);

pos_event *est_tr_curr_pos(estimator *est, int tr_num);

pos_event *est_tr_next_pos(estimator *est, int tr_num);

int est_update(estimator *est, int ts);

#endif
