#include <bits/stdc++.h>

using namespace std;

int main(){
	queue<int> *array = new queue<int>[100];
	queue<int> *array2 = new queue<int>[100];
	array[2].push(100); 
	array[1].push(300); 
	cout<<array[1].front()<<endl;
	queue<int> *aux = array;
	array = array2;
	array2 = aux;
	cout<<array[1].front()<<endl;
	cout<<array2[1].front()<<endl;
}