#ifndef NAMESERVER_H
#define NAMESERVER_H

#define NAMESERVER_SIZE 1024

typedef enum NSservice{
	WhoIs = 0,
	RegisterAs = 1
}NSservice;

typedef struct NSrecord{
	//int name;
	int tid;
}NSrecord;

typedef struct NSreq{
	NSservice type;
	int name;
	int tid;
}NSreq;

void NameServerTask();

#endif /*NAMESERVER_H*/
