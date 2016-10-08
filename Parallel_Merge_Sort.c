#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define N 1000000   //Size of array, should be multiple of number of threads
#define THREADS 8   //Number of threads, should be a power of 2
int values[N];
int values1[N];
pthread_t* thread_handles;



//input of thread functions. position of the array for that thread.
typedef struct Arr {
   int low;
   int high;
} ArrayIndex;



void fillarray(int* data, int *data1, unsigned int num)
{
    unsigned int i;
    for (i = 0; i < num; i++)
    {        
        data[i] = rand()/1000000;
        data1[i] = rand()/1000000;
    }
}

void merge (int *a, int n, int m) 
{
    int i, j, k;
    int *x = malloc(n * sizeof (int));
    for (i = 0, j = m, k = 0; k < n; k++) 
    {
        x[k] = j == n ? a[i++] : i == m ? a[j++] : a[j] < a[i] ? a[j++] : a[i++];
    }
    for (i = 0; i < n; i++) 
    {
       a[i] = x[i];
    }
    free(x);
}
void merge1(int *a, int low, int high, int mid)
{
    int i, j, k;
    int *c = malloc((high-low)*sizeof(int));
    i = low;
    k = 0;
    j = mid + 1;
    while (i <= mid && j <= high)
    {
        if (a[i] < a[j])
        {
            c[k] = a[i];
            k++;
            i++;
        }
        else
        {
            c[k] = a[j];
            k++;
            j++;
        }
    }
    while (i <= mid)
    {
        c[k] = a[i];
        k++;
        i++;
    }
    while (j <= high)
    {
        c[k] = a[j];
        k++;
        j++;
    }
    for (i = 0; i < k; i++)
    {
        a[i+low] = c[i];
    }
}
void merge_sort (int *a, int n)
{
    if (n < 2)
        return;
    int m = n / 2;
    merge_sort(a, m);
    merge_sort(a + m, n - m);
    merge(a, n, m);
}

void *pms(void *a)
{
        ArrayIndex *pa = (ArrayIndex *)a;
        merge_sort(values + pa->low,(pa->high-pa->low)+1);
        return 0;
}

void check()
{
    unsigned int i;
    for (i = 0; i <= N-2; i++)
        if (values[i] > values[i+1]) {
            printf("ERROR %d %d %d", i, values[i], values[i+1]);
            return;
        }
    printf("CORRECT\n");
}

void *pair_merge(void *a)
{
    ArrayIndex *pa = (ArrayIndex *)a;
    merge1(values, pa->low, pa->high - 1, (pa->high+pa->low)/2 -1);
}
void merge_pairs()
{   
    int thread_count = THREADS;
    ArrayIndex ai[thread_count];
    while(thread_count>=2)
    {   //Logic for parallel merge of 2 successive sorted blocks
        for (int i = 1; i <= thread_count/2; ++i)
        {   
            ai[i-1].low = (i-1)*(2*N)/thread_count;
            ai[i-1].high = ai[i-1].low + (N*2)/thread_count;
            pthread_create(&thread_handles[i-1], NULL, pair_merge, &ai[i-1]);

        }
        for (int i = 0; i < log(thread_count)/log(2); ++i)
        {
            pthread_join(thread_handles[i], NULL);
        }
        //Once done we repeat the merge again but this time with bigger merged blocks
        thread_count = thread_count/2;
    }
}

int main()
{   
    FILE *fp;
    fp = fopen("Results.txt","a");
    int thread;
    int thread_count = THREADS;
    ArrayIndex ai[thread_count];
    thread_handles = malloc(THREADS * sizeof(pthread_t));
    int i=0;
    for(i =0; i <thread_count; i++)
    {
        ai[i].low = i * (N / thread_count);
        ai[i].high = (i + 1) * (N / thread_count) - 1;
    }
    time_t t1, t2;
    double dt, dt1; //t2-t1
    //fill in the input array with random numbers
    fillarray(values, values1, N);
    t1 = clock();
    //Merging seperate blocks of array in parallel
    for(thread=0; thread<thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, pms, &ai[thread]);
    }
    //Waiting for threads to be completed
    for(thread=0; thread<thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    
    //Merging alternate pairs of sorted blocks
    merge_pairs();
    t2 = clock();
    dt = (double)(t2 - t1)/CLOCKS_PER_SEC;
    //check for correctness
    check();
    t1 = clock();
    merge_sort(values1, N);
    t2 = clock();
    dt1 = (double)(t2 - t1);
    fprintf(fp, "Time taken for %d elements is:\n", N);
    fprintf(fp,"Parallel MS with %d threads: %lf ticks\n",THREADS, dt);
    fprintf(fp,"Sequential MS: %lf ticks\n",dt1);   
    return 0;
}
