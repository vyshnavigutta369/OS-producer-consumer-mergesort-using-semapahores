

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>


void display(int array[], int length)
{
	int i;
	printf(">");
	for (i = 0; i < length; i++)
		printf(" %d", array[i]);
	printf("\n");
}


void selectionsort(int* b,int l,int h)
{
	int i,j;
	int x;
        int count=0;
        for(i=l;i<=h;i++)
	{
		for(j=i;j<=h;j++)
		{
			if(b[i]>b[j])
			{
				x=b[i];
				b[i]=b[j];
				b[j]=x;
			}
		}
	}
	return;
}

void merge(int *left, int llength, int *right, int rlength)
{
	/* Temporary memory locations for the 2 segments of the array to merge. */
        if(rlength-llength < 4 && rlength-llength>0)
         {
             selectionsort(left,llength,rlength); 
             return;
         }
            
              
	int *ltmp = (int *) malloc(llength * sizeof(int));
	int *rtmp = (int *) malloc(rlength * sizeof(int));

	/*
	 * Pointers to the elements being sorted in the temporary memory locations.
	 */
	int *ll = ltmp;
	int *rr = rtmp;

	int *result = left;

	/*
	 * Copy the segment of the array to be merged into the temporary memory
	 * locations.
	 */
	memcpy(ltmp, left, llength * sizeof(int));
	memcpy(rtmp, right, rlength * sizeof(int));

	while (llength > 0 && rlength > 0) {
		if (*ll <= *rr) {
			/*
			 * Merge the first element from the left back into the main array
			 * if it is smaller or equal to the one on the right.
			 */
			*result = *ll;
			++ll;
			--llength;
		} else {
			/*
			 * Merge the first element from the right back into the main array
			 * if it is smaller than the one on the left.
			 */
			*result = *rr;
			++rr;
			--rlength;
		}
		++result;
	}
	/*
	 * All the elements from either the left or the right temporary array
	 * segment have been merged back into the main array.  Append the remaining
	 * elements from the other temporary array back into the main array.
	 */
	if (llength > 0)
		while (llength > 0) {
			/* Appending the rest of the left temporary array. */
			*result = *ll;
			++result;
			++ll;
			--llength;
		}
	else
		while (rlength > 0) {
			/* Appending the rest of the right temporary array. */
			*result = *rr;
			++result;
			++rr;
			--rlength;
		}

	/* Release the memory used for the temporary arrays. */
	free(ltmp);
	free(rtmp);
}

void mergesort(int array[], int length)
{
	/* This is the middle index and also the length of the right array. */
	int middle;

	/*
	 * Pointers to the beginning of the left and right segment of the array
	 * to be merged.
	 */
	int *left, *right;

	/* Length of the left segment of the array to be merged. */
	int llength;

	int lchild = -1;
	int rchild = -1;

	int status;

	if (length <= 1)
		return;

	/* Let integer division truncate the value. */
	middle = length / 2;

	llength = length - middle;

	/*
	 * Set the pointers to the appropriate segments of the array to be merged.
	 */
	left = array;
	right = array + llength;

	lchild = fork();
	if (lchild < 0) {
		perror("fork");
		exit(1);
	}
	if (lchild == 0) {
		mergesort(left, llength);
		exit(0);
	} else {
		rchild = fork();
		if (rchild < 0) {
			perror("fork");
			exit(1);
		}
		if (rchild == 0) {
			mergesort(right, middle);
			exit(0);
		}
	}
	waitpid(lchild, &status, 0);
	waitpid(rchild, &status, 0);
	merge(left, llength, right, middle);
}

int main(int argc, char *argv[])
{
	int *array = NULL;
	int length;
	FILE *fh;
	int data,j;

	int *shm_array;
	int shm_id;
	key_t key;
	int i;
	size_t shm_size;
	scanf("%d",&length);

	for(j=0;j<length;j++)
	{
		scanf("%d",&data);
		array = (int *) realloc(array, length * sizeof(int));
		array[j] = data;
	}

	printf("%d elements read\n", length);

	shm_size = length * sizeof(int);
	if ((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1) {
		perror("shmget");
		exit(1);
	}

	if ((shm_array = shmat(shm_id, NULL, 0)) == (int *) -1) {
		perror("shmat");
		exit(1);
	}

	for (i = 0; i < length; i++) {
		shm_array[i] = array[i];
	}

	display(shm_array, length);
	mergesort(shm_array, length);
	printf("done sorting\n");
	display(shm_array, length);

	if (shmdt(shm_array) == -1) {
		perror("shmdt");
		exit(1);
	}

	if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		exit(1);
	}

	return 0;
}
