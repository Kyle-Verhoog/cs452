#include <user/nameserver.h>
#include <defines.h>

CIRCULAR_BUFFER_DEF(ns_q, tid_t, NS_MAX_TASK_QUEUE_SIZE);


void NameServer() {
  int i, r, ret, size;
  tid_t tid;
  ns_q name_q[NS_MAX_ENTRY];
  RegisterNS();

  for (i = 0; i < NS_MAX_ENTRY; ++i) {
    ns_q_init(&name_q[i]);
  }

  nameserver_store store;
  nss_init(&store);

  tid_t requestor;
  NSReq request;

  while(true) {
    Receive(&requestor, &request, sizeof(NSReq));

    switch(request.type) {
      case NS_WHOIS:
        ret = nss_get(&store, request.name);
        if (ret == -1) {
          r = ns_q_push(&name_q[request.name], requestor);
          assert(r == 0);
        } else {
          assert(ret >= 0);
          Reply(requestor, &ret, sizeof(ret));
        }
        break;
      case NS_REGISTERAS:
        ret = nss_set(&store, request.name, requestor);
        size = name_q[request.name].size;
        for (i = 0; i < size; ++i) {
          r = ns_q_pop(&name_q[request.name], &tid);
          assert(r == 0);
          Reply(tid, &requestor, sizeof(requestor));
        }
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
