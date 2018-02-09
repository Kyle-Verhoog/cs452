#include <user/nameserver.h>
#include <defines.h>


void NameServer() {
  int ret;
  RegisterNS();

  nameserver_store store;
  nss_init(&store);

  tid_t requestor;
  NSReq request;

  while(true) {
    Receive(&requestor, &request, sizeof(NSReq));

    switch(request.type) {
      case NS_WHOIS:
        ret = nss_get(&store, request.name);
        // assert(ret >= 0);
        Reply(requestor, &ret, sizeof(ret));
        break;
      case NS_REGISTERAS:
        ret = nss_set(&store, request.name, requestor);
        // assert(ret == 0);
        Reply(requestor, &ret, sizeof(ret));
        break;
      case NS_STOP:
        Reply(requestor, &requestor, sizeof(tid_t));
        Exit();
      default:
        assert(0);
        break;
    }
  }
}

int RegisterAs(int n) {
  tid_t ns_tid = GetNS();
  if (ns_tid < 0)
    assert(0);

  NSReq rec;
  rec.type = NS_REGISTERAS;
  rec.name = n;

  int reply;
  Send(ns_tid, &rec, sizeof(NSReq), &reply, sizeof(reply));
  return reply;
}

tid_t WhoIs(int n) {
  tid_t ns_tid = GetNS();
  assert(ns_tid > 0);

  NSReq rec;
  rec.type = NS_WHOIS;
  rec.name = n;

  int reply;
  Send(ns_tid, &rec, sizeof(NSReq), &reply, sizeof(reply));
  return reply;
}

void NameServerStop() {
  tid_t ns_tid = GetNS();

  NSReq rec;
  rec.type = NS_STOP;

  int reply;
  Send(ns_tid, &rec, sizeof(NSReq), &reply, sizeof(int));
  Exit();
}
