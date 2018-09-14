#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define FLOW_SIZE 100 		//number of process flow of gantt chart
#define QUEUE_SIZE 100

//Stores process information
typedef struct{
	char processName[16];	//name of process
	int arrivalTime;		//arrival time of process
	int burstTime;			//burst time of process
}PROCESS;

//Stores flow of gantt chart
typedef struct{
	int start;  			//start time of process
	int finish; 			//finish time of process
	char processName[16];	//name of process
}PROCESS_FLOW;

//PROTOTYPES
PROCESS* createProcess(int); 		//creates process array
int setProcessInfo(PROCESS*, int);  //gets inputs from user , stores process array and returns total burst time
void RRSchedule(PROCESS *, int , int , int , PROCESS_FLOW []); //Scheduler function

//Main Function
int main(void){
	PROCESS *process;   //stores processes
	PROCESS_FLOW pFlow[FLOW_SIZE]; //stores process flow

	int processNumber;  //process number
	int qt;				//quantum time
	int totalBurstTime;	//sum of all burst times
	
	printf("Please enter the process number : "); // getting process number from user
	scanf("%d",&processNumber);
	process = createProcess(processNumber); //creating process array
	
	printf("\nPlease enter the Q number : ");		//getting quantum time from user
	scanf("%d",&qt);
	
	//setting infomation of processes to process array and return totalBurstTime
	totalBurstTime = setProcessInfo(process, processNumber); 
	//Schedules, calculetes awt and writes gannt chart and awt to cikti.txt
	RRSchedule(process,processNumber,totalBurstTime,qt,pFlow);
		
	return 0;
}
/**	creates process array
*	@param1 : number of processes
*/
PROCESS* createProcess(int processNumber){
	PROCESS *p; 
	//memory allocation
	p = malloc(sizeof(PROCESS)*processNumber);
	if(!p){
		printf("\nError : Cannnot allocate memory");
		exit(1);
	}
	
	return p;
}

/**	gets inputs from user, stores them to process array and returns total burst time
*	@param1 : process array
*	@param2 : number of processes
*	return : sum of all burst times
*/
int setProcessInfo(PROCESS *p, int processNumber){
	int i; //index
	int totalBurstTime = 0;
	char name[16];		//name of process
	int arrivalTime;	//arrival time of process
	int burstTime;		//burst time of process
	
	for(i = 0; i < processNumber; i++){
		printf("\nEnter %d. process name : ",i+1);
		scanf("%s",name);			strcpy(p[i].processName,name);
		
		printf("\nEnter %d. process arrival time : ",i+1);
		scanf("%d",&arrivalTime);	p[i].arrivalTime = arrivalTime;
		
		printf("\nEnter %d. process burst time : ",i+1);
		scanf("%d",&burstTime);		p[i].burstTime = burstTime;
		
		totalBurstTime += burstTime; //calculates totalbursttime while getting values
	}
	return totalBurstTime;
}
/**	Schedules processes , calculates average waiting time and writes gantt chart and awt to cikti.txt
*	@param1 : process array
*	@param2 : length of process array
*	@param3 : number of processes
*	@param4 : sum of all burst times
*	@param5 : quantum time
*	@param6 : process flow array
*/
void RRSchedule(PROCESS *p, int processNumber, int totalBurstTime, int qt, PROCESS_FLOW pFlow[]) {
	int currentTime = 0;	//default current time 
	int i, j; 					//index
	int pFlowCounter = 0;	//index of pFlow array
	int waitingTime = 0;	//sum of all waiting times
	FILE *fp;				//file pointer
	PROCESS queue[QUEUE_SIZE];	//ready queue
	int qBegin = 0;			// head of queue
	int qEnd = 0;			// tail of queue
	int qCurrent;			// index for has not arrived processes
	int updateQCurrent;		// update qCurrent
	int updateQEnd;			// updating qEnd variable
	
	//initialize queue with ready processes
	for(j = 0; j < processNumber; j++){
		if(p[j].arrivalTime == 0){
			queue[qEnd++] = p[j];
			qCurrent = j;
		}
	}
	
	//ready queue of scheduler has not finished
	while(currentTime < totalBurstTime){
		updateQEnd = qEnd;
		for(i = qBegin; i < qEnd; i++) {
			//calc waiting time	
			waitingTime += (currentTime - queue[i].arrivalTime); 
			//save process name
			strcpy(pFlow[pFlowCounter].processName , queue[i].processName);
			//save start time
			pFlow[pFlowCounter].start = currentTime; 
			
			//controls burst time less or higher than qt and updates current time
			if(queue[i].burstTime < qt){
				currentTime += queue[i].burstTime;
				queue[i].burstTime = 0;
			}
			else{
				queue[i].burstTime -= qt;
				currentTime += qt;
			}
			//set new arrival time
			queue[i].arrivalTime = currentTime;
			//save finish time
			pFlow[pFlowCounter++].finish = currentTime;
			
			//if there is new arrival
			updateQCurrent = qCurrent;
			for(j = updateQCurrent+1 ; j < processNumber; j++){
				if(p[j].arrivalTime <= currentTime){
					queue[updateQEnd++] = p[j];
					qCurrent = j;
				}
			}
			// if process has not finished
			if(queue[i].burstTime != 0){
				queue[updateQEnd++] = queue[i];
			}
		}
		qBegin = qEnd;
		qEnd = updateQEnd;	
	}
	
	//opening file
	fp = fopen("output.txt","w+");
	if(!fp){
		printf("\nError : file cannot be created!");
		exit(1);
	}
	//Informations are writing to file
	for(i = 0; i < pFlowCounter; i++){
		fprintf(fp,"%s\t%d - %d\n",pFlow[i].processName, pFlow[i].start, pFlow[i].finish);
	}
	fprintf(fp,"\nawt : %f",(float)waitingTime / processNumber);
	
	printf("\nFile has successfully saved ! ");
	//file stream is closing
	fclose(fp);
}
