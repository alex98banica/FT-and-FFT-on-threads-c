#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <pthread.h>
 
typedef double complex cplx;
int N;	//numar perechi
int P;	//numar threaduri
double *v;	//vector citire
double complex *x;	//vector cu datele initiale
double complex *rez;	//vector ajutator
double PI=atan2(1, 1) * 4;

//functie alocare
void init()
{
	v = malloc(sizeof(double ) * N);
	x = malloc(sizeof(double complex ) * N);
	rez = malloc(sizeof(double complex ) * N);

}

//functia fft 
void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}

void* threadFunction(void *args)
{

	int thread_id = *(int*)args;
	if(P==1)
		_fft(x, rez, N, 1);

	if(P==2)	//caz pentru 2 threaduri
	{
		if(thread_id==0)
			_fft(rez, x, N, 2);
		
		if(thread_id==1){
			_fft(rez+1, x+1, N, 2);	
		}	
	}

	if(P==4)	//caz pentru 4 threaduri
	{
		if(thread_id==0)
			_fft(x, rez, N, 4);
		
		if(thread_id==1)
			_fft(x+2, rez+2, N, 4);
	
		if(thread_id==2)
			_fft(x+1, rez+1, N, 4);

		if(thread_id==3)
			_fft(x+3, rez+3, N, 4);
	}

	return NULL;		
}

int main(int argc, char * argv[])
{

	int h=0;
	FILE *read;
	read = fopen(argv[1], "r");
	FILE *write;
	write = fopen(argv[2], "w");

	if(fscanf(read,"%d",&N))	//verificare citire
		h++;
	else{
		printf("EROARE");
	}
	fprintf(write,"%d\n",N);
	
	init();
	int i;	
	
	for(i=0;i<N;i++){
		if(fscanf(read,"%lf",&v[i]))
			x[i]=v[i];
	}

	P=atoi(argv[3]);
	pthread_t tid[P];
	int thread_id[P];


	for (int i = 0; i < N; i++)
		rez[i]=x[i];



	for(i = 0;i < P; i++)
		thread_id[i] = i;

	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	if(P==2){		//caz pentru 2 threaduri
		for (int i = 0; i < N; i += 2) {
			cplx t = cexp(-I * PI * i / N) * rez[i + 1];
			x[i / 2]     = rez[i] + t;
			x[(i + N)/2] = rez[i] - t;}}
	
	if(P==4){		//caz pentru 4 threaduri
		for (int i = 0; i < N; i += 4) {
			cplx t = cexp(-I * PI * i / N) * x[i + 2];
			rez[i / 2]     = x[i] + t;
			rez[(i + N)/2] = x[i] - t;
		}
		for (int i = 0; i < N; i += 4) {
			cplx t = cexp(-I * PI * i / N) * (x+1)[i + 2];
			(rez+1)[i / 2]     = (x+1)[i] + t;
			(rez+1)[(i + N)/2] = (x+1)[i] - t;
		}

		for (int i = 0; i < N; i += 2) {
			cplx t = cexp(-I * PI * i / N) * rez[i + 1];
			x[i / 2]     = rez[i] + t;
			x[(i + N)/2] = rez[i] - t;
		}
	}

	for (int i = 0; i < N; i++)		//scriere
		fprintf(write,"%.6f %.6f\n", creal(x[i]), cimag(x[i]));
 
	return 0;
}
 
 
