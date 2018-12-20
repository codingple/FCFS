/* 201021323 ������ */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

//������ �� �� �������� �迭�� ����
#define line 10


//PCB
typedef struct PCB{
	unsigned long arrival_time;
	unsigned long start_time;
	unsigned long finish_time;
	int progress;
}PCB;

//Process ����
typedef struct Process{
	int id;
	unsigned long exe_time;
}Process;

//Node ����
typedef struct Node{
	Process* pro;
	PCB* pcb;
	struct Node* next;
}Node;

//List Ÿ��
typedef struct LinkedListType{
	Node* head;
	int length;
}LinkedListType;

//Queue Ÿ��
typedef struct QueueType{
	Node* front;
	Node* rear;
	int length;
}QueueType;


//������Ÿ��
Process* new_process ( int new_id, unsigned long new_exe_time );
PCB* new_PCB (unsigned long arrival);
Node* new_node( Process* new_pro, PCB* new_pcb);
void init_list( LinkedListType* list );
void add_list( LinkedListType* list, Node* new_node );
Node* search_list( LinkedListType* list, int pro_id );
void init_queue( QueueType* queue );
void push_queue( QueueType* queue, Node* node );
void pop_queue( QueueType* queue );




//main �Լ�
int main (void){

	//���� ���� �� �ʱ�ȭ
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

	//�Ű����� ����
	Process* tmp_process = NULL;
	PCB* tmp_PCB = NULL;
	Node* tmp_node = NULL;


	//List�� Queue �ʱ�ȭ
	init_list(&List);
	init_queue(&Queue);

	//Process ���� �ޱ�(pro_len)
	fscanf( file, "%s", str);
	pro_len = atoi(str);

	//�� column�� �ޱ�
	while ( 1 ){

		//Process id �ޱ�
		fscanf( file, "%s", str);

		//������ �����ٸ� ����
		if ( feof(file) != 0 ) break;
		process_id = atoi(str);

		//arrival time �ޱ�
		fscanf( file, "%s", str);
		arrival_time = strtoul(str, NULL, 10);

		//execution time �ޱ�
		fscanf( file, "%s", str);
		execution_time = strtoul(str, NULL, 10);


		//Node�� �����Ͽ� List�� �߰�
		tmp_process = new_process( process_id, execution_time );
		tmp_PCB = new_PCB(arrival_time);
		tmp_node = new_node( tmp_process, tmp_PCB );

		add_list( &List, tmp_node );

	}

	fclose(file);

	//���μ��� ���� monitor
	if( pro_len != List.length ){
		printf("�Է��� ���μ��� ������ ���μ��� ������ �ٸ��ϴ�.\n");
		return 0;
	}


	//���μ��� ó������
	while ( 1 ){

		//arrival time�� t�� ���Ͽ� queue�� push
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

		//CPU�� ������
		if( exe_node != NULL ){

			//CPU �����ð� ����
			exe_node->pcb->progress++;

			//Process �Ϸ��
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

				//Queue�� ����� ��
				if ( Queue.front == NULL )
					cpu_idle++;

				//Queue�� ���� ��
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

		//CPU�� ���������� ����
		else if ( exe_node == NULL ){

			//Queue�� ����� ��
			if ( Queue.front == NULL )
				cpu_idle++;

			//Queue�� ���� ��
			else{
				change = *(Queue.front);
				exe_node = &change;
				pop_queue(&Queue);
				exe_node->pcb->start_time = t;
				exe_id = exe_node->pro->id;
				printf("[%u] PID(%d): Started running\n", t, exe_id);
			}
		}

		//����
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


//���ο� Process ����
Process* new_process ( int new_id, unsigned long new_exe_time ){
	Process* new_node;

	new_node = (Process *)malloc(sizeof(Process));
	new_node->id = new_id;
	new_node->exe_time = new_exe_time;

	return new_node;
}


//���ο� PCB ����
PCB* new_PCB (unsigned long arrival){
	PCB* new_node;


	new_node = (PCB*)malloc(sizeof(PCB));

	//arrival time�� �ް�
	new_node->arrival_time = arrival;

	//��� 0���� init�Ǿ� ����
	new_node->finish_time = 0;
	new_node->start_time = 0;
	new_node->progress = 0;

	return new_node;
}


//���ο� Node ����
Node* new_node( Process* new_pro, PCB* new_pcb){
	Node* new_nod;

	new_nod = (Node*)malloc(sizeof(Node));
	new_nod->pro = new_pro;
	new_nod->pcb = new_pcb;
	new_nod->next = NULL;

	return new_nod;
}


//List �ʱ�ȭ
void init_list( LinkedListType* list ){
	list->head = NULL;
	list->length = 0;
}


//List �߰�
void add_list( LinkedListType* list, Node* new_node ){
	int i, j;
	Node* tmp;

	i = list->length;

	//List�� ����� ��
	if ( i == 0 ){
		list->head = new_node;
		list->length = 1;
	}

	//List�� ���� ��
	else{
		tmp = list->head;

		//tmp�� ������ ���� �ű�
		for(j=0; j<i-1; j++)
			tmp = tmp->next;

		//List�� �߰�
		tmp->next = new_node;

		list->length++;
	}
}


//Process ID�� �̿��� List���� ���Ž��
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


//Queue �ʱ�ȭ
void init_queue( QueueType* queue ){
	queue->front = NULL;
	queue->rear = NULL;
	queue->length = 0;
}


//Queue push
void push_queue( QueueType* queue, Node* node ){
	int i;

	i = queue->length;

	//queue�� ����� ��
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
