/* 201021323 구광민 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

//파일을 한 줄 읽을때의 배열의 길이
#define line 10


//PCB
typedef struct PCB{
	unsigned long arrival_time;
	unsigned long start_time;
	unsigned long finish_time;
	int progress;
}PCB;

//Process 정보
typedef struct Process{
	int id;
	unsigned long exe_time;
}Process;

//Node 구조
typedef struct Node{
	Process* pro;
	PCB* pcb;
	struct Node* next;
}Node;

//List 타입
typedef struct LinkedListType{
	Node* head;
	int length;
}LinkedListType;

//Queue 타입
typedef struct QueueType{
	Node* front;
	Node* rear;
	int length;
}QueueType;


//프로토타입
Process* new_process ( int new_id, unsigned long new_exe_time );
PCB* new_PCB (unsigned long arrival);
Node* new_node( Process* new_pro, PCB* new_pcb);
void init_list( LinkedListType* list );
void add_list( LinkedListType* list, Node* new_node );
Node* search_list( LinkedListType* list, int pro_id );
void init_queue( QueueType* queue );
void push_queue( QueueType* queue, Node* node );
void pop_queue( QueueType* queue );




//main 함수
int main (void){

	//변수 선언 등 초기화
	FILE *file = fopen("process.txt", "rt");
	char str[line];
	int pro_len, process_id;
	int exe_id, ready_id = 1;
	unsigned long arrival_time, execution_time;
	LinkedListType List;
	QueueType Queue;
	unsigned int t = 0;
	unsigned int cpu_idle = 0;
	Node* exe_node = NULL;
	Node change;
	unsigned int turnaround;
	unsigned int waiting;
	float util;
	float util2;
	float ql = 0;

	//매개변수 생성
	Process* tmp_process = NULL;
	PCB* tmp_PCB = NULL;
	Node* tmp_node = NULL;


	//List와 Queue 초기화
	init_list(&List);
	init_queue(&Queue);

	//Process 개수 받기(pro_len)
	fscanf( file, "%s", str);
	pro_len = atoi(str);

	//한 column씩 받기
	while ( 1 ){

		//Process id 받기
		fscanf( file, "%s", str);

		//파일이 끝났다면 종료
		if ( feof(file) != 0 ) break;
		process_id = atoi(str);

		//arrival time 받기
		fscanf( file, "%s", str);
		arrival_time = strtoul(str, NULL, 10);

		//execution time 받기
		fscanf( file, "%s", str);
		execution_time = strtoul(str, NULL, 10);


		//Node를 생성하여 List에 추가
		tmp_process = new_process( process_id, execution_time );
		tmp_PCB = new_PCB(arrival_time);
		tmp_node = new_node( tmp_process, tmp_PCB );

		add_list( &List, tmp_node );

	}

	fclose(file);

	//프로세스 개수 monitor
	if( pro_len != List.length ){
		printf("입력한 프로세스 개수와 프로세스 개수가 다릅니다.\n");
		return 0;
	}


	//프로세스 처리과정
	while ( 1 ){

		//arrival time을 t와 비교하여 queue에 push
		while ( 1 ){
            if ( pro_len + 1 != ready_id ){
                tmp_node = search_list(&List, ready_id);
                if ( tmp_node->pcb->arrival_time == t ){
                    push_queue(&Queue, tmp_node);
                    printf("[%u] PID(%d): Entered to queue\n", t, ready_id);
                    ready_id++;
                    continue;
                }
            }
            break;
		}

		//CPU를 점유중
		if( exe_node != NULL ){

			//CPU 점유시간 증가
			exe_node->pcb->progress++;

			//Process 완료시
			if( exe_node->pcb->progress == exe_node->pro->exe_time ){
				Node* tmp_node2 = NULL;
				exe_node->pcb->finish_time = t;

				//List update
				tmp_node = search_list(&List, exe_id);
				tmp_node2 = tmp_node->next;
				*tmp_node = *exe_node;
				tmp_node->next = tmp_node2;
				exe_node = NULL;
				printf("[%u] PID(%d): Finished running\n", t, exe_id);

				if( exe_id == pro_len )
					break;

				//Queue가 비었을 때
				if ( Queue.front == NULL )
					cpu_idle++;

				//Queue에 있을 때
				else{
					change = *(Queue.front);
					exe_node = &change;
					pop_queue(&Queue);
					exe_node->pcb->start_time = t;
					exe_id = exe_node->pro->id;
					printf("[%u] PID(%d): Started running\n", t, exe_id);
				}
			}
		}

		//CPU를 점유중이지 않음
		else if ( exe_node == NULL ){

			//Queue가 비었을 때
			if ( Queue.front == NULL )
				cpu_idle++;

			//Queue에 있을 때
			else{
				change = *(Queue.front);
				exe_node = &change;
				pop_queue(&Queue);
				exe_node->pcb->start_time = t;
				exe_id = exe_node->pro->id;
				printf("[%u] PID(%d): Started running\n", t, exe_id);
			}
		}

		//에러
		else{
			printf("error!\n");
			return 0;
		}

		ql += Queue.length;
		t++;
	}

	//Log of Process Scheduling
	printf("\nLog of Process Scheduling\n");

	tmp_node = List.head;

	//Turnaround Time, Waiting Time
	while ( tmp_node != NULL ){

		exe_id = tmp_node->pro->id;
		turnaround = tmp_node->pcb->finish_time - tmp_node->pcb->arrival_time;
		waiting = tmp_node->pcb->start_time - tmp_node->pcb->arrival_time;

		printf("PID(%d) Turnaround Time: %u Waiting Time: %u\n", exe_id, turnaround, waiting);

		tmp_node = tmp_node->next;
	}

	turnaround = 0;
	waiting = 0;
	tmp_node = List.head;


	//Average Turnaround Time, Waiting Time
	while ( tmp_node != NULL ){

		exe_id = tmp_node->pro->id;
		turnaround += tmp_node->pcb->finish_time - tmp_node->pcb->arrival_time;
		waiting += tmp_node->pcb->start_time - tmp_node->pcb->arrival_time;

		tmp_node = tmp_node->next;
	}

	printf("\nAverage Turnaround Time: %u\n", turnaround/pro_len);
	printf("Average Waiting Time: %u\n", waiting/pro_len);

	//CPU Utilization
	util = (t - cpu_idle) * 100;
	util2 = util / t;
	printf("CPU Utilization: %f%%\n", util2 );

	//Average Queue Length
	printf("Average Queue Length: %f\n", ql/t);


	return 0;
}


//새로운 Process 생성
Process* new_process ( int new_id, unsigned long new_exe_time ){
	Process* new_node;

	new_node = (Process *)malloc(sizeof(Process));
	new_node->id = new_id;
	new_node->exe_time = new_exe_time;

	return new_node;
}


//새로운 PCB 생성
PCB* new_PCB (unsigned long arrival){
	PCB* new_node;


	new_node = (PCB*)malloc(sizeof(PCB));

	//arrival time만 받고
	new_node->arrival_time = arrival;

	//모두 0으로 init되어 생성
	new_node->finish_time = 0;
	new_node->start_time = 0;
	new_node->progress = 0;

	return new_node;
}


//새로운 Node 생성
Node* new_node( Process* new_pro, PCB* new_pcb){
	Node* new_nod;

	new_nod = (Node*)malloc(sizeof(Node));
	new_nod->pro = new_pro;
	new_nod->pcb = new_pcb;
	new_nod->next = NULL;

	return new_nod;
}


//List 초기화
void init_list( LinkedListType* list ){
	list->head = NULL;
	list->length = 0;
}


//List 추가
void add_list( LinkedListType* list, Node* new_node ){
	int i, j;
	Node* tmp;

	i = list->length;

	//List가 비었을 때
	if ( i == 0 ){
		list->head = new_node;
		list->length = 1;
	}

	//List가 있을 때
	else{
		tmp = list->head;

		//tmp를 마지막 노드로 옮김
		for(j=0; j<i-1; j++)
			tmp = tmp->next;

		//List에 추가
		tmp->next = new_node;

		list->length++;
	}
}


//Process ID를 이용한 List에서 노드탐색
Node* search_list( LinkedListType* list, int pro_id ){
	Node* result;

	result = list->head;

	while(1){
		if ( result == NULL )
			return result;

		else if ( result->pro->id == pro_id )
			return result;

		else
			result = result->next;
	}
}


//Queue 초기화
void init_queue( QueueType* queue ){
	queue->front = NULL;
	queue->rear = NULL;
	queue->length = 0;
}


//Queue push
void push_queue( QueueType* queue, Node* node ){
	int i;

	i = queue->length;

	//queue가 비었을 때
	if ( i == 0 ){
		Node* new_node;

		new_node = (Node*)malloc(sizeof(Node));
		new_node->pro = NULL;
		new_node->pcb = NULL;
		new_node->next = NULL;

		*new_node = *node;
		new_node->next = NULL;

		queue->front = new_node;
		queue->rear = new_node;

		queue->length = 1;
	}

	else{
		Node* new_node;

		new_node = (Node*)malloc(sizeof(Node));
		new_node->pro = NULL;
		new_node->pcb = NULL;
		new_node->next = NULL;

		*new_node = *node;
		new_node->next = NULL;

		queue->rear->next = new_node;
		new_node->next = queue->rear;

		queue->length++;
	}
}


//Queue pop
void pop_queue( QueueType* queue ){
	Node* tmp;

		tmp = queue->front;
		queue->front = queue->front->next;

		free(tmp);
		queue->length--;
}
