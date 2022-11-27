#ifndef HEBRA_T
#define HEBRA_T
#include <queue>
#include <chrono>
#include <ctime>  
using namespace std;


class hebraT{
private:
	int tiempoOriginal;
	int tiempo;
	int id;
	chrono::time_point<std::chrono::system_clock> start;
public:
	hebraT(int id, int a, int b);
	void procesar(int);
	int tiempoRestante();
	int getId();
	int getTiempoOriginal();
	double turnAroundTime();
};
#endif