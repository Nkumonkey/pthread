#include<iostream>
#include<pthread.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
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
typedef struct {
 int k; //消去的轮次
int t_id; // 线程 idma
 }threadParam_t;
 int n;
 float **A;
void* threadFunc(void* param){
    threadParam_t* p=(threadParam_t*)param;
    int k=p->k;
    int t_id=p->t_id;
    int i=k+t_id+1;
    __m128 first_reg = _mm_set1_ps(A[i][k]);
            for (int j = k + 1; j + 4 < size; j += 4) {
                __m128 t1_reg = _mm_loadu_ps(&A[k][j]);
                t1_reg = _mm_mul_ps(first_reg, t1_reg);
                __m128 t2_reg = _mm_loadu_ps(&A[i][j]);
                t2_reg = _mm_sub_ps(t2_reg, t1_reg);
                _mm_storeu_ps(&A[i][j], t2_reg);
            }
    A[i][k]=0;
    pthread_exit(NULL);
}
 int main(){
      struct timeval start, end;

    double interval;
     cin>>n;
      srand(time(NULL));
      for(int i=0;i<n;++i)
        A=new float* [n];
     for(int i=0;i<n;++i)
        A[i]=new float[n]();
           gettimeofday(&start, NULL);
     for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            A[i][j] = rand(); // 生成一个 0 到 9 的随机数
        }
    }
     for(int k=0;k<n;++k){
            for(int j=k+1;j<n;++j)
            A[k][j]=A[k][j]/A[k][k];
     A[k][k]=1;
     int worker_count= n-1-k; //工作线程数量
     pthread_t* handles;
      handles=(pthread_t*) malloc(worker_count*sizeof(pthread_t));// 创建对应的 Handle
     threadParam_t* param =(threadParam_t* ) malloc(worker_count*sizeof(threadParam_t));// 创建对应的线程数据结构
     for(int t_id = 0; t_id < worker_count; t_id++)
     {
         param[t_id].k = k;
         param[t_id].t_id = t_id;
     }
     for(int t_id = 0; t_id < worker_count; t_id++)
        pthread_create(&handles[t_id],nullptr,threadFunc,&param[t_id]);
     for(int t_id = 0; t_id < worker_count; t_id++)
        pthread_join(handles[t_id],nullptr);
     }
       gettimeofday(&end, NULL);
      interval = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
      for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    cout<<interval<<endl;
    return 0;
 }
