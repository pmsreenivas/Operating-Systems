#include "cs402.h"
#include "my402list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

FILE* fp;
char fileName[100] = "\0";

typedef struct tagTransaction {
	char tType;
	int epoch;
	int amountInCents;
	char description[25];
	int postBalanceInCents;
} Transaction;

void Usage();
void ProcessOptions(int argc, char *argv[]);
void ProcessLine(My402List* pList, char line[1026], int lineNo);
void HandleFileError();
void ProcessFile(My402List* pList);
void AddToList(My402List* pList, char CtType, int Cepoch, int CamountInCents, char* Cdescription);
void CheckClash(My402List* pList);
void Sort(My402List* pList);
void computeBalance(My402List* pList);
void print(My402List* pList);


