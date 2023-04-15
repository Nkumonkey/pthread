#include <arm_neon.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;
struct threadParam_t{
    int t_id;//线程id
};
sem_t sem_Divsion;
sem_t sem_Elimination;
const int NUM_THREADS=8;
float **A;
int n=50;
void* threadFunc(void* param)
{
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;
 int i, j, k;
    for (k = 0;k < n;++k)
    {
        if(t_id==0)
        { 
           for (j = k + 1; j % 4 != 0 && j < n; j++)
            A[k][j] = A[k][j] / A[k][k];
        for (; j <= n - 4; j += 4)
        {
            float32x4_t temp;
            float32x4_t t = vmovq_n_f32(A[k][k]);
            temp = vld1q_f32(A[k] + j);
            temp = vdivq_f32(temp, t);
            vst1q_f32(A[k] + j, temp);
        }
        for (; j < n; j++)
            A[k][j] = A[k][j] / A[k][k];
        A[k][k] = 1.0;
        }
        else {
            sem_wait(&sem_Divsion);
        }
        if(t_id==0)
        {
            for(int t_id=0;t_id<NUM_THREADS-1;++t_id)sem_post(&sem_Divsion);
        }
        for (i = k + 1 + t_id;i < n;i += NUM_THREADS)
        {
             for (j = k + 1; j % 4 != 0 && j < n; j++)
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            for (; j <= n - 4; j += 4)
            {
                float32x4_t t1;
                float32x4_t t2;
                t1 = vld1q_f32(A[i] + j);
                t2 = vld1q_f32(A[k] + j);
                float32x4_t t3 = vmovq_n_f32(A[i][k]);
                t3 = vmulq_f32(t3, t2);
                t1 = vsubq_f32(t1, t3);
                vst1q_f32(A[i] + j, t1);
            }
            for (; j < n; j++)
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            A[i][k] = 0;
        }
        if (t_id == 0)
        {
            for (int t_id = 0;t_id < NUM_THREADS - 1;++t_id)sem_post(&sem_Elimination);
        }
        else {
            sem_wait(&sem_Elimination);
        }
    }
    pthread_exit(NULL);
}

void normal(float** a)
{
   
    int i, j, k;
    for (k = 0; k < n; k++)
    {
        for (j = k + 1; j < n; j++)
        {
            a[k][j] = a[k][j] / a[k][k];
        }
        a[k][k] = 1.0;
        for (i = k + 1; i < n; i++)
        {
            for (j = k + 1; j < n; j++)
            {
                a[i][j] = a[i][j] - a[i][k] * a[k][j];
            }
            a[i][k] = 0;
        }
    }
}

void m_rest(float** A, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
            A[i][j] = 0;
        A[i][i] = 1.0;
        for (int j = i + 1; j < n; j++)
        {
            float x = rand();
            while (x == 0) x = rand();
            A[i][j] = x;
        }
    }
    for (int k = 0; k < n; k++)
        for (int i = k + 1; i < n; i++)
            for (int j = 0; j < i; j++)
                A[i][j] += A[k][j];
}

int main()
{
	while(n<=1600)
{
     srand((unsigned)time(NULL));
             A = new float*[n];
            float**B = new float*[n];
           struct timeval tstart, tend;
             double timeUsed0=0.0, timeUsed1 = 0.0;
             for (int i = 0; i < n; i++)A[i] =  new float[n];
             for (int i = 0; i < n; i++)B[i] =  new float[n];
             m_rest(A, n);
            m_rest(B, n);
            gettimeofday(&tstart, NULL);
             normal(B);
             gettimeofday(&tend, NULL);
             timeUsed0 = 1000000 * (tend.tv_sec - tstart.tv_sec) + tend.tv_usec - tstart.tv_usec;
        gettimeofday(&tstart, NULL);
    sem_init(&sem_Divsion, 0, 0);
    sem_init(&sem_Elimination, 0, 0);

    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];
    for (int t_id = 0;t_id < NUM_THREADS;t_id++)
    {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
    }
    for (int t_id = 0;t_id < NUM_THREADS;t_id++)pthread_join(handles[t_id],NULL);

    sem_destroy(&sem_Divsion);
    sem_destroy(&sem_Elimination);
    gettimeofday(&tend, NULL);
    timeUsed1 = 1000000 * (tend.tv_sec - tstart.tv_sec) + tend.tv_usec - tstart.tv_usec;
    cout <<timeUsed0<<" "<<timeUsed1<<" "<< timeUsed0 / timeUsed1 * 100 << "%" << endl;
n*=2;
}
    return 0;
}