#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/semaphore.h>

// for sleep
#include <unistd.h>

int BUFF_SIZE;
int NC;
/*#define BUFF_SIZE   5           
#define NC          3*/           
#define NP          1           
#define NITERS      18       /* number of items produced/consumed */

typedef struct
{
	int buf[20];   /* shared var */
	int in;               /* buf[in%BUFF_SIZE] is the first empty slot */
	int out;              /* buf[out%BUFF_SIZE] is the first full slot */
	sem_t full;           /* keep track of the number of full spots */
	sem_t empty;          /* keep track of the number of empty spots */

	// use correct type here
	pthread_mutex_t mutex;          /* enforce mutual exclusion to shared data */

} sbuf_t;

sbuf_t shared;
int lo;
typedef struct
{
        int val;
	int a[20];
	int flag;
} repeat;

repeat b[20];

void *Producer(void *arg)
{
	int i, item=0, index;

	index = (int)arg;


	for (i=0; i < NITERS; i++)
	{

		/* Produce item */
	//	item = i;

		/* Prepare to write item to buf */

		/* If there are no empty slots, wait */
		sem_wait(&shared.empty);
		/* If another thread uses the buffer, wait */
		pthread_mutex_lock(&shared.mutex);
		if (b[shared.in].a[0]==NC || shared.buf[shared.in]==0)
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
			/* Release the buffer */
		}
		pthread_mutex_unlock(&shared.mutex);
		/* Increment the number of full slots */
		sem_post(&shared.full);

		/* Interleave  producer and consumer execution */
		if (i%2==1) sleep(1);
	}
	return NULL;
}

void *Consumer(void *arg)
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
			/* Release the buffer */
		}
		pthread_mutex_unlock(&shared.mutex);
		/* Increment the number of full slots */
		sem_post(&shared.empty);

		/* Interleave  producer and consumer execution */
		if (i%2==1) sleep(1);
	}
	return NULL;
}

int main()
{
	pthread_t idP, idC;
	int index;

	//  printf("%d\n",shared.buf[0]);
	scanf("%d%d",&BUFF_SIZE,&NC);
	sem_init(&shared.full, 0, 0);
	sem_init(&shared.empty, 0, BUFF_SIZE);
	pthread_mutex_init(&shared.mutex, NULL);
	for (index = 0; index < NP; index++)
	{
		/* Create a new producer */
		pthread_create(&idP, NULL, Producer, (void*)index);
	}
	/*create a new Consumer*/
	for(index=0; index<NC; index++)
	{
		pthread_create(&idC, NULL, Consumer, (void*)index);
	}



	pthread_exit(NULL);
}
