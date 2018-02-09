#include <test/nameserver.h>

void nss_sanity() {
  nameserver_store nss;
  nss_init(&nss);
  assert(nss.size == 0);
}

void nss_set_1() {
  int ret;
  nameserver_store nss;
  nss_init(&nss);

  ret = nss_set(&nss, 4, 0);
  assert(ret == 0);
  assert(nss.size == 1);
}

void nss_set_2() {
  int ret;
  nameserver_store nss;
  nss_init(&nss);

  ret = nss_set(&nss, 4, 0);
  assert(ret == 0);
  ret = nss_set(&nss, 4, 0);
  assert(ret == 0);
  assert(nss.size == 2);
  ret = nss_set(&nss, 7, 0);
  assert(ret == 0);
  assert(nss.size == 3);
}

void nss_get_1() {
  int ret;
  nameserver_store nss;
  nss_init(&nss);

  ret = nss_set(&nss, 4, 0);
  assert(ret == 0);
  ret = nss_get(&nss, 4);
  assert(ret == 0);

  ret = nss_set(&nss, 8, 1);
  assert(ret == 0);
  ret = nss_get(&nss, 8);
  assert(ret == 1);

  ret = nss_set(&nss, 8, 2);
  assert(ret == 0);
  ret = nss_get(&nss, 8);
  assert(ret == 2);
}

void nss_get_2() {
  int ret;
  nameserver_store nss;
  nss_init(&nss);

  ret = nss_get(&nss, 8);
  assert(ret == NS_EMPTY);

  ret = nss_set(&nss, 8, 0);
  ret = nss_get(&nss, 9);
  assert(ret == NS_DNE);
}

void nameserver_tests() {
  nss_sanity();
  nss_set_1();
  nss_set_2();
  nss_get_1();
  nss_get_2();
}
