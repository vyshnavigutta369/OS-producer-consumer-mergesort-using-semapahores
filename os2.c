#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/semaphore.h>

// for sleep
#include <unistd.h>

int BUFF_SIZE;
int NP;
#define NITERS      6       /* number of items produced/consumed */
#define MAX 20

typedef struct
{
	int buf[20];   /* shared var */
	int in;               /* buf[in%BUFF_SIZE] is the first empty slot */
	int out;              /* buf[out%BUFF_SIZE] is the first full slot */
	sem_t empty;          /* keep track of the number of empty spots */

	pthread_mutex_t mutex;          /* enforce mutual exclusion to shared data */

} sbuf_t;

sbuf_t shared;
int lo;
int count=0;
int front=-1;
int rear=-1;
int flag;
int stat=0;
int queue[20];


void *Producer(void *arg)
{
	int i, item=0, index,element;

	index = (int)arg;

	srand(time(0));

	for (i=0; i < NITERS; i++)
	{

		/* Produce item */
		//	item = i;

		/* Prepare to write item to buf */

		/* If there are no emptNy slots, wait */
		sem_wait(&shared.empty);
		/* If another thread uses the buffer, wait */
		pthread_mutex_lock(&shared.mutex);
		int z= rand() % 2 +1;
		count++;
		//                printf("buff size:%d\n",BUFF_SIZE);
		if(count==1 && stat==0)
		{
			flag=z;
			stat=1;
		}

		if(count<=BUFF_SIZE && flag==z)
		{
			count++;
			printf("%d in the buffer\n",z);
		}
		if(flag!=z || count>=BUFF_SIZE)
		{
			printf("%d waiting in the queue\n",z);
			//	count=0;
			if(count==BUFF_SIZE)
				flag=queue[front];
			if(front==0 && rear==-1)
				printf("\n Queue OverFlow Occured");
			else if(front==-1 && rear==-1)
			{
				front=rear=0;
				queue[rear]=z;

			}
			else if(rear==MAX-1 && front!=0)
			{
				rear=0;
				queue[rear]=z;
			}
			else
			{
				rear++;
				queue[rear]=z;
			}
		}


		if(count!=0) 
			count=0;

		//	printf("%d\n",z);
		/*		if (b[shared.in].a[0]==NC || shared.buf[shared.in]==0)


				{

				item++;
				shared.buf[shared.in] = item;
				b[shared.in].val=item;
				lo=shared.in;
				shared.in = (shared.in+1)%BUFF_SIZE;
		//		b[item+1].a[index+1]=1;
		printf("[P%d] Producing  %d at %d...\n", index, item,lo);
		if ( b[lo].a[0]==NC)
		b[lo].a[0]=0;

		fflush(stdout);
		}*/
		pthread_mutex_unlock(&shared.mutex);
		/* Increment the number of full slots */
		sem_post(&shared.empty);

		/* Interleave  producer and consumer execution */
		if (i%2==1) sleep(1);
	}
	return NULL;
}

/*void *Consumer(void *arg)
  {
  int i, item, index;

  index = (int)arg;
  for (i=NITERS; i > 0; i--) {
//a[shared.out]++;
// printf("%d consumed by %d consumers\n",shared.out,a[shared.out]);
sem_wait(&shared.full);
pthread_mutex_lock(&shared.mutex);
printf("--arrived---[C%d]\n",index);
if (b[b[shared.out].val].a[index+1]!=1)
{
b[shared.out].a[0]++;
//			printf("location %d containing %d consumed by %d consumers\n",shared.out,shared.buf[shared.out],b[shared.out].a[0]);

item=i;
item=shared.buf[shared.out];
lo=shared.out;
b[b[shared.out].val].a[index+1]=1;

shared.out = (shared.out+1)%BUFF_SIZE;
printf("[C%d] Consuming  %d at %d consumed by %d....\n", index,b[lo].val,lo,b[lo].a[0]);
fflush(stdout);
}
pthread_mutex_unlock(&shared.mutex);
sem_post(&shared.empty);

if (i%2==1) sleep(1);
}
return NULL;
}*/

int main()
{
	pthread_t idP;
	int index;

	//  printf("%d\n",shared.buf[0]);
	scanf("%d%d",&BUFF_SIZE,&NP);
	//	sem_init(&shared.full, 0, 0);
	sem_init(&shared.empty, 0, BUFF_SIZE);
	pthread_mutex_init(&shared.mutex, NULL);
	for (index = 0; index < NP; index++)
	{
		/* Create a new producer */
		pthread_create(&idP, NULL, Producer, (void*)index);
	}
	/*create a new Consumer*/
	/*	for(index=0; index<NC; index++)
		{
		pthread_create(&idC, NULL, Consumer, (void*)index);
		}*/



	pthread_exit(NULL);

}
