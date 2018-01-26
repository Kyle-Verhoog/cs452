#include <nameserver/nameserver.h>
#include <defines.h>
#include <syscalls.h>


void NameServerTask(){
	NSrecord NameServer[NAMESERVER_SIZE];

	while(true){
		int requestor;
		NSreq request;

		Receive(&requestor, &request, sizeof(NSreq));

		switch(request.type){
			case WhoIs:
				Reply(requestor, &NameServer[request.name].tid, sizeof(int));
				break;
			case RegisterAs:
				NameServer[request.name].tid = request.tid;
				Reply(requestor, &NameServer[request.name].tid, sizeof(int));
				break;
			default:
				assert(0);
				break;
		}
	}
}
