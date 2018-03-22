#ifndef RESERVATION_MANAGER_H
#define RESERVATION_MANAGER_H

#include <defines.h>
#include <user/syscalls.h>
#include <lib/train/reservation.h>
#include <user/terminal_manager.h>

typedef enum RMReqType {
  RM_RESERVE,
  RM_FREE,
  RM_GEN,
} RMReqType;

typedef struct RMReq {
  RMReqType type;
  int tr_num;
  track_node *node;
  int dist;
  path *p;
} RMReq;


typedef enum RMReplyType {
  RM_SUCCESS,
  RM_CONFLICT,
} RMReplyType;

typedef struct RMReply {
  RMReplyType ret;
} RMReply;

void ReservationManager();

int Reserve(tid_t rm_tid, int tr_num, track_node *s, int dist);

int Free(tid_t rm_tid, int tr_num, track_node *s);

int PathFind(tid_t rm_tid, int tr_num, path *p);

#endif
