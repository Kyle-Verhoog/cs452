#include <user/nameserver.h>
#include <defines.h>

// TODO error checking
void NameServerTask() {
  RegisterNS();
  NSrecord NameServer[NAMESERVER_SIZE];

  while(true) {
    int requestor;
    NSreq request;

    Receive(&requestor, &request, sizeof(NSreq));

    switch(request.type) {
      case WhoIs_t:
        Reply(requestor, &NameServer[request.name].tid, sizeof(int));
        break;
      case RegisterAs_t:
        NameServer[request.name].tid = requestor; // request.tid;
        Reply(requestor, &NameServer[request.name].tid, sizeof(int));
        break;
      default:
        assert(0);
        break;
    }
  }
}

int RegisterAs(int n) {
  int ns_tid = GetNS();
  if (ns_tid < 0) {
    // TODO: error checking
  }
  NSreq rec;
  rec.type = RegisterAs_t;
  rec.name = n;
  
  int reply;
  Send(ns_tid, &rec, sizeof(NSreq), &reply, sizeof(NSrecord));

  return reply;
}

int WhoIs(int n) {
  int ns_tid = GetNS();
  if (ns_tid < 0) {
    // TODO: error checking
  }
  NSreq rec;
  rec.type = WhoIs_t;
  rec.name = n;

  int reply;
  Send(ns_tid, &rec, sizeof(NSreq), &reply, sizeof(NSrecord));

  return reply;
}
