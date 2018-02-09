#include <user/nameserver.h>
#include <defines.h>

// TODO error checking
void NameServer() {
  RegisterNS();
  NSrecord NameServer[NAMESERVER_SIZE];

  while(true) {
    tid_t requestor;
    NSreq request;

    Receive(&requestor, &request, sizeof(NSreq));

    switch(request.type) {
    case WhoIs_t:
      Reply(requestor, &NameServer[request.name].tid, sizeof(tid_t));
      break;
    case RegisterAs_t:
      NameServer[request.name].tid = requestor; // request.tid;
      Reply(requestor, &NameServer[request.name].tid, sizeof(tid_t));
      break;
    case Stop_t:
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
  if (ns_tid < 0) {
    assert(0);
    // TODO: error checking
  }
  NSreq rec;
  rec.type = RegisterAs_t;
  rec.name = n;

  int reply;
  Send(ns_tid, &rec, sizeof(NSreq), &reply, sizeof(NSrecord));

  return reply;
}

tid_t WhoIs(int n) {
  tid_t ns_tid = GetNS();
  
  // TODO: error checking
  assert(ns_tid > 0);

  NSreq rec;
  rec.type = WhoIs_t;
  rec.name = n;

  int reply;
  Send(ns_tid, &rec, sizeof(NSreq), &reply, sizeof(NSrecord));

  return reply;
}

void NameServerStop() {
  tid_t ns_tid = GetNS();

  NSreq rec;
  rec.type = Stop_t;

  int reply;
  Send(ns_tid, &rec, sizeof(NSreq), &reply, sizeof(int));
  Exit();
}
