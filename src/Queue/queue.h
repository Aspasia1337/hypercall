#pragma once

#include <ntddk.h>
#include <ntstrsafe.h>

#define MAXMSG 100
#define MAXLEN 256

typedef struct _MESSAGE_QUEUE {
	char Messages[MAXMSG][MAXLEN];
	ULONG WriteIndex; 
	ULONG ReadIndex; 
	ULONG Count;
} MESSAGE_QUEUE;

extern MESSAGE_QUEUE gQueue;
extern KSPIN_LOCK gQueueLock;
extern BOOLEAN gQueueInitialized;

void InitQueue(void);
BOOLEAN EnqueueMessage(const char*);
BOOLEAN DequeueMessage(char* outbuffer, ULONG bufferSize);