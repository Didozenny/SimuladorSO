#include "hebraT.h"
#include <cstdlib>
#include <unistd.h>
#include <algorithm>



hebraT::hebraT(int id, int a,int b){
	this->id = id;
	tiempo = tiempoOriginal = a+rand()%(b-a+1);

	start = chrono::system_clock::now();
}

void hebraT::procesar(int x){
	sleep(min(x,tiempo));
	tiempo = max(tiempo-x,0);
}

int hebraT::tiempoRestante(){return tiempo;}

int hebraT::getId(){return id;}

int hebraT::getTiempoOriginal(){return tiempoOriginal;}

double hebraT::turnAroundTime(){
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end-start;
	return elapsed_seconds.count();
}