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
#include <lib/train/track_data.h>
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

typedef struct sensor {
  int conf;
} sensor;

typedef struct train {
  // speed model
  int num;               // train num
  int gear;              // train gear setting
  int len;               // the length of the train
  pos_event next_pos;
  pos_event curr_pos;
  pp_list   prev_pos;     // previous positions train has visited
} train;

#define INC_TR_LIST_SIZE NUM_TRAINS
CIRCULAR_BUFFER_DEC(inc_tr_list, train *, INC_TR_LIST_SIZE);

#define TR_AT_LIST_SIZE NUM_TRAINS
EXT_CIRCULAR_BUFFER_DEC(tr_at_list, train *, TR_AT_LIST_SIZE);

typedef struct estimator {
  train train[NUM_TRAINS];
  swi sw[SWITCH_SIZE];
  sensor sensor[SENSOR_SIZE];
  inc_tr_list inc_tr[SENSOR_SIZE]; // trains incoming to a sensor
  tr_at_list tr_at[TRACK_MAX];     // trains at a particular track node sorted descending distance from the node
  int tmap[TRAIN_MAX];
  int ntrains;                     // number of trains being tracked
  int last_ts;                     // timestamp of the last estimate update
} estimator;


void est_init(estimator *estr);

int est_last_ts(estimator *est);

int est_add_tr(estimator *est, int tr_num, pos_event *pe);

int est_update_tr_at(estimator *est, pos_event *pe);

int est_update_tr_gear(estimator *est, int tr_num, int gear);

int est_update_sw(estimator *est, int sw_num, int state);

int est_update_rse(estimator *est, int se_num, int state);

pos_event *est_tr_curr_pos(estimator *est, int tr_num);

pos_event *est_tr_next_pos(estimator *est, int tr_num);

int est_update(estimator *est, int ts);

#endif
