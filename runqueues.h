#ifndef RUNQUEUES
#define RUNQUEUES
#include "hebraT.h"
#include <mutex>
#include <iostream>

class runQueues{
private:
	queue<hebraT> *active, *expired;
	int sizeActive, sizeExpired;
	void swapQ();
public:
	runQueues();
	~runQueues();
	void insertE(hebraT T, int priority);
	void insertA(hebraT T, int priority);
	hebraT pop(mutex &p);
	int getFirstPriority();
	int rqSize();
	int getActiveSize();
};
#endif