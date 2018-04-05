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
#include <lib/train/train_model.h>


typedef struct pos_event {
  track_node *pos;   // position
  int off;           // dist offset from pos
  int ts;            // timestamp of the event
} pos_event;

#define PREV_POS_LIST_SIZE 16
CIRCULAR_BUFFER_DEC(pp_list, pos_event, PREV_POS_LIST_SIZE);

typedef struct swi {
  int state;
  int last_known_state;
  int conf;
} swi;

typedef struct sensor {
  int conf;
} sensor;

typedef struct speed_info {
  int speed;
  int gear;
  int start_gear;
  int stop_gear;
  int duration;
  int elapsed;
  int dist;
} acc_info;

typedef struct train {
  int num;               // train num
  int gear;              // train gear setting
  int len;               // the length of the train
  pos_event next_pos;
  pos_event curr_pos;
  // pp_list   prev_pos;     // previous positions train has visited
  int       next_sen_eta; // next sensor estimated arrival time
  track_node *next_sen;   // the next sensor this train is expected to hit
  TrainModel  s_model;
} train;

typedef struct train_crumb {
  train *train;
  int ts;
  int speed;
  // speed/acceleration information
} train_crumb;

typedef struct tn_crumbs {
  train_crumb crumb[NUM_TRAINS];
  int num;
} tn_crumbs;

#define INC_TR_LIST_SIZE NUM_TRAINS
CIRCULAR_BUFFER_DEC(sen_reg_list, train *, INC_TR_LIST_SIZE);

#define TN_LIST_SIZE 16
CIRCULAR_BUFFER_DEC(tn_list, track_node *, TN_LIST_SIZE);

#define TR_AT_LIST_SIZE NUM_TRAINS
EXT_CIRCULAR_BUFFER_DEC(tr_at_list, train *, TR_AT_LIST_SIZE);

// typedef struct train_register {
//   train *train;
//   int    conf;  // confidence we have in event actually happening
// } train_reg;

typedef struct estimator {
  train train[NUM_TRAINS];
  swi sw[SWITCH_SIZE];
  sensor sensor[SENSOR_SIZE];
  sen_reg_list sen_reg[SENSOR_SIZE]; // trains passed a sensor in the model, but not irl
  tr_at_list tr_at[TRACK_MAX];     // trains at a particular track node sorted descending distance from the node
  int tmap[TRAIN_MAX];
  tn_crumbs crumb[TRACK_MAX];
  int ntrains;                     // number of trains being tracked
  int last_ts;                     // timestamp of the last estimate update
} estimator;


int tr_at_list_insert(tr_at_list *tr_at, train *tr);

void est_init(estimator *estr);

void est_init_trains(estimator *est, int ts, track_node *TRACK, int track);

int est_last_ts(estimator *est);

int est_add_tr(estimator *est, int tr_num, pos_event *pe);

int est_update_tr_at(estimator *est, pos_event *pe);

int est_update_tr_gear(estimator *est, int tr_num, int gear, int ts);

int est_update_sw(estimator *est, int sw_num, int state, int ts);

int est_update_rse(estimator *est, int se_num, int state);

pos_event *est_tr_curr_pos(estimator *est, int tr_num);

pos_event *est_tr_next_pos(estimator *est, int tr_num);

int est_update(estimator *est, int ts);

train *est_get_train(estimator *est, int tr_num);
#endif
