#ifndef HEBRA_T
#define HEBRA_T
#include <queue>
using namespace std;


class hebraT{
private:
	int tiempo;
	int id;
public:
	hebraT(int id, int a, int b);
	void procesar(int);
	int tiempoRestante();
	int getId();
};
#endif