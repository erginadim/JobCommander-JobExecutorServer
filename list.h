typedef struct activelistnode *ActiveList;

struct activelistnode{
	char job[100];
	int jobID;
	int pid;
	ActiveList next;
};

typedef struct queuedlistnode *QueuedList;

struct queuedlistnode{
	char job[100];	
	int jobID;
	QueuedList next ;
};

extern ActiveList activelist;
extern QueuedList queuedlist;

//Οι διεργασίες που βρίσκονται στην ουρά για εκτέλεση δεν έχουν ακόμα pid
//θα δωθεί όταν γίνει η εξαγώγη από αυτη την λίστα και γίνει η εισαγωγή στην λίστα με τις διεργασίες προς εκτέλεση

int activeSize(ActiveList *);
void activePush(ActiveList *,int , char *,int );
int activePop(ActiveList *,int);
void activePrint(ActiveList , char **);
int activeStop(ActiveList *, int );


int queuedSize(QueuedList *);
void queuedPush(QueuedList *,int ,char *);
int queuedPop(QueuedList *);
void queuedPrint(QueuedList ,char **);
int queuedStop(QueuedList *, int );



