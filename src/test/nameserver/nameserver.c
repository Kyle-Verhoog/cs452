#include <test/nameserver.h>

void ns_sanity() {
  NSrecord NameServer[NAMESERVER_SIZE];

  NSrecord firstRecord;
  firstRecord.tid = 11;

  NameServer[1] = firstRecord;
  NameServer[2].tid = 22;

  assert(NameServer[1].tid == 11);
  assert(NameServer[2].tid == 22);
}

void nameserver_tests() {
  ns_sanity();
}
