//Το αρχείο αυτό περιέχει τις συναρτήσεις επικοινωνίας μέσω pipes μεταξύ jobCommander και jobExecutorServer

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "communication.h"
#include "headers.h"


//Η συνάρτηση αυτή ανοίγει ένα name pipe μόνο για ανάγνωση
void openNamePipeForReading(){
    int fdJC;
    fdJC = open("fifo", O_RDONLY | O_NONBLOCK);
    if(fdJC < 0){
        perror("open");
        printf("Name Pipe failed to open for reading \n");
        exit(1);
   }
    printf("Name pipe opened successfully for reading \n");
}


//H συνάρτηση αυτή ανοίγει ένα name pipe για εγγράφη και απόστολη απάντησης 
void openNamePipeForWritingAndRespond(char *args){
    int fdJC;
    char message[MESSIZE+1];
    //Άνοιγμα name pipe για αποστολή μηνύματος
    fdJC = open(fifoJC, O_WRONLY);
    //Αν δεν ανοίξει επιτυχώς επιστρέφεται μήνυμα
    if(fdJC < 0){
        perror("open");
        printf("Name pipe failed to open for writing \n");
        exit(1);
    }
    //Αν ανοίξει επιτυχώς γράφεται το μήνυμα
    printf("Name pipe opened successfully for writing \n");
    strcpy(message,args);
    int w = write(fdJC,message, MESSIZE+1);
    if(w == -1){
        perror("write");
        printf("Error occured in writing");
        exit(2);
    }
    //Τελός όταν γραφτεί το μήνυμα exit σημαίνει ότι έχει τελείωσει η ροή μηνύματων επομένως κλείνει και το name pipe 
    message[0] = '\0';
    strcpy(message , "exit");
    w = write(fdJC,message, MESSIZE+1);
    if(w == -1){
        perror("write");
        printf("Error occured in writing");
        exit(2);
    }

    close(fdJC);
}


//Η συνάρτηση αυτή είναι σχεδόν ίδια με την παραπάνω με την μόνη διαφόρα οτί αυτή συνάρτηση γάφει και και απανταεί ένα πίνακα χαρακτήρων με την τριπλέτα  <jobID,job,queuePosition>
void openNamePipeForWritingAndRespondTheTriplet(char **args){
    int fdJC,i;
    char message[MESSIZE+1];
    //Άνοιγμα name pipe για αποστολή μηνύματος
    fdJC = open(fifoJC, O_WRONLY);
    //Αν δεν ανοίξει επιτυχώς επιστρέφεται μήνυμα
    if(fdJC < 0){
        perror("open");
        printf("Name pipe failed to open for writing \n");
        exit(1);
    }
    i=0;
    //Αν ανοίξει επιτυχώς γράφεται το μήνυμα
    printf("Name pipe opened successfully for writing \n");
    while(args[i] != NULL){
        strcpy(message,args[i]);
        int w = write(fdJC,message, MESSIZE+1);
        if(w == -1){
            perror("write");
            printf("Error occured in writing");
            exit(2);
        }
        i++;
    }
    
    //Τελός όταν γραφτεί το μήνυμα exit σημαίνει ότι έχει τελείωσει η ροή μηνύματων επομένως κλείνει και το name pipe 
    message[0] = '\0';
    strcpy(message , "exit");
    int w = write(fdJC,message, MESSIZE+1);
    if(w == -1){
        perror("write");
        printf("Error occured in writing");
        exit(2);
    }

    close(fdJC);
}