//Η συνάρτηση main διαχειρίζεται το άνοιγμα και κλείσιμο του server
//Στο αρχείο jobExecutorServer.txt αποθηκεύεται το pid του server
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "headers.h"


char *fifoES = "fifoES";
char *fifoJC = "fifoJC"; 
char *serverFile = "jobExecutorServer.txt";
int MESSIZE = 200; 


int main(int argc,char* argv[]){
    FILE *jobExecutorServer;
    pid_t server_pid;
    int pid;
    jobExecutorServer = fopen(serverFile,"r");
    //Αν το αρχείο δεν υπάρχει τότε ανοίγει για να δέχτει το pid του server 
    if (jobExecutorServer == NULL) {
		server_pid = fork();
        //Αν αποτύχει επίστρεφει κατάλληλο μήνυμα λάθους 
        if (server_pid == -1) {
			perror("Fork failed");
			exit(1);
		}
        //Αν δεν έχει γράψει το server pid ανοίγει το αρχείο, γράφει το pid και μέτα κλείνει το αρχείο
        if (server_pid == 0) {
            jobExecutorServer = fopen(serverFile, "w");
            fprintf(jobExecutorServer, "%d\n", getpid());
            fclose(jobExecutorServer);
            jobExecutorserver();
        }
        //Αν το έχει γράψει κράταει το pid και ανόιγει pipe για επικοινωνία 
        else {
            pid = server_pid;
            if (mkfifo (fifoES , 0777) == -1){
                if (errno != EEXIST ) {
                    perror ("mkfifo failed") ;
                    exit (6) ;
                }
            }
        }        
    }
    //Αν το αρχείο υπάρχει ήδη απλά πέρνει το ήδη γραμμένο pid
    else {
		fscanf(jobExecutorServer, "%d", &pid);
		fclose(jobExecutorServer);
	}
    if (argc == 1) {
		printf("missing arguments...\n");
		exit(1);
	}
    //Τέλος καλεί τον jobCommander
    jobCommander(argv,pid);
}