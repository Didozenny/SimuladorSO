#include "hebraT.h"
#include "runqueues.h"
runQueues::runQueues(){
	sizeActive = 0;
	sizeExpired = 0;
	active = new queue<hebraT>[10];
	expired = new queue<hebraT>[10];
}

runQueues::~runQueues(){
	delete[] active;
	delete[] expired;
}

void runQueues::insertE(hebraT T, int priority){
	expired[priority].push(T);
	sizeExpired++;
}

void runQueues::insertA(hebraT T, int priority){
	active[priority].push(T);
	sizeActive++;
}

hebraT runQueues::pop(){
	if(sizeActive == 0)swapQ();
	int lowestPrior;
	for (int i = 0; i < 10; ++i){
		if(!active[i].size())continue;
		lowestPrior = i;
		break;
	}
	hebraT aux = active[lowestPrior].front();
	active[lowestPrior].pop();
	sizeActive--;
	return aux; 
}

void runQueues::swapQ(){
	queue<hebraT> *aux = active;
	active = expired;
	expired = aux;
	sizeActive = sizeExpired;
	sizeExpired = 0;
}

int runQueues::rqSize(){return (sizeActive+sizeExpired);}