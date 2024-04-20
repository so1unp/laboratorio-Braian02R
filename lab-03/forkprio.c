#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>

int num_hijos;
int segundos;
pid_t hijos[25];
struct rusage usage;

 void trataSenhal(int sig)
{	
	switch (sig)
	{
	case SIGINT:
		/* code */
		break;
	case SIGTERM:
		getrusage(RUSAGE_SELF, &usage);
		printf("\nChild %d (nice %2d):\t%3li\n\n", getpid(), getpriority(PRIO_PROCESS, 0),  usage.ru_stime.tv_sec);
		exit(-1);
		break;

	default:
		break;
	}
}
 void trataPadre(int sig)
{	
	int i;
	switch (sig)
	{
	case SIGINT:
		
		for ( i=0; i < num_hijos; i++)
		{

			kill(hijos[i], SIGTERM);

		}
		exit(0);
		
		break;
	case SIGTERM:
		exit(-1);
		break;

	default:
		break;
	}
}

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[])
{   
	
	num_hijos = atoi(argv[1]);
	segundos = atoi(argv[2]);
	int reducir_prioridades = atoi(argv[3]); 

	int i=0;
	//int j=0;
	for (i; i < num_hijos; i++){
		/* Se crea el proceso hijo y se comprueba el error */
		hijos[i] = fork();

		if (hijos[i] == -1){
		perror ("No se puede lanzar proceso");
		exit (-1);
		}
		/* Camino que sigue el proceso hijo.*/
		else if (hijos[i]==0) {
			if(reducir_prioridades == 1)
				setpriority(PRIO_PROCESS, getpid(), 10-i);
			else if (reducir_prioridades == 0)
				setpriority(PRIO_PROCESS, getpid(), 10);
			else 
				printf("\nerror debe colocar un valor valido 1 o 0");

			signal(SIGTERM, trataSenhal);
			signal(SIGINT, trataSenhal);

            busywork();
		}
	}


	/* Camino que sigue el proceso padre.*/
	signal(SIGINT, trataPadre);
	sleep (segundos);
	
    exit(EXIT_SUCCESS);
}

