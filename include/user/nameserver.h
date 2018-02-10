#ifndef NAMESERVER_H
#define NAMESERVER_H

#include <kernel/syscalls.h>
#include <lib/nameserver_store.h>

#define NAMESERVER_SIZE 512

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
