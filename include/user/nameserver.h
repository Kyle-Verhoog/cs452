#ifndef NAMESERVER_H
#define NAMESERVER_H

#include <kernel/syscalls.h>
#define NAMESERVER_SIZE 1024

typedef enum NSservice {
  WhoIs_t = 0,
  RegisterAs_t = 1,
  Stop_t = 2
} NSservice;

typedef struct NSrecord {
  //int name;
  tid_t tid;
} NSrecord;

typedef struct NSreq {
  NSservice type;
  int name;
  //int tid;
} NSreq;

int RegisterAs(int n);

tid_t WhoIs(int n);

void NameServer();

void NameServerStop();

#endif /*NAMESERVER_H*/
