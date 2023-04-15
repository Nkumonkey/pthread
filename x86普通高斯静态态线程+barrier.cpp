#include<iostream>
#include<pthread.h>
#include <cstdlib>
#include <ctime>
using namespace std;
#define NUM_THREADS 10
typedef struct {
int t_id; // 线程 idma
 }threadParam_t;
 int n;
 double **A;
 pthread_barrier_t barrier_Divsion;
  pthread_barrier_t barrier_Elimination;
void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int t_id=p->t_id;
    for(int k = 0; k < n; ++k)
    {
        if (t_id == 0){
            for(int j = k+1; j < n; j++)
            A[k][j] = A[k][j] / A[k][k];
            A[k][k] = 1.0;
        }
      pthread_barrier_wait(&barrier_Divsion);
        for(int i=k+1+t_id; i < n; i += NUM_THREADS){
            for(int j = k + 1; j < n; ++j)
            A[i ][ j ] = A[i][j ] -A[i][k] * A[k][j ];
A[i ][ k]=0.0;
        }
       pthread_barrier_wait(&barrier_Elimination);
    }
    pthread_exit(NULL);
}
 int main(){
     cin>>n;
      srand(time(NULL));
      for(int i=0;i<n;++i)
        A=new double* [n];
     for(int i=0;i<n;++i)
        A[i]=new double[n]();
     for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A[i][j] = rand(); // 生成一个 0 到 9 的随机数
        }
    }
   pthread_barrier_init(&barrier_Divsion, NULL, NUM_THREADS);
   pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREADS);
    pthread_t handles[NUM_THREADS];
    threadParam_t param[NUM_THREADS];
    for(int t_id = 0; t_id < NUM_THREADS; t_id++)
    {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id],nullptr,threadFunc,&param[t_id]);
    }
    for(int t_id = 0; t_id < NUM_THREADS; t_id++){
       pthread_join(handles[t_id],nullptr);
       }
      pthread_barrier_destroy(&barrier_Divsion);
      pthread_barrier_destroy(&barrier_Elimination);
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
