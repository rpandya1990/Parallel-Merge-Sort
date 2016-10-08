/* C program for Merge Sort */
#include<stdlib.h>
#include<stdio.h>	
#include<time.h>

void merge (int *a, int n, int m) {
 int i, j, k;
 int *x = malloc(n * sizeof (int));
 for (i = 0, j = m, k = 0; k < n; k++) {
 x[k] = j == n ? a[i++]
 : i == m ? a[j++]
 : a[j] < a[i] ? a[j++]
 : a[i++];
 }
 for (i = 0; i < n; i++) {
 a[i] = x[i];
 }
 free(x);
}
void merge_sort (int *a, int n) {
 if (n < 2)
 return;
 int m = n / 2;
 merge_sort(a, m);
 merge_sort(a + m, n - m);
 merge(a, n, m);
}

void fillarray(int* data, unsigned int N)
{
    unsigned int i;
    for (i = 0; i < N; i++)
        data[i] = rand();
}

int main()
{
	FILE *fp;
	fp = fopen("Results.txt","a");
	clock_t start, end;
    double cpu_time_used;
    int N = 500000;
    int array[N];
    fillarray(array,N);
	start = clock();
	merge_sort(array, N);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	fprintf(fp,"Time taken by sequential merge sort for %d elements is: %f seconds\n", N, cpu_time_used);	
}
