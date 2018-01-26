#include <nameserver/nameserver.h>
#include <defines.h>
#include <syscalls.h>

//Just for compile sake
void receive(int* a, void* msg, int len){}
void reply(int a, void* msg, int len){}

void NameServerTask(){
	NSrecord NameServer[NAMESERVER_SIZE];

	while(true){
		int requestor;
		NSreq request;

		receive(&requestor, &request, sizeof(NSreq));

		switch(request.type){
			case WhoIs:
				reply(requestor, &NameServer[request.name].tid, sizeof(int));
				break;
			case RegisterAs:
				NameServer[request.name].tid = request.tid;
				reply(requestor, &NameServer[request.name].tid, sizeof(int));
				break;
			default:
				assert(0);
				break;
		}
	}
}
