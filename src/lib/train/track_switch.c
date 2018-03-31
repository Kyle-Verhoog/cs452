#include <lib/train/track_switch.h>

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
