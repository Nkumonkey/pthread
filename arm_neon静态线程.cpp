#include<iostream>
#include<pthread.h>
#include <cstdlib>
#include <ctime>
#include <semaphore.h>
#include<arm_neon.h>
using namespace std;
#define NUM_THREADS 5
typedef struct {
int t_id; // 线程 idma
 }threadParam_t;
 int n;
 float **A;
 sem_t sem_main;
 sem_t sem_workerstart[NUM_THREADS];
 sem_t sem_workerend[NUM_THREADS];
void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int t_id=p->t_id;
    for(int k = 0; k < n; ++k){
    sem_wait(&sem_workerstart[t_id]);
  for(int i=k+1+t_id;i<n;i+=NUM_THREADS)
{int j;

  	                int off=((unsigned long int)(&A[i][k+1]))%16/4;
                        for(j=k+1;j<k+1+off&&j<n;j++)
                        {
                        A[i][j]-=A[i][k]*A[k][j];
                        }
                        for (; j <= n-4; j+=4)
                        {      float32x4_t test1=vld1q_f32((&A[i][j]));
                                float32x4_t temp1=vmovq_n_f32(A[i][k]);
                                float32x4_t temp2=vld1q_f32(&(A[k][j]));
                                temp1=vmulq_f32(temp1,temp2);
                                test1=vsubq_f32(test1,temp1);


                        }
                        for(;j<n;j++)
                                {
                                A[i][j]-=A[i][k]*A[k][j];
                                }
                        A[i][k] = 0;
A[i][k]=0;
}
   sem_post(&sem_main);
   sem_wait(&sem_workerend[t_id]);
    }
    pthread_exit(NULL);
}
 int main(){
     cin>>n;
      srand(time(NULL));
      for(int i=0;i<n;++i)
        A=new float* [n];
     for(int i=0;i<n;++i)
        A[i]=new float[n]();
     for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A[i][j] = rand(); // 生成一个 0 到 9 的随机数
        }
    }
    sem_init(&sem_main, 0, 0);
    for(int i = 0; i < NUM_THREADS; ++i)
    {
        sem_init(&sem_workerstart[i], 0, 0);
        sem_init(&sem_workerend[i], 0, 0);
    }
    pthread_t handles[NUM_THREADS];// 创建对应的 Handle
    threadParam_t param[NUM_THREADS];// 创建对应的线程数据结构
    for(int t_id = 0; t_id < NUM_THREADS; t_id++)
    {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id],nullptr,threadFunc,&param[t_id]);
    }
     for(int k=0;k<n;++k){
            for(int j=k+1;j<n;++j)
            A[k][j]=A[k][j]/A[k][k];
     A[k][k]=1;
     for(int t_id = 0; t_id < NUM_THREADS; ++t_id)
     sem_post(&sem_workerstart[t_id]);
     for(int t_id = 0; t_id < NUM_THREADS; ++t_id)
     sem_wait(&sem_main);
     for(int t_id = 0; t_id < NUM_THREADS; ++t_id)
     sem_post(&sem_workerend[t_id]);
     }
     for(int t_id = 0; t_id < NUM_THREADS; t_id++){
     pthread_join(handles[t_id],nullptr);
     sem_destroy(&sem_workerstart[t_id]);
     sem_destroy(&sem_workerend[t_id]);
     }
      sem_destroy(&sem_main);
      for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
 }