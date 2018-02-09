#include <lib/nameserver_store.h>


void nss_init(nameserver_store *nss) {
  int i;
  for (i = 0; i < NS_NUM_ENTRIES; ++i)
    nss->entries[i] = NS_DNE;
  nss->size = 0;
}

int nss_get(nameserver_store *nss, nskey_t key) {
  KASSERT(0 <= key && key < NS_NUM_ENTRIES);
  if (nss->size <= 0)
    return NS_EMPTY;
  return nss->entries[key];
}

int nss_set(nameserver_store *nss, nskey_t key, tid_t tid) {
  KASSERT(0 <= key && key < NS_NUM_ENTRIES);
  if (nss->size == NS_NUM_ENTRIES-1)
    return NS_FULL;

  nss->size++;
  nss->entries[key] = tid;
  return 0;
}
