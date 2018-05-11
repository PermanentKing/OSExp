#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int M, K, N;
// the size of matrix
int A[100][100];
int B[100][100];
int C[100][100];
// structure for passing data to threads
struct v
{
    int i, j;
};
// calculate the matrix product in C[row][col]
void *calculate(void *data) {
    struct v *a = (struct v*)data;
    int i = a->i;
    int j = a->j;
    for (int k = 0; k < K; k++) {
        C[i][j] += A[i][k] * B[k][j];
    }
    pthread_exit(NULL);
}

int main() {
    printf("Please enter three numbers(M/K/N) that are less than 100:\n");
    scanf("%d%d%d", &M, &K, &N);
    printf("Please enter the first matrix(M*K):\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            scanf("%d", &A[i][j]);
        }
    }
    printf("Please enter the second matrix(K*N):\n");
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            scanf("%d", &B[i][j]);
        }
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }
    pthread_t tid[M * N];
    pthread_attr_t attr;
    // get the default attributes
    pthread_attr_init(&attr);
    // we have to create M*N pthreads
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            struct v *a = (struct v*)malloc(sizeof(struct v));
            a->i = i;
            a->j = j;
            pthread_create(&tid[i * N + j], &attr, calculate, (void*)a);
            //pthread_join(tid[i], NULL);
        }
    }
    // join upon each thread
    for (int i = 0; i < M * N; i++) {
        pthread_join(tid[i], NULL);
    }
    // output the result
    printf("The result(M*N) is:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", C[i][j]);
            if (j == N - 1){
                printf("\n");
            }
        }
    }
    return 0;
}