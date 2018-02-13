#include <user/ioserver.h>

void IOServerNotifier(void *args) {
  int r;
  IONotifierArgs *myargs;
  myargs = (IONotifierArgs *)args;
  int req;
  int rep;

  while (true) {
    AwaitEvent(myargs->inter);
    //r = Send(myargs->tid, );
    assert(r == 0);
  }
}

void IOServerRX(void *args) {
  int r;
  tid_t mytid, req_tid;
  IOServerArgs *myargs;
  IONotifierArgs notargs;
  myargs = (IOServerArgs *)args;

  mytid = MyTid();
  assert(IS_VALID_ID(mytid));

  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  assert(r == 0);

  while (true) {
    //r = Receive(&req_tid, );
    assert(r == 0);

  }
}

void IOServerTX(void *args) {
  int r;
  tid_t mytid;
  IOServerArgs *myargs;
  IONotifierArgs notargs;
  myargs = (IOServerArgs *)args;

  mytid = MyTid();
  assert(IS_VALID_ID(mytid));
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
}
