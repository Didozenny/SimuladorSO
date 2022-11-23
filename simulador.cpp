#include "hebraT.h"
#include "runqueues.h"
#include <cstdlib>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <unistd.h>



using namespace std;
int finish = 0;
runQueues rq;
int id = 0;
int pCount = 0;//Cantidad de elementos siendo procesados
mutex m;//mutex para manejar la runqueue
mutex p;//mutex para impresiones
condition_variable vacio;//variable de condicion para que las hebras G intenten crear nuevas hebras
condition_variable no_vacio;//variable de condicion para evitar procesar nuevas hebras cuando no quedan hebras a procesar

void work(int idHebra, int periodo, int b){
	while(!finish){
		unique_lock<mutex> ul(m);//obtenemos mutex
		no_vacio.wait(ul, []{return (rq.rqSize()!=0 && (rq.getActiveSize()!=0 || pCount == 0)) ? true : false; });//en caso de que este vacio espera
		hebraT T = rq.pop(p);
		pCount++;
		ul.unlock();

		int tiempoProcesar = min(periodo, T.tiempoRestante());
		T.procesar(periodo);
		p.lock();
		cout<<"Hebra id: "<<idHebra<<" Proceso id: "<<T.getId()<<" Tiempo de ejecucion: "<<tiempoProcesar<<" Tiempo restante: "<<T.tiempoRestante()<<endl;;
		p.unlock();

		bool inserted = false;
		ul.lock();
		pCount--;
		if(T.tiempoRestante() > 0){
			inserted = true;
			int priority = 9*T.tiempoRestante()/b;
			rq.insertE(T,priority);
		}
		ul.unlock();
		if(!inserted)vacio.notify_one();
		else no_vacio.notify_all();
	}
	vacio.notify_one();
}

void addT(int maxAdd,int a,int b){
	while(!finish){
		unique_lock<mutex> ul(m);
		vacio.wait(ul, []{return ((!rq.rqSize() && !pCount)||finish) ? true : false; });
		if(finish)break;
		for (int i = 0; i < 1 + rand()%maxAdd; ++i){
			hebraT T(id,a,b);
			int priority = 9*T.tiempoRestante()/b;
			rq.insertA(T,priority);
			id++;
		}
		ul.unlock();
		p.lock();
		cout<<"SE CREAN NUEVOS PROCESOS"<<endl;
		p.unlock();
		no_vacio.notify_all();
	}
}


int main(int argc, char *argv[]){
	int N = atoi(argv[1]);
	int M = atoi(argv[2]);
	int a = atoi(argv[3]);
	int b = atoi(argv[4]);
	int periodo = atoi(argv[5]);
	srand(time(NULL));
	//creamos hebrasT iniciales
	for (int i = 0; i < N; ++i)
	{
		hebraT T(id,a,b);
		int priority = 9*T.tiempoRestante()/b;
		rq.insertA(T,priority);
		id++;
	}
	//creamos arreglo de hebras M 
	thread hebraE[M];
	for (int i = 0; i < M; ++i)
		hebraE[i] = thread(work,i, periodo, b);
	thread hebraG(addT,max(N,M),a,b);
	while(cin>>finish and finish== 0);
	sleep(periodo+1);
	for (int i = 0; i < M; ++i)
	{
		hebraE[i].join();
	}
	hebraG.join();
	cout<<"EJECUCION TERMINADA"<<endl;
	return 0;
}
