#ifndef RESERVATION_H
#define RESERVATION_H

#include <stdlib.h>
#include <defines.h>
#include <lib/circular_buffer.h>
#include <lib/train/train_defines.h>
#include <lib/train/path_finding.h>

#ifdef X86
#include <assert.h>
#endif

typedef struct reservation {
  int train_num;
  bool reserved;
  int timestamp;
} reservation;

typedef struct pather {
  track_node track[TRACK_MAX];
  reservation reserv[TRACK_MAX];
  int nreservs[TRAIN_MAX];
} pather;

#define PATHER_Q_SIZE TRACK_MAX
CIRCULAR_BUFFER_DEC(tn_q, track_node *, PATHER_Q_SIZE);

void get_all_nodes_in_dist(track_node *node, int max_dist, tn_q *nodes);

void get_all_to_sen_in_dist(track_node *node, int min_dist, tn_q *nodes);

void reserv_init(reservation *r);

void pather_init(pather *p, track_node *TRACK);

void pather_path(pather *p, track_node *s, track_node *e);

void pather_recover(pather *p, int tn, track_node *new_pos, int stp_dist);

int pather_reserve_to_sensor(pather *p, int trn, track_node *next_pos, int dist);

int pather_reserve(pather *p, int tn, track_node *next_pos, int stp_dist);

int pather_free_before(pather *p, int trn, track_node *cur);

#endif
