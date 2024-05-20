#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "list.h"

//Το αρχείο αυτό αναλαμβάνει την διαχείριση των δύο λίστων Active List και Queued List στις οποίες αποθηκεύονται 
//οι διεργασίες που εκτελούνται τώρα και οι διεργασίες που είναι σε αναμονή προς εκτέλεση αντίστοιχα


//Η συνάρτηση αυτή επιστρέφει το μέγεθος της active list
int activeSize(ActiveList *list) {
	int count = 0;
	ActiveList templist;
	templist = *list;
	if (templist == NULL)
		return 0;
	while (templist != NULL) {
		count++;
		templist = templist->next;
	}
	return count;
}


//Η συνάρτηση αυτή επιστρέφει το μέγεθος της queued list
int queuedSize(QueuedList *list) {
	int count = 0;
	QueuedList templist;
	templist = *list;
	if (templist == NULL)
		return 0;
	while (templist != NULL) {
		count++;
		templist = templist->next;
	}
	return count;
}


//H συνάρτηση αυτή εισάγει μια διεργασία στην λίστα των active
void activePush(ActiveList *list, int jobID, char *job, int pid) {
	ActiveList templist;
	templist = *list;
	//Αν είναι η πρώτη διεργασία 
	if (templist == NULL) {
		*list = malloc(sizeof(struct activelistnode));
		(*list)->jobID = jobID;
		(*list)->pid = pid;
		strcpy((*list)->job, job);
		(*list)->next = templist; 
	}
	else {
		while (*list != NULL)
			list = &((*list)->next);
		*list = malloc(sizeof(struct activelistnode));
		(*list)->jobID = jobID;
		(*list)->pid = pid;
		strcpy((*list)->job, job);
		(*list)->next = NULL; 
	}
}


//Η συνάρτηση αυτή εισάγει μια διεργασία στην λίστα με τις queued διεργασίες
void queuedPush(QueuedList *list, int jobID, char *job) {
	QueuedList templist;
	templist = *list;
	//Αν είναι η πρώτη διεργασία στην λίστα
	if (templist == NULL) {
		*list = malloc(sizeof(struct queuedlistnode));
		(*list)->jobID = jobID;
		strcpy((*list)->job, job);
		(*list)->next = templist; 
	}
	else {
		while (*list != NULL)
		list = &((*list)->next);
		*list = malloc(sizeof(struct queuedlistnode));
		(*list)->jobID = jobID;
		strcpy((*list)->job, job);
		(*list)->next = NULL; 
	}
}


//Η συνάρτηση αυτή εκτυπώνει όλες τις διεργασίες στην active list
void activePrint(ActiveList list, char **args) {
	int i = 0;
	char buffer[500];
	
	while (list != NULL) {
		sprintf(buffer, "%d\t%s", list->jobID, list->job);
		args[i] = malloc(sizeof(char)*(strlen(buffer)+1));
		strcpy(args[i], buffer);
		i++;
		list = list->next;
	}
	args[i] = NULL;
	
}

//H συνάρτηση αυτή εκτυπώνει όλες τις διεργασίες στην queued list
void queuedPrint(QueuedList list, char **args) {
	int i;
	char buffer[500];
	i = 0;
	while (list != NULL) {
		sprintf(buffer, "%d\t%s", list->jobID, list->job);
		args[i] = malloc(sizeof(char)*(strlen(buffer)+1));
		strcpy(args[i], buffer);
		i++;
		list = list->next;
	}
	args[i] = NULL;
}


//H συνάρτηση αυτή σταματαεί την διεργασία με το συγκεκριμένο jobID από την λίστα με τις active διεργασίες
int activeStop(ActiveList *list, int jobID) {
	ActiveList templist;
	while ((*list) != NULL) {
		if ((*list)->jobID == jobID) {
			templist = *list;
			kill((*list)->pid, SIGKILL);
			*list = (*list)->next;
			free(templist);
			return 1;
		}
		else
			list = &((*list)->next);
	}
	return 0;
}


//Η συνάρτηση αυτή σταματαεί την διεργασία με το συγκεκριμένο jobID από την λίστα με τις queued διεργασίες
int queuedStop(QueuedList *list, int jobID) {
	QueuedList templist;
	while ((*list) != NULL) {
		if ((*list)->jobID == jobID) {
			templist = *list;
			*list = (*list)->next;
			free(templist);
			return 1;
		}
		else
			list = &((*list)->next);
	}
	return 0;
}



//Η συνάρτηση αυτή αφαιρεί την διεργασία με το συγκεκριμένο pid από την λίστα με τις active διεργασίες 
int activePop(ActiveList *list, int pid) {
	ActiveList templist;
	while (*list != NULL) {
		if ((*list)->pid == pid) {
			templist = *list;
			*list = (*list)->next;
			free(templist);
			return 1;
		}
		else
			list = &((*list)->next);
	}
	return 0;
}


//Η συνάρτηση αυτή αφαιρεί τον πρώτο κόμβο της λίστας (δηλαδή την πρώτη διεργασία) που βρίσκεται στην λίστα με τις queued διεργασίες
int queuedPop(QueuedList *list) {
	QueuedList templist;
	if ((list) != NULL) {
		templist = list;
		*list = (*list)->next;
		free(templist);
		return 1;
	}
	return 0;
}