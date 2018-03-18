#ifndef NAMESERVER_H
#define NAMESERVER_H

#include <user/syscalls.h>
#include <lib/circular_buffer.h>
#include <lib/nameserver_store.h>

#define NS_MAX_TASK_QUEUE_SIZE 5

CIRCULAR_BUFFER_DEC(ns_q, tid_t, NS_MAX_TASK_QUEUE_SIZE);

typedef enum NSService {
  NS_WHOIS = 0,
  NS_REGISTERAS = 1,
  NS_STOP = 2
} NSService;

typedef struct NSReq {
  NSService type;
  int name;
} NSReq;

int RegisterAs(int n);

tid_t WhoIs(int n);

void NameServer();

void NameServerStop();

#endif /* NAMESERVER_H */
