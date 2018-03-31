#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/switch.h>

CIRCULAR_BUFFER_DEF(bfs_q, track_node *, BFS_Q_SIZE);

int dist_to_node(track_node *node, track_node *dest) {
  int r, d;
  int dist[TRACK_MAX];
  bfs_q q;
  track_node *next;
  bfs_q_init(&q);

  dist[node->id] = 0;
  r = bfs_q_push(&q, node);
  d = BFS_Q_SIZE;

  while (q.size > 0 && d-- > 0) {
    r = bfs_q_pop(&q, &node);
    if (r) return -1;

    if (node == dest)
      return dist[node->id];

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_CURVED].dist + dist[node->id];
      if (r) return -1;
      next = node->edge[DIR_STRAIGHT].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_STRAIGHT].dist + dist[node->id];
      if (r) return -1;
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_AHEAD].dist + dist[node->id];
      if (r) return -1;
    }
  }

  return -1;
}


// does a BFS to a depth of `sensor_depth` sensors (since we can assume at most
// 1 sensor failure)
bool node_nearby_sd(track_node *node, track_node *dest, int sensor_depth) {
  int r;
  int nse[TRACK_MAX];
  bfs_q q;
  track_node *next;
  bfs_q_init(&q);

  nse[node->id] = 0;
  r = bfs_q_push(&q, node);

  while (q.size > 0) {
    r = bfs_q_pop(&q, &node);
    if (r) return 0;

    if (node == dest)
      return true;

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_CURVED].dest;
      nse[next->id] = nse[node->id];
      r = bfs_q_push(&q, next);
      if (r) return 0;

      next = node->edge[DIR_STRAIGHT].dest;
      nse[next->id] = nse[node->id];
      r = bfs_q_push(&q, next);
      if (r) return 0;
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else if (node->type == NODE_SENSOR) {
      next = node->edge[DIR_AHEAD].dest;
      if (nse[node->id] + 1 > sensor_depth)
        continue;
      nse[next->id] = nse[node->id] + 1;
      r = bfs_q_push(&q, next);
      if (r) return 0;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      nse[next->id] = nse[node->id];
      r = bfs_q_push(&q, next);
      if (r) return 0;
    }
  }

  return false;
}

CIRCULAR_BUFFER_DEF(poss_node_list, PossibleSensor, POSSIBLE_NODE_LIST_SIZE);

int next_poss_sensors(track_node *node, int start_dist, poss_node_list *pnl) {
  int r, d;
  int dist[TRACK_MAX];
  bfs_q q;
  track_node *next;
  PossibleSensor sensor;

  bfs_q_init(&q);

  dist[node->id] = start_dist;
  r = bfs_q_push(&q, node);
  d = BFS_Q_SIZE;

  while (q.size > 0 && d-- > 0) {
    r = bfs_q_pop(&q, &node);
    if (r) return -1;

    if (node->type == NODE_BRANCH) {
      next = node->edge[DIR_STRAIGHT].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_STRAIGHT].dist + dist[node->id];
      if (r) return -1;
      next = node->edge[DIR_CURVED].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_CURVED].dist + dist[node->id];
      if (r) return -1;
    }
    else if (node->type == NODE_SENSOR) {
      sensor.node = node;
      sensor.dist = dist[node->id];
      r = poss_node_list_push(pnl, sensor);
      if (r) return -1;
    }
    else if (node->type == NODE_EXIT) {
      continue;
    }
    else {
      next = node->edge[DIR_AHEAD].dest;
      r = bfs_q_push(&q, next);
      dist[next->id] = node->edge[DIR_AHEAD].dist + dist[node->id];
      if (r) return -1;
    }
  }

  return -1;
}

int GetNextSensorINC(Switch *sw, track_node *n, PossibleSensor *pos){
  pos->dist = 0;
  track_node *node = n;

  while(node->type != NODE_SENSOR){
    if(node->type == NODE_EXIT){
      return -1;
    }
    else if(node->type == NODE_BRANCH){
      if(sw[node->num].state == SW_STRAIGHT){
        pos->dist += node->edge[DIR_STRAIGHT].dist;
        node = node->edge[DIR_STRAIGHT].dest;
      }
      else{
        pos->dist += node->edge[DIR_CURVED].dist;
        node = node->edge[DIR_CURVED].dest;
      }
    }
    else{
      pos->dist += node->edge[DIR_AHEAD].dist;
      node = node->edge[DIR_AHEAD].dest;
    }
  }

  pos->node = node;
  return 0;
}

int GetNextSensorEXC(Switch *sw, track_node *n, PossibleSensor *pos){
  pos->dist = 0;
  track_node *node = n;

  do{
    if(node->type == NODE_EXIT){
      return -1;
    }
    else if(node->type == NODE_BRANCH){
      if(sw[node->num].state == SW_STRAIGHT){
        pos->dist += node->edge[DIR_STRAIGHT].dist;
        node = node->edge[DIR_STRAIGHT].dest;
      }
      else{
        pos->dist += node->edge[DIR_CURVED].dist;
        node = node->edge[DIR_CURVED].dest;
      }
    }
    else{
      pos->dist += node->edge[DIR_AHEAD].dist;
      node = node->edge[DIR_AHEAD].dest;
    }
  }while(node->type != NODE_SENSOR);

  pos->node = node;
  return 0;
}

int GetPrevSensorINC(Switch *sw, track_node *n, PossibleSensor *pos){
  return GetNextSensorINC(sw, n->reverse, pos);
}

int GetPrevSensorEXC(Switch *sw, track_node *n, PossibleSensor *pos){
  return GetNextSensorEXC(sw, n->reverse, pos);
}

int dist_between_nodes(Switch *sw, track_node *start, track_node *end){
  int dist = 0;
  track_node *n = start;

  while(n != end){
    if(n->type == NODE_EXIT){
     return -1;
    }
    else if(n->type == NODE_BRANCH){
     if(sw[n->num].state == SW_STRAIGHT){
       dist += n->edge[DIR_STRAIGHT].dist;
       n = n->edge[DIR_STRAIGHT].dest;
     }
     else{
       dist += n->edge[DIR_CURVED].dist;
       n = n->edge[DIR_CURVED].dest;
     }
    }
    else{
     dist += n->edge[DIR_AHEAD].dist;
     n = n->edge[DIR_AHEAD].dest;
    }

    if (dist < 0 || dist > 50000)
      return -1;
  }

  return dist;
}

