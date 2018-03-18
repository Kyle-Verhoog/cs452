#ifndef TRAIN_H
#define TRAIN_H


#define IS_VALID_TR_NUM(x) (x >= 1 && x <= 81)

typedef struct Train {
  int num;
  int gear;
  int dir; //forward is 1
  bool exist;
  track_node *pos;
  int speed;
  int timestamp;
} Train;

#endif
