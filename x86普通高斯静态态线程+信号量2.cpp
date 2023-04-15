#include<iostream>
#include<pthread.h>
#include <cstdlib>
#include <ctime>
#include <semaphore.h>
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE2
#include <pmmintrin.h> //SSE3
#include <tmmintrin.h> //SSSE3
#include <smmintrin.h> //SSE4.1
#include <nmmintrin.h> //SSSE4.2
#include <immintrin.h> //AVX、AVX2
#include <avx512fintrin.h>
#include <avxintrin.h>
using namespace std;
#define NUM_THREADS 10
typedef struct {
int t_id; // 线程 idma
 }threadParam_t;
 int n;
 float **A;
 sem_t  sem_leader;
 sem_t sem_Divsion[NUM_THREADS-1];
 sem_t sem_Elimination[NUM_THREADS-1];
void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int t_id=p->t_id;
    for(int k = 0; k < n; ++k)
    {
        if (t_id == 0){
            __m128 t1 = _mm_set1_ps(A[k][k]);
            for (int j = k + 1; j + 4 <=n; j += 4) {
                __m128 t2 = _mm_loadu_ps(&A[k][j]);
                __m128 temp = _mm_div_ps(t2, t1);
                _mm_storeu_ps(&A[k][j], temp);
            }
            A[k][k]=1;
        }
        else sem_wait(&sem_Divsion[t_id-1]);
        if (t_id == 0){
            for(int i = 0; i < NUM_THREADS-1; ++i)
            sem_post(&sem_Divsion[i]);
        }
        for(int i=k+1+t_id; i < n; i += NUM_THREADS){
           __m128 first_reg = _mm_set1_ps(A[i][k]);
            for (int j = k + 1; j + 4 <n; j += 4) {
                __m128 t1_reg = _mm_loadu_ps(&A[k][j]);
                t1_reg = _mm_mul_ps(first_reg, t1_reg);
                __m128 t2_reg = _mm_loadu_ps(&A[i][j]);
                t2_reg = _mm_sub_ps(t2_reg, t1_reg);
                _mm_storeu_ps(&A[i][j], t2_reg);
            }
A[i ][ k]=0.0;
        }
        if (t_id == 0)
        {
            for(int i = 0; i < NUM_THREADS-1; ++i)
            sem_wait(&sem_leader);
            for(int i = 0; i < NUM_THREADS-1; ++i)
            sem_post(&sem_Elimination[i]); 
        }
        else {
            sem_post(&sem_leader);
            sem_wait(&sem_Elimination[t_id-1]);
        }
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
    sem_init(&sem_leader, 0, 0);
    for(int i = 0; i < NUM_THREADS-1; ++i){
        sem_init(&sem_Divsion[i], 0, 0);
        sem_init(&sem_Elimination[i], 0, 0);
    }
    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];
    for(int t_id = 0; t_id < NUM_THREADS; t_id++)
    {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id],nullptr,threadFunc,&param[t_id]);
    }
    for(int t_id = 0; t_id < NUM_THREADS; t_id++){
       pthread_join(handles[t_id],nullptr);
       if(t_id!=NUM_THREADS-1){
  sem_destroy(&sem_Divsion[t_id]);
     sem_destroy(&sem_Elimination[t_id]);
       }
    }
sem_destroy(&sem_leader);
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
