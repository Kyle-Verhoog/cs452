#ifndef NAMESERVER_STORE_H
#define NAMESERVER_STORE_H

#include <stdlib.h>

#define NS_NUM_ENTRIES 64

#define NS_NONE   0
#define NS_DNE   -1
#define NS_EMPTY -2
#define NS_FULL  -3

typedef int nskey_t;

typedef struct nameserver_store {
  tid_t entries[NS_NUM_ENTRIES];
  int size;
} nameserver_store;


void nss_init(nameserver_store *nss);

int nss_get(nameserver_store *nss, nskey_t key);

int nss_set(nameserver_store *nss, nskey_t key, tid_t tid);

#endif /* NAMESERVER_STORE_H */
