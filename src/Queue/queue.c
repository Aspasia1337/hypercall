#include "queue.h"

MESSAGE_QUEUE gQueue;
KSPIN_LOCK gQueueLock;
BOOLEAN gQueueInitialized;

void InitQueue(void) {
	KIRQL IRQ;
	if (!gQueueInitialized) {
		KeInitializeSpinLock(&gQueueLock);
		KeAcquireSpinLock(&gQueueLock, &IRQ);
		gQueueInitialized = 1;
		gQueue.Count = 0;
		gQueue.ReadIndex = 0;
		gQueue.WriteIndex = 0;
		KeReleaseSpinLock(&gQueueLock, IRQ);
	}
}

BOOLEAN EnqueueMessage(const char* message) {
	KIRQL IRQ;
	
	KeAcquireSpinLock(&gQueueLock, &IRQ);

	if (!gQueueInitialized)
		return 0;

	if (gQueue.Count >= MAXMSG) {
		gQueue.ReadIndex = (gQueue.ReadIndex + 1) % MAXMSG;
	}
	else {
		gQueue.Count += 1;
	}

	RtlStringCchCopyA(gQueue.Messages[gQueue.WriteIndex], MAXLEN, message);
	gQueue.WriteIndex = (gQueue.WriteIndex + 1) % MAXMSG;

	KeReleaseSpinLock(&gQueueLock, IRQ);
	return 1;
}

BOOLEAN DequeueMessage(char* outbuffer, ULONG bufferSize) {
	KIRQL IRQ;

	KeAcquireSpinLock(&gQueueLock, &IRQ);

	if (!gQueueInitialized)
		return 0;

	if (gQueue.Count == 0) {
		return 0;
	}

	RtlStringCchCopyA(outbuffer, bufferSize, gQueue.Messages[gQueue.ReadIndex]);

	gQueue.ReadIndex = (gQueue.ReadIndex + 1) % MAXMSG;
	gQueue.Count -= 1;

	KeReleaseSpinLock(&gQueueLock, IRQ);
	return 1;
}