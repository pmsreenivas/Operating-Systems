#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include<ctype.h>
#include<math.h>


#include "cs402.h"
#include "my402list.h"
#include "warmup2.h"

void SetDefaults(){
	lambda = 1.0;
	mu = 0.35;
	r = 1.5;
	B = 10;
	P = 3;
	num = 20;
	mode = 0;
}

void Usage(){
	fprintf(stderr, "usage: warmup2 [-lambda lambda=positive_real] [-mu mu=positive_real] [-r r=positive_real] [-B B=positive_int] [-P P=positive_int] [-n num=positive_int] [-t tsfile]\n");
	exit(-1);
}

void AdjustValuesEPP(){
	packetInterArrivalTime_ms = (1/lambda > 10) ? (10000) : (round(1000*(1/lambda)));
	packetServiceTime_ms = (1/mu > 10) ? (10000) : (round(1000*(1/mu)));
	tokenInterArrivalTime_ms = (1/r > 10) ? (10000) : (round(1000*(1/r)));
	char line[1026];
	if(mode){
		fgets(line, sizeof(line), tsfile);
		if(strlen(line) > 1024) {
			fprintf(stderr, "line 1 of %s has more than 1024 characters\nexiting program\n", fileName);
			exit(-1);
		}
		line[strlen(line) - 1] = '\0';
		int i;
		for(i = 0; i < strlen(line); i++){
			if(!isdigit(line[i])){
				fprintf(stderr, "line 1 of %s is not in the right format\nexiting program\n", fileName);
				exit(1);
			}
		}
		num = atoi(line);
		if(num < 1) {
			fprintf(stderr, "line 1 of %s is not in the right format\nexiting program\n", fileName);
			exit(1);
		}
	}
	printf("Emulation Parameters:\n" );
	printf("\tnumber to arrive = %d\n",num);
	if(!mode){
		printf("\tlambda = %lf\n", lambda);
		printf("\tmu = %lf\n", mu);
	}
	printf("\tr = %lf\n", r);
	printf("\tB = %d\n", B);
	if(!mode) {printf("\tP = %d\n\n", P);}
	if(mode) {printf("\ttsfile = %s\n\n", fileName);}
	numLeftT = num;
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
  		case EBFONT:
  			fprintf(stderr, "%s is in a wrong format\nexiting program\n", fileName);
  			break;
  		default:
  			fprintf(stderr, "%s has some sort of error\nexiting program\n", fileName);
  			break;   			
 	}
	exit(-1);	
}

void ProcessOptions(int argc, char *argv[]){
	int i , j, flag, lambdaF = 0, muF = 0, rF = 0, BF = 0, PF = 0, nF = 0, tF = 0;
	if((argc > 15) || (argc % 2 == 0)) {Usage();}
	for (i = 1, flag = 0; i < argc; i = i + 2, flag = 0) {
		if(argv[i][0] == '-'){
			if(strcmp(argv[i],"-lambda") == 0){
				if(lambdaF == 0) lambdaF = 1;
				else Usage();
				for(j = 0; j < strlen(argv[i + 1]) ; j++){
					if(argv[i + 1][j] == '.'){
						if(flag == 0){
						flag = 1;
						} else {
							Usage();
						}
					}
					else if(isdigit(argv[i + 1][j])){
						continue;
					}
					else {Usage();}
				}
				if ((sscanf(argv[i + 1], "%lf", &lambda) != 1) || argv[i + 1][0] == '-') {
					Usage();
    			} else {
    				continue;
    			}
			}
			else if(strcmp(argv[i],"-mu") == 0){
				if(muF == 0) muF = 1;
				else Usage();
				for(j = 0; j < strlen(argv[i + 1]) ; j++){
					if(argv[i + 1][j] == '.'){
						if(flag == 0){
						flag = 1;
						} else {
							Usage();
						}
					}
					else if(isdigit(argv[i + 1][j])){
						continue;
					}
					else {Usage();}
				}
				if ((sscanf(argv[i + 1], "%lf", &mu) != 1) || argv[i + 1][0] == '-') {
					Usage();
    			} else {
    				continue;
    			}
			}
			else if(strcmp(argv[i],"-r") == 0){
				if(rF == 0) rF = 1;
				else Usage();
				for(j = 0; j < strlen(argv[i + 1]) ; j++){
					if(argv[i + 1][j] == '.'){
						if(flag == 0){
						flag = 1;
						} else {
							Usage();
						}
					}
					else if(isdigit(argv[i + 1][j])){
						continue;
					}
					else {Usage();}
				}
				if ((sscanf(argv[i + 1], "%lf", &r) != 1) || argv[i + 1][0] == '-') {
					Usage();
    			} else {
    				continue;
    			}
			}
			else if(strcmp(argv[i],"-B") == 0){
				if(BF == 0) BF = 1;
				else Usage();
				for(j = 0; j < strlen(argv[i + 1]) ; j++){
					 if(isdigit(argv[i + 1][j])){
						continue;
					}
					else {Usage();}
				}
				if ((sscanf(argv[i + 1], "%d", &B) != 1) || argv[i + 1][0] == '-') {
					Usage();
    			} else {
    				continue;
    			}
			}
			else if(strcmp(argv[i],"-P") == 0){
				if(PF == 0) PF = 1;
				else Usage();
				for(j = 0; j < strlen(argv[i + 1]) ; j++){
					 if(isdigit(argv[i + 1][j])){
						continue;
					}
					else {Usage();}
				}
				if ((sscanf(argv[i + 1], "%d", &P) != 1) || argv[i + 1][0] == '-') {
					Usage();
    			} else {
    				continue;
    			}
			}
			else if(strcmp(argv[i],"-n") == 0){
				if(nF == 0) nF = 1;
				else Usage();
				for(j = 0; j < strlen(argv[i + 1]) ; j++){
					 if(isdigit(argv[i + 1][j])){
						continue;
					}
					else {Usage();}
				}
				if ((sscanf(argv[i + 1], "%d", &num) != 1) || argv[i + 1][0] == '-') {
					Usage();
    			} else {
    				continue;
    			}
			}
			else if(strcmp(argv[i],"-t") == 0){
				if(tF == 0) tF = 1;
				else Usage();
				strcpy(fileName, argv[i + 1]);
				tsfile = fopen(argv[i + 1], "r");
				if(tsfile == NULL) {HandleFileError();}
				if(feof(tsfile)){ 
 					fprintf(stderr, "%s has nothing\nexiting program\n", fileName);
					exit(-1);  
 				}
				mode = 1;
			}
			else {Usage();}												
		} else {Usage();}	
    }
    if(lambda == 0.0 || mu == 0.0 || r == 0.0 || P == 0 || B == 0 || num == 0) {Usage();}
}

void parse_line(char *line, char **addr_of_first_str, char **addr_of_second_str, char **addr_of_third_str){
	*addr_of_first_str = line;
    while (line[0] != ' ' && line[0] != '\t') line++;
    while (line[0] == ' ' || line[0] == '\t') line++;
    *addr_of_second_str = line;
    while (line[0] != ' ' && line[0] != '\t') line++;
    while (line[0] == ' ' || line[0] == '\t') line++;
    *addr_of_third_str = line; 	
}

void timersubb(struct timeval *big, struct timeval *small, struct timeval *res){
	if(big -> tv_usec > small -> tv_usec){
		res -> tv_usec = big -> tv_usec - small -> tv_usec;
		res -> tv_sec = big -> tv_sec - small -> tv_sec;
	}
	else{
		res -> tv_usec = 1000000 + big -> tv_usec - small -> tv_usec;
		res -> tv_sec= big -> tv_sec - small -> tv_sec - 1;
	}
}

void printCurTimeStamp(){
	struct timeval now, dif;
	gettimeofday(&now,0);
	timersubb(&now ,&beginTime, &dif);
	double curtime_ms = dif.tv_sec * 1000.0 + dif.tv_usec/1000.0 ;
	printf("%012.3lfms: ",curtime_ms);  
}

double printTimeDiff(struct timeval* t2, struct timeval* t1){
	struct timeval dif;
	timersubb(t2, t1, &dif);
	double time_ms = dif.tv_sec * 1000.0 + dif.tv_usec/1000.0 ;
	printf("%.3lfms",time_ms );
	return time_ms;
}

void *arrivalFun(void* arg){
	int numLeft = num;
	int lineNo = 2;
	char line[1026];
	line[0] = '\0';
	Packet* packet;
	struct timeval prevArrive = beginTime;
	struct timeval bk1, bk2, dbk;
	while(numLeft > 0 && !shutdown){
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		packet = (Packet*)malloc(sizeof(Packet));
		line[0] = '\0';
		if(packet == NULL) {
			fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
			exit(-1);	
		}
		packet -> numb = lineNo - 1;
		if(!mode){
			packet -> InterArrivalTime_us = packetInterArrivalTime_ms * 1000;
			packet -> RequiredTokens = P;
			packet -> RequiredService_us = packetServiceTime_ms * 1000;
		} else{
			fgets(line, sizeof(line), tsfile);
			if(line[0] == '\0'){
				fprintf(stderr, "file %s doesn't have the right number of lines\nexiting program\n",fileName);
				exit(-1);
			}
			if(strlen(line) > 1024) {
				fprintf(stderr, "line %d of %s has more than 1024 characters\nexiting program\n",lineNo, fileName);
				exit(-1);
			}
			if((line[strlen(line) - 1] != '\n' && lineNo != num + 1) || ((!isdigit(line[strlen(line) - 1]) && line[strlen(line) - 1] != '\n') && lineNo == num + 1) || !isdigit(line[0])){
				fprintf(stderr, "line %d of %s is not in the right format\nexiting program\n",lineNo, fileName);
				exit(-1);				
			}
			int i;
			for(i = 0; i < strlen(line) - 1; i++){
				if(!(isdigit(line[i]) || line[i] == ' ' || line[i] == '\t')){
					fprintf(stderr, "line %d of %s is not in the right format\nexiting program\n",lineNo, fileName);
					exit(-1);
				}
			}
			int b1, n2, b2, n3, b3;
			b1 = n2 = b2 = n3 = b3 =0;
			for(i = 0; i < strlen(line) - 1; i++){
				if(isdigit(line[i])){
					if(b1 == 0) continue;
					else if(b1 == 1 && n2 == 0) n2 = 1;
					else if(b1 == 1 && n2 == 1 && b2 == 0) continue;
					else if(b1 == 1 && n2 == 1 && b2 == 1 && n3 == 0) n3 = 1;
					else if(b1 == 1 && n2 == 1 && b2 == 1 && n3 == 1) continue;
				} else {
					if(b1 == 0) b1 = 1;
					else if(b1 == 1 && n2 == 0) continue;
					else if(b1 == 1 && n2 == 1 && b2 == 0) b2 = 1;
					else if(b1 == 1 && n2 == 1 && b2 == 1 && n3 == 0) continue;
					else if(b1 == 1 && n2 == 1 && b2 == 1 && n3 == 1 && b3 == 0) {b3 = 1; break;}
				}
			}
			if(b1 == 0 || n2 == 0 || b2 == 0 || n3 == 0 || b3 == 1){
				fprintf(stderr, "line %d of %s is not in the right format\nexiting program\n",lineNo, fileName);
				exit(-1);	
			}
			char *first_str=NULL, *second_str=NULL, *third_str=NULL;
    		parse_line(line, &first_str, &second_str, &third_str);
    		packet -> InterArrivalTime_us = 1000 * atoi(first_str);
    		packet -> RequiredTokens = atoi(second_str);
    		packet -> RequiredService_us = 1000 * atoi(third_str);
		}
		lineNo++;
		//at this point, we have obtained the IAT, RT and RS of packet in us
		gettimeofday(&bk1 , 0);
		if(numLeft == num) timersubb(&bk1, &beginTime, &dbk);
		else timersubb(&bk1, &bk2, &dbk);
		int totalBKT_us = 1000000 * dbk.tv_sec + dbk.tv_usec;
		if(packet -> InterArrivalTime_us > totalBKT_us){
			int sleepTime_us = packet -> InterArrivalTime_us  - totalBKT_us;
			int ST_sec = sleepTime_us / 1000000;
			int ST_usec = sleepTime_us % 1000000;
			usleep(ST_usec);
			while(ST_sec > 0){
				usleep(1);
				usleep(999999);
				ST_sec --;
			}
		}
		gettimeofday(&bk2 , 0);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
		pthread_mutex_lock(&mutex); //{
		if(shutdown){
			pthread_mutex_unlock(&mutex);
			pthread_testcancel();
		}	
		gettimeofday(&packet -> packetArrive, 0);
		printCurTimeStamp();
		printf("p%d arrives, needs %d tokens, inter-arrival time = ",packet -> numb, packet -> RequiredTokens );
		totalInterArrivalTime_s += printTimeDiff(&packet -> packetArrive, &prevArrive)/1000.0;
		prevArrive = packet -> packetArrive;
		if(packet -> RequiredTokens > B){
			printf(", dropped\n");
			droppedPackets ++;
			numLeftT --;
			goto L;
		}
		printf("\n");
		if(!My402ListAppend(Q1, (void*)packet)){
			fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
			exit(-1);
		}
		gettimeofday(&packet -> Q1Entry, 0);
		printCurTimeStamp();
		printf("p%d enters Q1\n", packet -> numb );
		L: ;
		My402ListElem* elem = My402ListFirst(Q1);
		if(elem == NULL) goto L1;
		packet = (Packet*)(elem -> obj);
		if(packet -> RequiredTokens <= bucketCount){
			bucketCount -= packet -> RequiredTokens;
			if(noneTransferred) noneTransferred = 0;
			My402ListUnlink(Q1, elem);
			gettimeofday(&packet -> Q1Leave, 0);
			printCurTimeStamp();
			printf("p%d leaves Q1, time in Q1 = ",packet -> numb);
			totalTimeQ1_s +=  printTimeDiff(&packet -> Q1Leave, &packet -> Q1Entry)/1000.0;
			printf(", token bucket now has %d tokens\n", bucketCount);
			if(!My402ListAppend(Q2, (void*)packet)){
			fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
			exit(-1);
			}
			gettimeofday(&packet -> Q2Entry, 0);
			printCurTimeStamp();
			printf("p%d enters Q2\n", packet -> numb );
			pthread_cond_broadcast(&queue_not_empty);
		}
		L1: numLeft --;	
		if(numLeft == 0) allDone = 1;
	    pthread_mutex_unlock(&mutex); //}
	
	}// while close
	if(numLeft == 0 && !shutdown && mode){
		line[0] = '\0';
		fgets(line, sizeof(line), tsfile);
		if(line[0] != '\0' && line[0] != '\n'){
			fprintf(stderr, "file %s doesn't have the right number of lines\nexiting programm\n",fileName);
			exit(-1);
		}
	}
	if(allDone){
		pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&queue_not_empty);
		pthread_mutex_unlock(&mutex);
	}
	return((void*)0);
}

void *tokenDepositFun(void* arg){
	struct timeval bk1, bk2, dbk;
	while(!shutdown){
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		gettimeofday(&bk1 , 0);
		if(totalTokens == 0) timersubb(&bk1, &beginTime, &dbk);
		else timersubb(&bk1, &bk2, &dbk);
		int totalBKT_us = 1000000 * dbk.tv_sec + dbk.tv_usec;
		if(tokenInterArrivalTime_ms * 1000 > totalBKT_us){
			int sleepTime_us = tokenInterArrivalTime_ms * 1000 - totalBKT_us;
			int ST_sec = sleepTime_us / 1000000;
			int ST_usec = sleepTime_us % 1000000;
			usleep(ST_usec);
			while(ST_sec > 0){
				usleep(1);
				usleep(999999);
				ST_sec --;
			}
		}
		gettimeofday(&bk2, 0);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
		pthread_mutex_lock(&mutex); //{
		if(shutdown){
			pthread_mutex_unlock(&mutex);
			pthread_testcancel();
		}	
		totalTokens ++;
		if(bucketCount < B){
			bucketCount++;
			printCurTimeStamp();
			printf("token t%d arrives, token bucket now has %d tokens\n", totalTokens, bucketCount);
		} else{
			printCurTimeStamp();
			printf("token t%d arrives, dropped\n", totalTokens);
			droppedTokens++;
		}
		My402ListElem* elem = My402ListFirst(Q1);
		if(elem == NULL) goto L2;
		Packet* packet = (Packet*)(elem -> obj);
		if(packet -> RequiredTokens <= bucketCount){
			bucketCount -= packet -> RequiredTokens;
			if(noneTransferred) noneTransferred = 0; 
			My402ListUnlink(Q1, elem);
			gettimeofday(&packet -> Q1Leave, 0);
			printCurTimeStamp();
			printf("p%d leaves Q1, time in Q1 = ",packet -> numb);
			totalTimeQ1_s += printTimeDiff(&packet -> Q1Leave, &packet -> Q1Entry)/1000.0;
			printf(", token bucket now has %d tokens\n", bucketCount);
			if(!My402ListAppend(Q2, (void*)packet)){
			fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
			exit(-1);
			}
			gettimeofday(&packet -> Q2Entry, 0);
			printCurTimeStamp();
			printf("p%d enters Q2\n", packet -> numb );
			pthread_cond_broadcast(&queue_not_empty);
		}	
		L2: if(allDone && My402ListEmpty(Q1)){
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex); //}		
	}
	if(allDone){
		pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&queue_not_empty);
		pthread_mutex_unlock(&mutex);
	}
	return((void*)0);
}

void *server1Fun(void* arg){
	while(!shutdown && numLeftT > 0){
		pthread_mutex_lock(&mutex); //{
		if(noneTransferred){
			pthread_mutex_unlock(&mutex);
			//if(allDone) break;
			continue;
		}
		if(allDone && My402ListEmpty(Q1) && My402ListEmpty(Q2)){
			A:pthread_mutex_unlock(&mutex);
			break;
		}	
		while(My402ListEmpty(Q2) && !shutdown){
			if(numLeftT == 0) goto A;
			pthread_cond_wait(&queue_not_empty ,&mutex);
			if(shutdown) {
				pthread_mutex_unlock(&mutex);
				goto C;
			} 
		}
		My402ListElem* elem = My402ListFirst(Q2);		
		Packet* packet = (Packet*)elem -> obj;
		My402ListUnlink(Q2, elem);
		gettimeofday(&packet -> Q2Leave, 0);
		printCurTimeStamp();
		printf("p%d leaves Q2, time in Q2 = ",packet -> numb);
		numLeftT --;
		totalTimeQ2_s += printTimeDiff(&packet -> Q2Leave, &packet -> Q2Entry)/1000.0;
		printf("\n");
		gettimeofday(&packet -> serviceBegin, 0);
		printCurTimeStamp();
		printf("p%d begins service at s1, requesting %dms of service\n",packet -> numb, packet -> RequiredService_us / 1000);
		pthread_mutex_unlock(&mutex); //}
			int sleepTime_us = packet -> RequiredService_us;
			int ST_sec = sleepTime_us / 1000000;
			int ST_usec = sleepTime_us % 1000000;
			usleep(ST_usec);
			while(ST_sec > 0){
				usleep(1);
				usleep(999999);
				ST_sec --;
			}
		pthread_mutex_lock(&mutex); //{
		gettimeofday(&packet -> serviceEnd, 0);
		printCurTimeStamp();
		printf("p%d departs from s1, service time = ",packet -> numb);
		double a = printTimeDiff(&packet -> serviceEnd, &packet -> serviceBegin)/1000.0;
		totalTimeS1_s += a;
		totalServiceTime_s += a;
		printf(", time in system = ");
		double aa = printTimeDiff(&packet -> serviceEnd, &packet -> packetArrive)/1000.0;
		totalSystemTime_s += aa;
		totalSystemTimeSq_sSq += aa * aa;
		printf("\n");	
		completedPackets ++;
		pthread_mutex_unlock(&mutex); //}	
	}
	if(numLeftT == 0){
		pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&queue_not_empty);
		pthread_mutex_unlock(&mutex);
	}
	C:return((void*)0);
}

void *server2Fun(void* arg){
	while(!shutdown && numLeftT > 0){
		pthread_mutex_lock(&mutex); //{
		if(noneTransferred){
			pthread_mutex_unlock(&mutex);
			//if(allDone) break;
			continue;
		}	
		if(allDone && My402ListEmpty(Q1) && My402ListEmpty(Q2)){
			B:pthread_mutex_unlock(&mutex);
			break;
		}	
		while(My402ListEmpty(Q2) && !shutdown){
			if(numLeftT == 0) goto B;
			pthread_cond_wait(&queue_not_empty ,&mutex);
			if(shutdown){ 
				pthread_mutex_unlock(&mutex);
				goto D;
			}
		}
		My402ListElem* elem = My402ListFirst(Q2);		
		Packet* packet = (Packet*)elem -> obj;
		My402ListUnlink(Q2, elem);
		gettimeofday(&packet -> Q2Leave, 0);
		printCurTimeStamp();
		printf("p%d leaves Q2, time in Q2 = ",packet -> numb);
		numLeftT --;
		totalTimeQ2_s += printTimeDiff(&packet -> Q2Leave, &packet -> Q2Entry)/1000.0;
		printf("\n");
		gettimeofday(&packet -> serviceBegin, 0);
		printCurTimeStamp();
		printf("p%d begins service at s2, requesting %dms of service\n",packet -> numb, packet -> RequiredService_us / 1000);
		pthread_mutex_unlock(&mutex); //}
			int sleepTime_us = packet -> RequiredService_us;
			int ST_sec = sleepTime_us / 1000000;
			int ST_usec = sleepTime_us % 1000000;
			usleep(ST_usec);
			while(ST_sec > 0){
				usleep(1);
				usleep(999999);
				ST_sec --;
			}
		pthread_mutex_lock(&mutex); //{
		gettimeofday(&packet -> serviceEnd, 0);
		printCurTimeStamp();
		printf("p%d departs from s2, service time = ",packet -> numb);
		double a = printTimeDiff(&packet -> serviceEnd, &packet -> serviceBegin)/1000.0;
		totalTimeS2_s += a;
		totalServiceTime_s += a;
		printf(", time in system = ");
		double aa = printTimeDiff(&packet -> serviceEnd, &packet -> packetArrive)/1000.0;
		totalSystemTime_s += aa;
		totalSystemTimeSq_sSq += aa * aa;
		printf("\n");	
		completedPackets ++;
		pthread_mutex_unlock(&mutex); //}	
	}
	if(numLeftT == 0){
		pthread_mutex_lock(&mutex);
		pthread_cond_broadcast(&queue_not_empty);
		pthread_mutex_unlock(&mutex);
	}
	D:return((void*)0);
}

void *monitor(void* arg){
	int sig;
	sigwait(&set, &sig);
	pthread_mutex_lock(&mutex);
	shutdown = 1;
	printf("\n");
	printCurTimeStamp();
	printf("SIGINT caught, no new packets or tokens will be allowed\n");
	pthread_cancel(arrival);
	pthread_cancel(tokenDeposit);
	pthread_cond_broadcast(&queue_not_empty);
	pthread_mutex_unlock(&mutex);
	return((void*)0);
}

int main(int argc, char *argv[]){
	SetDefaults();
 	ProcessOptions(argc, argv);
 	AdjustValuesEPP();
 	Q1 = (My402List*)malloc(sizeof(My402List));
 	Q2 = (My402List*)malloc(sizeof(My402List));
 	if(Q1 == NULL || Q2 == NULL){
 		fprintf(stderr, "Memory insufficient to continue program\nexiting program\n");
		exit(-1);	
	 }
 	My402ListInit(Q1);
 	My402ListInit(Q2);
 	bucketCount = completedPackets = droppedPackets = removedPackets = totalInterArrivalTime_s = totalServiceTime_s = totalTimeQ1_s = totalTimeQ2_s = totalTimeS1_s = totalTimeS2_s = totalSystemTime_s = totalSystemTimeSq_sSq = droppedTokens = totalTokens = shutdown = allDone = 0;
 	noneTransferred = 1;
 	mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
 	queue_not_empty = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
 	sigemptyset(&set);
 	sigaddset(&set, SIGINT);
 	sigprocmask(SIG_BLOCK, &set, 0);
 	gettimeofday(&beginTime, 0);
 	printf("00000000.000ms: emulation begins\n");
 	pthread_create(&arrival,0,arrivalFun,0);
 	pthread_create(&tokenDeposit,0,tokenDepositFun,0);
 	pthread_create(&server1,0,server1Fun,0);
 	pthread_create(&server2,0,server2Fun,0);
 	pthread_create(&sigCatch,0,monitor,0);
 	pthread_join(arrival,0);
 	pthread_join(tokenDeposit,0);
 	pthread_join(server1,0);
 	pthread_join(server2,0);
 	Packet* packet;
 	My402ListElem* elem;
 	while(!My402ListEmpty(Q1)){
		elem = My402ListFirst(Q1);		
		packet = (Packet*)elem -> obj;
		My402ListUnlink(Q1, elem);
		printCurTimeStamp();
		printf("p%d removed from Q1\n", packet -> numb);
		removedPackets ++;
 	}
 	while(!My402ListEmpty(Q2)){
		elem = My402ListFirst(Q2);		
		packet = (Packet*)elem -> obj;
		My402ListUnlink(Q2, elem);
		printCurTimeStamp();
		printf("p%d removed from Q2\n", packet -> numb);
		removedPackets ++;
 	}
 	gettimeofday(&endTime, 0);
 	struct timeval sToE;
 	timersubb(&endTime, &beginTime, &sToE);
 	double totalEmulationTime_s = (double)sToE.tv_sec + sToE.tv_usec/1000000.0;
 	printCurTimeStamp();
 	printf("emulation ends\n\nStatistics:\n\n");
 	double avgIAT, avgPST, avgPQ1, avgPQ2, avgPS1, avgPS2, avgTPS, SDTPS, TDP, PDP;
 	printf("average packet inter-arrival time = ");
 	if(completedPackets + droppedPackets + removedPackets == 0) printf("N/A no packets arrived\n");
 	else {
 		avgIAT = totalInterArrivalTime_s/(completedPackets + droppedPackets + removedPackets);
 		printf("%.6g\n", avgIAT);
 	}
 	printf("average packet service time = ");
 	if(completedPackets == 0) printf("N/A no packets served\n\n");
 	else{
 		avgPST = totalServiceTime_s/completedPackets;
 		printf("%.6g\n\n", avgPST);
 	}
 	printf("average number of packets in Q1 = ");
 	avgPQ1 = totalTimeQ1_s/totalEmulationTime_s;
 	printf("%.6g\n", avgPQ1);
 	printf("average number of packets in Q2 = ");
 	avgPQ2 = totalTimeQ2_s/totalEmulationTime_s;
 	printf("%.6g\n", avgPQ2);
 	printf("average number of packets in S1 = ");
 	avgPS1 = totalTimeS1_s/totalEmulationTime_s;
 	printf("%.6g\n", avgPS1);
 	printf("average number of packets in s2 = ");
 	avgPS2 = totalTimeS2_s/totalEmulationTime_s;
 	printf("%.6g\n\n", avgPS2);
 	printf("average time a packet spent in system = ");
 	if(completedPackets == 0) printf("N/A no packets completed\n");
 	else{
 		avgTPS = totalSystemTime_s/completedPackets;
 		printf("%.6g\n", avgTPS);
 	}
 	printf("standard deviation for time spent in system = ");
 	if(completedPackets == 0) printf("N/A no packets completed\n\n");
 	else if(completedPackets == 1){
 		SDTPS = 0.0;
 		printf("%.6g\n\n", SDTPS);
 	}
 	else{
 		SDTPS = sqrt(totalSystemTimeSq_sSq/completedPackets - avgTPS * avgTPS);
 		printf("%.6g\n\n", SDTPS);
 	}
 	printf("token drop probability = ");
 	if(totalTokens == 0) printf("N/A no tokens arrived\n");
 	else{
 		TDP = droppedTokens/(double)totalTokens;
 		printf("%.6g\n", TDP);
 	}
 	printf("packet drop probability = ");
 	if(completedPackets + droppedPackets + removedPackets == 0) printf("N/A no packets arrived\n");
 	else{
 		PDP = (double)droppedPackets/(completedPackets + droppedPackets + removedPackets);
 		printf("%.6g\n", PDP);
 	}
 	if(mode) fclose(tsfile); 
 	return 0;
}
