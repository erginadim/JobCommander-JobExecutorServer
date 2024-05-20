#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "headers.h"

//Η συνάρτηση αυτή διαχειρίζεται τον jobCommander και τα pipes για την ανάγνωση μηνύματων από τον jobExecutorServer
void jobCommander(char **message, int serverPid){
    printf("JobCommander activated \n");
    int fdES,fdJC;
    char writemessbuff[MESSIZE+1], readmessbuff[MESSIZE+1];
    //Άνοιγμα name pipe για αποστολή μηνύματος
    fdES = open(fifoES, O_WRONLY);
    //Αν δεν ανοίξει επιτυχώς επιστρέφεται μήνυμα
    if(fdES < 0){
        perror("open");
        printf("Name pipe failed to open for writing \n");
        exit(1);
    }
    int i = 2;
	strcpy(writemessbuff, message[1]);
	while (message[i] != NULL) {
		sprintf(writemessbuff, "%s %s", writemessbuff, message[i]);
		i++;
	}
    int w = write(fdES,writemessbuff, MESSIZE+1);
    if(w == -1){
        perror("write");
        printf("Error occured in writing");
        exit(2);
    }
    close(fdES);
    if ((fdJC = open(fifoJC, O_RDONLY)) < 0) {
            perror("Server: can't open read fifo");
    }
    while(1){
        int r = read(fdJC,readmessbuff, MESSIZE+1);
        if(r == -1){
            perror("read");
            printf("Error occured in reading");
            exit(5);
        }
        if(strcmp(readmessbuff,"exit") == 0){
            break;
        }
        printf("%s \n",readmessbuff);
    }
    close(fdJC);		
}
