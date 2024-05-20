/*Σε αυτό το αρχείο υπάρχουν οι συναρτήσεις void jobExecutorServer(), void issueJob(char *command,ActiveList *activelist,QueuedList *queuedlist,int jobID,unsigned short int inst),void queuedToActive()
void splitCommand(char *buf, char **args),void update() και void exitOp() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>



#include "communication.h"
#include "headers.h"


#define PERMS 0777




ActiveList activelist = NULL;
QueuedList queuedlist = NULL;
int concurrency = 1; /*από εκφώνηση*/
/*Η void jobExecutorServer() είναι η υλοποίηση του job Executor Server, αρχίκα ανοίγει ένα name pipe για ανάγνωση και στην συνέχεια ανάλογα με τις εντολές που δέχεται (μια απο τις "issueJob","setConcurrency","stop ","poll" και "exit")
υλοποιεί την αντίστοιχη λειτουργία κρατώντας πάντα επικοινωνία με τον job commander μέσω name pipes*/
void jobExecutorserver(){
    int id,jobID=0;
    int fdES;
    char respondString[200] , *command, *input,*args[64], *buffer[MESSIZE+1];
    printf("JobExecutorServer activated\n");
    if ( (mkfifo(fifoJC, PERMS) < 0) &&
        (errno != EEXIST) ) {
        perror("Can't create fifo");
    }
    for(;;){
        for (int i=0 ; i<64 ; i++){
            args[i] = NULL;
        }
        if ((fdES = open(fifoES, O_RDONLY)) < 0) {
            perror("Server: can't open read fifo");
            exit(3);
        }
        if(read(fdES,buffer,MESSIZE+1) < 0){
            perror("Reading problem");
            exit(5);
        }
        close(fdES);	
        //Διαβάζει την εντολή που δόθηκε
        command = strtok(buffer," ");


        //Αν η εντολή είναι η issueJob τοτε, αν η εντολή έχει γράφει σώστα εκτέλει το issueJob όπως θα φανεί παρακάτω και αυξάνει το JobID κατά 1, αλλίως εκτύπωνει κατάλληλο μήνυμαμ λάθους
        if(strcmp(command, "issueJob") == 0){
            input = strtok(NULL," ");
            if(input == NULL){
                sprintf(respondString,"%s","Not correct,argument is missing");
                openNamePipeForWritingAndRespond(respondString);
            }
            else{
                issueJob(input,&activelist,&queuedlist,jobID,1); 
                jobID ++;
                printf("job ID: %d\n",jobID);
            }
        }


        //Αν η εντολή είναι η setConcurrency τότε, δέχεται το νέο concurrency που ορίζεται από τον χρήστη, στέλνει κατάλληλο μήνυμα αλλαγής στον jobCommander και κάλει την συνάρτηση update
        //η οποία θα φάνει πιο παρακάτω
        if(strcmp(command,"setConcurrency") == 0){
        input = strtok(NULL,"");
        concurrency = atoi(input);
        sprintf(respondString,"%s","Concurrency changed to %d \n",concurrency);
        openNamePipeForWritingAndRespond(respondString);
        update();
        }


        //Αν η εντολή είναι η stop τότε, δέχεται το συγκέκριμενο id διεργασίας που θέλουμε να σταματήσει, αναζητεί την διεργασία αρχίκα στην λίστα με τις διεργασίες που είναι active 
        //αν δεν βρεθεί εκεί τότε αναζητεί και την λίστα με τις διεργασίες που είναι στην ούρα προς εκτέλεση και τέλος αν βρεί την διεργασία σε μια από τις δύο λίστες σταματαεί την εκτέλεση της
        //στέλνοντας καταλλήλο μήνυμα στον jobCommander, αλλίως στέλνει μήνυμα οτι δεν βρέθηκε η συγκεκρίμενη διεργασία
        if(strcmp(command,"stop ") == 0){
            input = strtok(NULL,"");
            id = atoi(input);
            //Αναζητούμε την διεργασία με το συγκεκριμένο id τόσο στην λίστα με τις διεργασίες που τρέχουν όσο και σε αυτή με τις διεργασίες που βρίσκονται σε αναμονή
            if(!activeStop(activelist,id)){
                if(!queuedStop(queuedlist,id)){
                    sprintf(respondString,"%s","No job found to stop ,neither in the running jobs nor the queued ones \n");
                    openNamePipeForWritingAndRespond(respondString);
                }
                else{
                    //Η διεργασία βρέθηκε σε ουρά αναμόνης
                    int stopped = atoi(input);
                    sprintf(respondString,"%s %d","Job with jobID stopped",stopped);
                    openNamePipeForWritingAndRespond(respondString);
                }
            }
            else{
                //Η διεργασία βρέθηκε στην λίστα με αυτές που τρέχουν
                int stopped = atoi(input);
                sprintf(respondString,"%s %d","Job with jobID stopped",stopped);
                openNamePipeForWritingAndRespond(respondString);
            }

        }


        //Αν η εντολή είναι η poll,τότε δέχεται και το δεύτερο όρισμα (running ,queued)
        //αν είναι poll running, τότε για κάθε διεργασία που είναι στην λίστα με τις active δίεργασίες στέλνει την τριπλέτα  <jobID,job,queuePosition> στον jobCommander
        //αν είναι poll queued, τότε για κάθε διεργασία που είναι στην λίστα με τις queued διεργασίες στέλνει την τριπλέτα <jobID,job,queuePosition> στον jobCommander
        if(strcmp(command,"poll") == 0){
            input = strtok(NULL,"");
            if(strcmp(input,"running") == 0){
                activePrint(activelist,args);
                openNamePipeForWritingAndRespondTheTriplet(args);
            }
            if(strcmp(input,"queued") == 0){
                queuedPrint(queuedlist,args);
                openNamePipeForWritingAndRespondTheTriplet(args);
            }
            int i = 0;
            while (args[i] != NULL) {
            free(args[i]);
                args[i] = NULL;
                i++;
            }
        }


        //Αν η εντολή είναι exit, τότε κάνει κλείνει την σύνδεση με τον server και στέλνει κατάλληλο μήνυμα στον jobCommander για έξοδο
        if(strcmp(command,"exit") == 0){
            char respondString[200];
            unlink(serverFile);
            sprintf(respondString,"%s","Exiting");
            openNamePipeForWritingAndRespond(respondString);
            exit(0); 
        }

        fflush(stdout);

        //Αν η εντόλη σεν είναι καμία από τις παραπάνω τότε στέλνει κατάλληλο μηνύμα λάθους
        if(strcmp(command, "issueJob") != 0 && strcmp(command,"setConcurrency") != 0 && strcmp(command,"stop ") != 0 && strcmp(command,"poll") != 0 && strcmp(command,"exit") != 0){
            sprintf(respondString,"%s","Unknown command \n");
            openNamePipeForWritingAndRespond(respondString);
        }
        fflush(stdout);
    };    
};



/*Η void issueJob(char *command,ActiveList *activelist,QueuedList *queuedlist,int jobID,unsigned short int inst) υλοποιεί την εισαγωγή εργασίων στο σύστημα που πρόκειται
να εκτελεστούν, διακρίνοντας τις 3 περιπώσεις. */

void issueJob(char *command,ActiveList *activelist,QueuedList *queuedlist,int jobID,unsigned short int inst){ 
    pid_t pid;
    char *args[64],respondString[200],job[strlen(command)];
    strcpy(job,command);
    //Χρησιμοποιεί την συνάρτηση splitCommand γία να χώρισει την εντολή σε μέρη ώστε να χρησιμοποιηθεί στην issueJob
    splitCommand(command,job);
    //Ορίζουμε έναν διαχειρίστη σήματος,ο οποίος στέλνει ένα σήμα SIGCHLD στην δίεργασία γόνεα όταν η διεργασία παιδί έχει αλλάξει κατάσταση (έδω γίνεται active και απόκτα pid) 
    //και καλείται η συνάρτηση handler για την διαχείρηση του σήματος
    signal(SIGCHLD,handler);
    if(activeSize(activelist) < concurrency){
        //Διακρίνουμε τις εξής περιπτώσεις με βάση την τίμη της μεταβλήτης inst ( χρησιμοποιείται σαν flag )
        //Αρχίκα αν δεν υπάρχει χώρος στην λίστα με τις active διεργασίες, τότε η διεργασία εισάγεται στην λίστα με τις queued
        //Αν υπάρχει χώρος τότε αν το inst είναι 1 σημαίνει οτι μια active διεργασία τελείωσε και θα ερθεί μια νέα από τις queued
        //Αν είναι 1 τότε σημαίνει οτι η διεργασία θα μπεί για εκτέλεση κατευθείαν
        if(inst == 1){
            sprintf(respondString,"%s","Job added to running \n");
            openNamePipeForWritingAndRespond(respondString);
        }
        else{
            printf("%d %s %s\n", jobID, job, "running ...");
            if ((pid = fork()) < 0) { 
				perror("fork");
				exit(1);
			}
			if (pid == 0) { 
				execvp(*args, args); 
				perror(*args);
				exit(1);
            }
            else{
                printf("Added to the list with the active jobs\n");
                activePush(activelist,jobID,job,pid);
            }        
        }
    }
    else{
        sprintf(respondString,"%s","There is no space left to run the process,added to the queue \n");
        openNamePipeForWritingAndRespond(respondString);
        queuedPush(queuedlist,jobID,job);
    }
    
};



//Η συνάρτηση του handler διαχειρίζεται το σήμα SIGCHLD 
void handler(int sig){
    printf("Handler activated \n");
    QueuedList newlist;
    char job[100];
    int jobID, status;
    pid_t pid;
    //Περίμενει για όλες τις διεργασίες παιδία που έχουν τελείωσει για να τις αφαίρεσει από την λίστα των active διεργασίων
    while ((pid = waitpid((pid_t)(-1), &status, WNOHANG)) > 0){
        activePop(&activelist,pid);
    }
    //Αν μέτα την αφαίρεση υπάρχει διαθέσιμος χώρος στην λίστα με τις active διεργασίες κάλει την συνάρτηση issueJob για εκτέλεση των νέων διεργασίων
    if(activeSize((activelist)) < concurrency){
        for(int i = 0; i <(concurrency-activeSize(activelist));i++){
            if(queuedSize(queuedlist) > 0){
                newlist = queuedlist;
                jobID = (newlist)->jobID;
                printf("jobID: %d \n",jobID);
                queuedPop(queuedlist);
                issueJob(job,&activelist,&queuedlist,jobID,1);
            }
        }
    }
};



/*Η void splitCommand(char *buf, char **args) σπαεί την εντολή σε κομμάτια ώστε να μπορεί να υλοποιηθεί η λειτουργία της, χρεισιμοποείται στην issueJob*/
void splitCommand(char *buf, char **args) {
	while (*buf != '\0') {
		while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n'))
			*buf++ = '\0'; 
		*args++ = buf;
		while ((*buf != '\0') && (*buf != ' ') && (*buf != '\t') && (*buf != '\n'))
			buf++; 
		*args = NULL;
	}
	
};



/*Η void update() καλείται καθέ φορά που αλλάζει το concurrency με σκόπο να προσαρμόσει την λίστα active με βάση το νέο concurrency*/
void update(){
    QueuedList newqueuedlist;
    char job[100];
    int jobID;
    if (activeSize(activelist) < concurrency) {
        for(int i = 0; abs(i < concurrency - activeSize(activelist)); i++){
           int count = queuedSize(queuedlist);
           if(count > 0){
             newqueuedlist = queuedlist;
			 jobID = queuedlist->jobID;
             strcpy(job,queuedlist->job);
             queuedPop(queuedlist);
             issueJob(job,&activelist,&queuedlist,jobID,0); 
           }
        }
    }
}

