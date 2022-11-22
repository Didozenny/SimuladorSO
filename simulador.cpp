#include "hebraT.h"
#include "runqueues.h"
#include <cstdlib>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <thread>


using namespace std;

runQueues rq;
int id = 0;
int pCount = 0;
mutex m;//mutex para manejar la runqueue
mutex p;//mutex para impresiones
condition_variable vacio;//variable de condicion para que las hebras G intenten crear nuevas hebras
condition_variable no_vacio;//variable de condicion para evitar procesar nuevas hebras cuando no quedan hebras a procesar
condition_variable active_vacio;//variable de condicion para evitar que se comience a procesar elementos en la lista de expirados antes de terminar de procesar la lista de procesos activos por completo

void work(int idHebra, int periodo, int b){
	while(true){
		unique_lock<mutex> ul(m);
		no_vacio.wait(ul, []{return (rq.rqSize()!=0) ? true : false; });
		active_vacio.wait(ul, []{return (rq.getActiveSize()!=0 || !pCount) ? true : false; });
		p.lock();
		hebraT T = rq.pop();
		p.unlock();
		pCount++;
		ul.unlock();

		int tiempoProcesar = min(periodo, T.tiempoRestante());
		T.procesar(periodo);
		p.lock();
		cout<<"Hebra id: "<<idHebra<<" Proceso id: "<<T.getId()<<" Tiempo de ejecucion: "<<tiempoProcesar<<" Tiempo restante: "<<T.tiempoRestante()<<endl;;
		p.unlock();

		ul.lock();
		pCount--;
		if(T.tiempoRestante() > 0){
			int priority = 9 - 9*T.tiempoRestante()/b;
			rq.insertE(T,priority);
		}
		ul.unlock();
		vacio.notify_one();
		active_vacio.notify_all();
	}
}

void addT(int maxAdd,int a,int b){
	while(true){
		unique_lock<mutex> ul(m);
		vacio.wait(ul, []{return (!rq.rqSize() && !pCount) ? true : false; });
		for (int i = 0; i < 1 + rand()%maxAdd; ++i){
			hebraT T(id,a,b);
			int priority = 9 - 9*T.tiempoRestante()/b;
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
		int priority = 9 - 9*T.tiempoRestante()/b;
		rq.insertA(T,priority);
		id++;
	}

	thread hebraE[M];
	for (int i = 0; i < M; ++i)
		hebraE[i] = thread(work,i, periodo, b);
	thread hebraG(addT,max(N,M),a,b);
	string finish;
	getline(cin,finish);
	return 0;
}