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

double lambda;
double mu;
double r;
int B;
int P;
int num;
int mode; // 0 is deterministic 1 is trace driven
FILE* tsfile;
char fileName[1024];
int packetInterArrivalTime_ms;
int packetServiceTime_ms;
int tokenInterArrivalTime_ms;

My402List* Q1;
My402List* Q2;

int bucketCount;
int numLeftT;

pthread_t arrival;
pthread_t tokenDeposit;
pthread_t server1;
pthread_t server2;
pthread_t sigCatch;

sigset_t set;

pthread_mutex_t mutex;
pthread_cond_t queue_not_empty;

//stat vars
int completedPackets;
int droppedPackets;
int removedPackets;
double totalInterArrivalTime_s;
double totalServiceTime_s;
double totalTimeQ1_s;
double totalTimeQ2_s;
double totalTimeS1_s;
double totalTimeS2_s;
double totalSystemTime_s;
double totalSystemTimeSq_sSq;
int droppedTokens;
int totalTokens;

int shutdown;// for cntrl+C
int allDone;
int noneTransferred;

struct timeval beginTime, endTime;



typedef struct tagPacket {
	int numb;
	int InterArrivalTime_us;
	int RequiredTokens;
	struct timeval Q1Entry;
	struct timeval Q1Leave;
	struct timeval Q2Entry;
	struct timeval Q2Leave;
	int RequiredService_us;
	struct timeval serviceBegin;
	struct timeval serviceEnd;
	struct timeval packetArrive;
} Packet;


void Usage();
void ProcessOptions(int argc, char *argv[]);
void SetDefaults();
void HandleFileError();
void AdjustValuesEPP();
void *arrivalFun(void* arg);
void *tokenDepositFun(void* arg);
void *server1Fun(void* arg);
void *server2Fun(void* arg);
void *monitor(void* arg);
void parse_line(char *line, char **addr_of_first_str, char **addr_of_second_str, char **addr_of_third_str);
void printCurTimeStamp();
double printTimeDiff(struct timeval* t2, struct timeval* t1);
void timersubb(struct timeval *a, struct timeval *b, struct timeval *res);

