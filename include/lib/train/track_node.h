#ifndef TRACK_NODE_H
#define TRACK_NODE_H

#include <lib/circular_buffer.h>

#ifndef X86
#include <user/syscalls.h>
#else
#include <assert.h>
#define assertf(cond, ...) assert(cond)
#endif

typedef enum {
  NODE_NONE,
  NODE_SENSOR,
  NODE_BRANCH,
  NODE_MERGE,
  NODE_ENTER,
  NODE_EXIT,
} node_type;

#define DIR_AHEAD 0
#define DIR_STRAIGHT 0
#define DIR_CURVED 1

struct track_node;
typedef struct track_node track_node;
typedef struct track_edge track_edge;

struct track_edge {
  track_edge *reverse;
  track_node *src, *dest;
  int dist;             /* in millimetres */
};

struct track_node {
  int reserver;
  const char *name;
  node_type type;
  int id;
  int num;              /* sensor or switch number */
  track_node *reverse;  /* same location, but opposite direction */
  track_edge edge[2];
};

#define BFS_Q_SIZE 16
CIRCULAR_BUFFER_DEC(bfs_q, track_node *, BFS_Q_SIZE);


typedef struct PossibleSensor {
  int dist;
  track_node *node;
} PossibleSensor;

#define POSSIBLE_NODE_LIST_SIZE 5
CIRCULAR_BUFFER_DEC(poss_node_list, PossibleSensor, POSSIBLE_NODE_LIST_SIZE);


int dist_to_node(track_node *node, track_node *dest);

bool node_nearby_sd(track_node *node, track_node *dest, int sensor_depth);

int next_poss_sensors(track_node *node, int dist, poss_node_list *pnl);

#endif //TRACK_NODE_H
