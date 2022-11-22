#include "hebraT.h"
#include <cstdlib>
#include <unistd.h>
#include <algorithm>


hebraT::hebraT(int id, int a,int b){
	this->id = id;
	tiempo = a+rand()%(b-a+1);
}

void hebraT::procesar(int x){
	sleep(min(x,tiempo));
	tiempo = max(tiempo-x,0);
}

int hebraT::tiempoRestante(){return tiempo;}

int hebraT::getId(){return id;}