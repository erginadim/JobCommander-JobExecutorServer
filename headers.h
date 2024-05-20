#include "list.h"
#include "communication.h"

extern int MESSIZE;
extern int concurrency;
extern char *serverFile;
extern char *fifoES;
extern char *fifoJC;


void jobExecutorserver();
void issueJob(char *,ActiveList *,QueuedList *,int,unsigned short int);
void handler(int );
void splitCommand(char *, char **);
void update();


void jobCommander(char **, int );


