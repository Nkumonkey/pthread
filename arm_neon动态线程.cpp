#include<iostream>
#include<pthread.h>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include<arm_neon.h>
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
   int j;
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
        for (int j = 0; j < n; j++)
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