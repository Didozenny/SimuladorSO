#include "hebraT.h"
#include "runqueues.h"
#include <cstdlib>
#include <mutex>
#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <ctime>   

using namespace std;
runQueues rq;
int finish = 0;//variable para terminar programa
int id = 0;//id de las hebras
int pCount = 0;//Cantidad de elementos siendo procesados
mutex m;//mutex para manejar la runqueue
mutex p;//mutex para impresiones
condition_variable vacio;//variable de condicion para que las hebras G intenten crear nuevas hebras
//Variable encargada de que que la hebra espere cuando no quedan procesos o cuando no quedan elementos en 
//la queue activa y a su vez hay elementos procesandose(para evitar hacer swap entre las queues antes de terminar)
condition_variable no_vacio;

void work(int idHebra, int periodo, int b, bool fullPrint){
	while(!finish){
		int prevPriority;
		unique_lock<mutex> ul(m);//obtenemos mutex
		no_vacio.wait(ul, []{return ((rq.rqSize()!=0 && (rq.getActiveSize()!=0 || pCount == 0))||finish) ? true : false; });//en caso descrito arriba espera
		//en caso de que se deba terminar el programa este notifica a otra hebra esperando y hace break
		if(finish){
			//llama a una hebra siguiente para que termine ejecucion
			no_vacio.notify_one();
			break;
		}
		//se obtiene la prioridad del elemento y el elemento
		prevPriority = rq.getFirstPriority();
		hebraT T = rq.pop(p);
		pCount++;//entra un elemento en proceso
		ul.unlock();
		//se calcula tiempo a procesar y se procesa T
		int tiempoProcesar = min(periodo, T.tiempoRestante());
		T.procesar(periodo);
		p.lock();
		if(fullPrint)cout<<"Hebra id: "<<idHebra<<" Proceso id: "<<T.getId()<<" Tiempo de ejecucion: "<<tiempoProcesar<<" Tiempo restante: "<<T.tiempoRestante()<<endl;
		if(!T.tiempoRestante())cout<<"Proceso "<<T.getId()<<" termino con turn around time = "<<T.turnAroundTime()<<" con un tiempo original = "<<T.getTiempoOriginal()<<endl;
		p.unlock();


		bool finished = false;//booleano para saber si no quedan elementos hebraE y llamar a la hebraG
		bool swap = false;//booleano para saber cuando se hará swap y por ende despertar a las hebras esperando por swap
		
		ul.lock();
		pCount--;//sale un elemento en proceso
		if(pCount == 0 and rq.getActiveSize() == 0)swap = true;//se debe notificar para que se haga el swap
		if(T.tiempoRestante() > 0){//se ingresa elemento a queue expirada en caso de ser adecuado
			int priority = 1 + (9 - prevPriority)/2 + (9*(T.tiempoRestante())/b)/2;//calculo prioridad
			if(priority > 9)priority = 9;
			rq.insertE(T,priority);
		}
		else if(pCount == 0 and rq.rqSize() == 0)//ya no quedan hebrasE
			finished = true;
		ul.unlock();
		if(finished)vacio.notify_one();
		else if(swap) no_vacio.notify_all();
	}
	vacio.notify_one();
}

void nNewProcess(int N,int a,int b){//se crean procesos y a medida se crean se notifica a las hebras
	for (int i = 0; i < N; ++i)
	{
		hebraT T(id,a,b);
		rq.insertA(T,0);
		id++;
		no_vacio.notify_one();
	}
}

void addT(int maxAdd,int a,int b, int maxProcesses){
	while(!finish){
		//obtiene el lock y espera con la condicion de que no queden elementos en las runqueue y no haya elementos procesando o que haya un termino forzado
		unique_lock<mutex> ul(m);
		vacio.wait(ul, []{return ((!rq.rqSize() && !pCount)||finish) ? true : false; });
		if(finish)break;//se realizo un termino forzado
		int nprocesses = min(1+ rand()%maxAdd, maxProcesses - id);//en caso de que el programa termine ejecucion nproceses sera 0
		if(nprocesses <= 0){
			//se cambia el valor de finish se desbloque el mutex y se llema a las hebras para que terminen ejecucion
			finish = 1;
			ul.unlock();
			no_vacio.notify_one();
			p.lock();
			cout<<"Procesos terminados, ingresar un 1 para terminar ejecucion"<<endl;
			p.unlock();
			break;
		}
		nNewProcess(nprocesses, a, b);
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
	int maxProcesses = atoi(argv[5]);
	int periodo = atoi(argv[6]);
	bool fullPrint = false;
	if(argc == 8 and string(argv[7]) == "fullPrint")
		fullPrint = true;		

	srand(time(NULL));
	//creamos arreglo de hebras M 
	thread hebraE[M];
	for (int i = 0; i < M; ++i)
		hebraE[i] = thread(work,i, periodo, b, fullPrint);
	//creamos hebrasT iniciales
	nNewProcess(N, a, b);
	thread hebraG(addT,maxProcesses,a,b,N + maxProcesses);
	int add;
	while(!finish){
		cin >> add;
		finish += 1;
	}
	if(finish < 2){
		cout<<"Interrupcion: espere "<< periodo << " segundos para que finalice el programa"<<endl;
		sleep(periodo+1);
	}
	for (int i = 0; i < M; ++i)
		hebraE[i].join();
	hebraG.join();
	cout<<"EJECUCION TERMINADA"<<endl;
	return 0;
}