#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <pthread.h>
#include <math.h>


int N;			//numar perechi
double *v;		//vector pentru citire date 
double complex *x;	//vectorul cu datele initiale
double complex *rez;	//vectorul cu rezultate
int P;		//numarul de threaduri


//calculez in functie de numarul de threaduri
void* threadFunction(void *args)
{
	int thread_id = *(int*)args;
	int inc=thread_id;
	int i,j;	
	for(i = inc*N/P; i < N*(inc+1)/P; i++)
	       	for (j=0 ; j<N ; j++) 
			rez[i] += (x[j] * cos(j * i * M_PI*2 / N))-(x[j] * sin(j * i * M_PI*2 / N))*I;	
	return NULL;	
}

//aloc memorie pentru vectori
void init()
{
	v = malloc(sizeof(double ) * N);
	x = malloc(sizeof(double complex ) * N);
	rez = malloc(sizeof(double complex ) * N);
}

int main(int argc, char * argv[]) {

	FILE *read;
	read = fopen(argv[1], "r");
	FILE *write;
	int h=0;
	write = fopen(argv[2], "w");

	if(fscanf(read,"%d",&N)){	//verific daca pot sa citesc din fisier
		h++;
	}
	if(h==0){
		printf("EROARE");
		return 0;
	}

	fprintf(write,"%d\n",N);
	
	int i;
	init();
	
	for(i=0;i<N;i++){			//citesc datele in vecotul v si le salvez in x
		if(fscanf(read,"%lf",&v[i]))
			x[i]=v[i];
	}
	
	P=atoi(argv[3]);
	pthread_t tid[P];
	int thread_id[P];

	for(i = 0;i < P; i++)
		thread_id[i] = i;

	for(i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}


	for(i=0;i<N;i++)	//afisez rezultatul
		fprintf(write,"%.6f %.6f\n",creal(rez[i]),cimag(rez[i]));  

	fclose (read);
	fclose (write);

	return 0;
}
