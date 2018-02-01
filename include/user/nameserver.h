#ifndef NAMESERVER_H
#define NAMESERVER_H

#include <syscalls.h>
#define NAMESERVER_SIZE 1024

typedef enum NSservice {
  WhoIs_t = 0,
  RegisterAs_t = 1,
  Stop_t = 2
} NSservice;

typedef struct NSrecord {
  //int name;
  int tid;
} NSrecord;

typedef struct NSreq {
  NSservice type;
  int name;
  //int tid;
} NSreq;

int RegisterAs(int n);

int WhoIs(int n);

void NameServerTask();
void StopNameServer();

#endif /*NAMESERVER_H*/
