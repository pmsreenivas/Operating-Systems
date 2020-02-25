#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "cs402.h"

#include "my402list.h"

#include "warmup1.h"

void Usage(){
	fprintf(stderr, "usage: warmup1 sort [tfile]\n");
	exit(-1);
}

void HandleFileError(){
	switch(errno){
  		case ENOENT:
  			fprintf(stderr, "%s does not exist\nexiting program\n", fileName);
  			break;
  		case EACCES:
  			fprintf(stderr, "%s access denied\nexiting program\n", fileName);
  			break;
  		case EISDIR:
  			fprintf(stderr, "%s is a directory\nexiting program\n", fileName);
  			break;
  	//	case EBFONT:
  		//	fprintf(stderr, "%s is in a wrong format\nexiting program\n", fileName);
  		//	break;
  		default:
  			fprintf(stderr, "%s has some sort of error\nexiting program\n", fileName);
  			break;   			
 	}
	exit(-1);	
}

void ProcessOptions(int argc, char *argv[]){
	if((argc < 2) || (argc > 3)) {Usage(); }
	if(strcmp(argv[1],"sort") != 0) {Usage(); }
  	if(argc == 2) {
	  fp = stdin;
	  strcpy(fileName,"input");
	  }
  	else{
  	  char str[100] = "file ";
  	  strcat(str,argv[2]);
  	  strcpy(fileName, str);	
	  fp = fopen(argv[2], "r");
	  if(fp == NULL) {
	  	HandleFileError();
	  }
	  }
 	 
}

void CheckClash(My402List* pList){
	My402ListElem *elem1 = NULL;
	My402ListElem *elem2 = NULL;
	Transaction* transaction1;
	Transaction* transaction2;
	for(elem1 = My402ListFirst(pList); elem1 != NULL; elem1 = My402ListNext(pList , elem1))	{
		for(elem2 = My402ListNext(pList , elem1); elem2 != NULL; elem2 = My402ListNext(pList , elem2)){
			transaction1 = (Transaction*)(elem1 -> obj );
			transaction2 = (Transaction*)(elem2 -> obj );
			if(transaction1 -> epoch == transaction2 -> epoch){
				fprintf(stderr, "%s contains multiple transactions with same timestamp\nexiting program\n", fileName);
				exit(-1);	
			} 
		}
	}	
}

void Sort(My402List* pList){
	My402ListElem *elem1 = NULL;
	My402ListElem *elem2 = NULL;
	Transaction* transaction1;
	Transaction* transaction2;
	void* t;
	for(elem1 = My402ListFirst(pList); elem1 != NULL; elem1 = My402ListNext(pList , elem1))	{
		for(elem2 = My402ListNext(pList , elem1); elem2 != NULL; elem2 = My402ListNext(pList , elem2)){
			transaction1 = (Transaction*)(elem1 -> obj );
			transaction2 = (Transaction*)(elem2 -> obj );
			if(transaction1 -> epoch > transaction2 -> epoch){
				t = (void*)(transaction1);
				elem1 -> obj = elem2 -> obj;
				elem2 -> obj = t;		
			} 
		}
	}		
}

void computeBalance(My402List* pList){
	int prevBal = 0;
	My402ListElem *elem = NULL;
	Transaction* transaction;
	for(elem = My402ListFirst(pList); elem != NULL; elem = My402ListNext(pList , elem)){
		transaction = (Transaction*)(elem -> obj);
		if(transaction -> tType == '+') {transaction -> postBalanceInCents = prevBal + transaction -> amountInCents;  }
		else {transaction -> postBalanceInCents = prevBal - transaction -> amountInCents; }
		prevBal = transaction -> postBalanceInCents;				
	}		
}

void print(My402List* pList){
 	if(My402ListEmpty(pList)){ 
 		fprintf(stderr, "%s has no transactions\nexiting program\n", fileName);
		exit(-1);  
 	}
	fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");
	fprintf(stdout, "|       Date      | Description              |         Amount |        Balance |\n");	
	fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");
	typedef struct tagPrintFields {
		char date[21];
		char desc[25];
		char amnt[21];
		char balc[18];
 	} PrintFields;
	PrintFields fieldVar;
	fieldVar.date[0] = '|';
	fieldVar.date[1] = ' ';
	fieldVar.date[17] = ' ';
	fieldVar.date[18] = '|';
	fieldVar.date[19] = ' ';
	fieldVar.date[20] = '\0';
	fieldVar.amnt[0] = ' ';
	fieldVar.amnt[1] = '|';
	fieldVar.amnt[2] = ' ';
	fieldVar.amnt[17] = ' ';
	fieldVar.amnt[18] = '|';
	fieldVar.amnt[19] = ' ';
	fieldVar.amnt[20] = '\0';
	fieldVar.balc[14] = ' ';
	fieldVar.balc[15] = '|';
	fieldVar.balc[16] = '\n';
	fieldVar.balc[17] = '\0';
	int cents, hundereds, thousands, millions, absoluteB = 0;						
	My402ListElem *elem = NULL;
	Transaction* transaction;
	time_t Cepoch;
	char timeStr[26];
	for(elem = My402ListFirst(pList); elem != NULL; elem = My402ListNext(pList , elem)){
		transaction = (Transaction*)(elem -> obj);
		Cepoch = transaction -> epoch;
		strncpy(timeStr, ctime(&Cepoch), sizeof(timeStr));
		fieldVar.date[2] = timeStr[0];
		fieldVar.date[3] = timeStr[1];
		fieldVar.date[4] = timeStr[2];
		fieldVar.date[5] = timeStr[3];
		fieldVar.date[6] = timeStr[4];
		fieldVar.date[7] = timeStr[5];
		fieldVar.date[8] = timeStr[6];
		fieldVar.date[9] = timeStr[7];
		fieldVar.date[10] = timeStr[8];
		fieldVar.date[11] = timeStr[9];
		fieldVar.date[12] = timeStr[10];
		fieldVar.date[13] = timeStr[20];
		fieldVar.date[14] = timeStr[21];
		fieldVar.date[15] = timeStr[22];
		fieldVar.date[16] = timeStr[23];
		strcpy(fieldVar.desc, transaction -> description);
		
		cents = (transaction -> amountInCents) % 100;
		hundereds = ((transaction -> amountInCents) % 100000)/100;
		thousands = ((transaction -> amountInCents) % 100000000)/100000;
		millions = (transaction -> amountInCents)/100000000;
		if(transaction -> tType == '-') {
			fieldVar.amnt[3] = '(';
			fieldVar.amnt[16] = ')';
		}
		else{
			fieldVar.amnt[3] = fieldVar.amnt[16] = ' ';
		} 
		if(millions == 0){
			fieldVar.amnt[4] = fieldVar.amnt[5] = ' ';
		}
		else{
			fieldVar.amnt[4] = (char)millions + '0';
			fieldVar.amnt[5] = ',';
		}
		if(transaction -> amountInCents <= 99999){
			fieldVar.amnt[6] = fieldVar.amnt[7] = fieldVar.amnt[8] = fieldVar.amnt[9] = ' ';
		}
		else{
			fieldVar.amnt[9] = ',';
			if(thousands > 99){
				fieldVar.amnt[6] = (char)(thousands/100) + '0';
				fieldVar.amnt[7] = (char)((thousands % 100)/10) + '0';
				fieldVar.amnt[8] = (char)(thousands % 10) + '0';
			}
			else if(thousands > 9){
				fieldVar.amnt[6] = (millions == 0)?(' '):('0');
				fieldVar.amnt[7] = (char)(thousands/10) + '0';
				fieldVar.amnt[8] = (char)(thousands%10) + '0';
			}
			else{
				fieldVar.amnt[6] = (millions == 0)?(' '):('0');
				fieldVar.amnt[7] = (millions == 0)?(' '):('0');
				fieldVar.amnt[8] = (char)(thousands) + '0';
			}
		}
		fieldVar.amnt[12] = (char)(hundereds % 10) + '0';
		if(transaction -> amountInCents <= 999){
			fieldVar.amnt[10] = fieldVar.amnt[11] = ' ';
		}
		else{
			if(hundereds > 99){
				fieldVar.amnt[10] = (char)(hundereds/100) + '0';
				fieldVar.amnt[11] = (char)((hundereds % 100)/10) + '0';				
			}
			else if(hundereds > 9){
				fieldVar.amnt[10] = ((millions > 0) || (thousands > 0)) ?('0') : (' ');
				fieldVar.amnt[11] = (char)((hundereds % 100)/10) + '0';				
			}
			else{
				fieldVar.amnt[10] = ((millions > 0) || (thousands > 0)) ?('0') : (' ');
				fieldVar.amnt[11] = ((millions > 0) || (thousands > 0)) ?('0') : (' ');				
			}
		}
		fieldVar.amnt[13] = '.';
		fieldVar.amnt[14] = (char)(cents/10) + '0';
		fieldVar.amnt[15] = (char)(cents%10) + '0';	
		if(transaction -> postBalanceInCents >= 0){
			fieldVar.balc[0] = fieldVar.balc[13] = ' ';
		}
		else{
			fieldVar.balc[0] = '(';
			fieldVar.balc[13] = ')';
		}
		absoluteB = (transaction -> postBalanceInCents > 0)?(transaction -> postBalanceInCents):(-1 * (transaction -> postBalanceInCents));
		cents = (absoluteB) % 100;
		hundereds = ((absoluteB) % 100000)/100;
		thousands = ((absoluteB) % 100000000)/100000;
		millions = (absoluteB)/100000000;
		if(millions > 9){
			fieldVar.balc[1] = fieldVar.balc[3] = fieldVar.balc[4] = fieldVar.balc[5] = fieldVar.balc[7] = fieldVar.balc[8] = fieldVar.balc[9] = fieldVar.balc[11] = fieldVar.balc[12] = '?';
			fieldVar.balc[2] = fieldVar.balc[6] = ','; 
			fieldVar.balc[10] = '.'; 
		}	
		else{
			if(millions == 0){
				fieldVar.balc[1] = fieldVar.balc[2] = ' ';
			}
			else{
				fieldVar.balc[1] = (char)millions + '0';
				fieldVar.balc[2] = ',';
			}
			if(absoluteB <= 99999){
				fieldVar.balc[3] = fieldVar.balc[4] = fieldVar.balc[5] = fieldVar.balc[6] = ' ';
			}
			else{
				fieldVar.balc[6] = ',';
				if(thousands > 99){
					fieldVar.balc[3] = (char)(thousands/100) + '0';
					fieldVar.balc[4] = (char)((thousands % 100)/10) + '0';
					fieldVar.balc[5] = (char)(thousands % 10) + '0';
				}
				else if(thousands > 9){
					fieldVar.balc[3] = (millions == 0)?(' '):('0');
					fieldVar.balc[4] = (char)(thousands/10) + '0';
					fieldVar.balc[5] = (char)(thousands%10) + '0';
				}
				else{
					fieldVar.balc[3] = (millions == 0)?(' '):('0');
					fieldVar.balc[4] = (millions == 0)?(' '):('0');
					fieldVar.balc[5] = (char)(thousands) + '0';
				}
			}
			fieldVar.balc[9] = (char)(hundereds % 10) + '0';
			if(absoluteB <= 999){
				fieldVar.balc[7] = fieldVar.balc[8] = ' ';
			}
			else{
				if(hundereds > 99){
					fieldVar.balc[7] = (char)(hundereds/100) + '0';
					fieldVar.balc[8] = (char)((hundereds % 100)/10) + '0';				
				}
				else if(hundereds > 9){
					fieldVar.balc[7] = ((millions > 0) || (thousands > 0)) ?('0') : (' ');
					fieldVar.balc[8] = (char)((hundereds % 100)/10) + '0';				
				}
				else{
					fieldVar.balc[7] = ((millions > 0) || (thousands > 0)) ?('0') : (' ');
					fieldVar.balc[8] = ((millions > 0) || (thousands > 0)) ?('0') : (' ');				
				}
			}
			fieldVar.balc[10] = '.';
			fieldVar.balc[11] = (char)(cents/10) + '0';
			fieldVar.balc[12] = (char)(cents%10) + '0';	
		}
		fprintf(stdout,"%s%s%s%s",fieldVar.date,fieldVar.desc,fieldVar.amnt,fieldVar.balc);																												
	}
	fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");		
}

void AddToList(My402List* pList, char CtType, int Cepoch, int CamountInCents, char* Cdescription){
	Transaction* transaction = (Transaction*)malloc(sizeof(Transaction));
	if(transaction == NULL){
 		fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
		exit(-1);		 	
	}	
	int success;
	int i;
	transaction -> tType = CtType;
	transaction -> epoch = Cepoch;
	transaction -> amountInCents = CamountInCents;
	strncpy (transaction -> description, Cdescription, sizeof(transaction -> description));
	transaction -> description[24] = '\0';
	if(strlen(transaction -> description) < 24){
		for(i = strlen(transaction -> description); i < 24; i++ ){
			transaction -> description[i] = ' ';
		}
	}
	transaction -> postBalanceInCents = 0;
	success = My402ListAppend(pList, (void*)transaction);
	if(!success){
 		fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
		exit(-1);		 	
	}	
}

void ProcessLine(My402List* pList, char line[1026], int lineNo){
	int i;
	int numTabs = 0;
	char CtType;
	int Cepoch;
	int CamountInCents;
	char* Cdescription = "\0";
	while(1){
		if(isspace(line[strlen(line) - 1])){
			line[strlen(line) - 1] = '\0';
		}
		else break;
	}//right trim
	for(i = 0; i < strlen(line); i++ ){
		if(line[i] == '\t') numTabs++; 				
	}
	if(numTabs != 3){
		fprintf(stderr, "line %d of %s does not have 4 fields\nexiting program\n",lineNo,fileName);
		exit(-1);		
	}
	char *start_ptr = line;
 	char *tab_ptr = NULL;
 	char *fields[4];
 	i = 0;
 	do{
  		tab_ptr = strchr(start_ptr, '\t');
  		if(tab_ptr != NULL){
   			*tab_ptr++ = '\0';
   			fields[i] = start_ptr;
   			i++;
   			start_ptr = tab_ptr;
  		}
 	}while(tab_ptr != NULL);
 	fields[i] = start_ptr;
	if(!((strlen(fields[0]) == 1) && ((fields[0][0] == '+') || (fields[0][0] == '-')))){
		fprintf(stderr, "line %d of %s has invalid transaction type\nexiting program\n",lineNo,fileName);
		exit(-1);	
	}
	CtType = fields[0][0];
	if(strlen(fields[1]) > 10){
		L: fprintf(stderr, "line %d of %s has invalid transaction time stamp\nexiting program\n",lineNo,fileName);
		exit(-1);
	} 
	for(i = 0; i < strlen(fields[1]); i++){
		if(!(isdigit(fields[1][i]))) goto L;
	}
	int currentEpoch;
	currentEpoch = time(NULL);
	Cepoch = atoi(fields[1]);
	if (Cepoch > currentEpoch) goto L;

	if(strlen(fields[2]) > 10){
		L2: fprintf(stderr, "line %d of %s has invalid transaction amount\nexiting program\n",lineNo,fileName);
		exit(-1);
	}
	for(i = 0; i < strlen(fields[2]); i++){
		if((isdigit(fields[2][i])) || fields[2][i] == '.') continue;
		else goto L2;
	} 		
	char *preDecimal;
	char *postDecimal;
	char amount[12];
	strcpy(amount, fields[2]);
	preDecimal = amount;
	postDecimal = strchr(preDecimal, '.');
	if(postDecimal == NULL) goto L2;
	*postDecimal++ = '\0';
	if((strlen(preDecimal) > 7) || (strlen(postDecimal) != 2) || (strlen(preDecimal) == 0)) goto L2;
	
	for(i = 0; i < strlen(preDecimal); i++){
		if((isdigit(preDecimal[i]))) continue;
		else goto L2;
	} 
	for(i = 0; i < strlen(postDecimal); i++){
		if((isdigit(postDecimal[i]))) continue;
		else goto L2;
	} 	
	
	CamountInCents = 100 * atoi(preDecimal) + atoi(postDecimal);	
	Cdescription = fields[3];	
	while(isspace(Cdescription[0])){
		for(i = 0; i < strlen(Cdescription); i++){
			Cdescription[i] = Cdescription[i+1];
		}	
	}//left trim
	if(strlen(Cdescription) == 0){
		fprintf(stderr, "line %d of %s has invalid description\nexiting program\n",lineNo,fileName);
		exit(-1);	
	}
	//at this point, line is not malformed and all Cs are right, time clash error alone still possible
	
	AddToList(pList, CtType, Cepoch, CamountInCents, Cdescription);				
}

void ProcessFile(My402List* pList){
 	if(feof(fp)){ 
 		fprintf(stderr, "%s has no transactions\nexiting program\n", fileName);
		exit(-1);  
 	}
	char line[1026];
	int lineNo = 0;
	while(TRUE){
		if(fgets(line, sizeof(line), fp) == NULL){ break;}
		lineNo++;		
		if(strlen(line) > 1024 ) {
			fprintf(stderr, "line %d of %s has more than 1024 characters\nexiting program\n",lineNo,fileName);
			exit(-1);	
		}
		ProcessLine(pList, line, lineNo); 
	}
}

int main(int argc, char *argv[]){
 	ProcessOptions(argc, argv);
 	My402List* pList;
 	pList = (My402List*)malloc(sizeof(My402List));
 	if(pList == NULL){
 		fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
		exit(-1);	
	 }
 	My402ListInit(pList);
 	ProcessFile(pList);
 	if(fp != stdin) {fclose(fp);}
 	//At this point we are left with checkClash, sort, computeBalance and print
 	CheckClash(pList);
 	Sort(pList);
 	computeBalance(pList);
 	print(pList);
 	return(0);
}


