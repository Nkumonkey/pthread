#include <pthread.h>
#include <iostream>
#include <semaphore.h>
#include <stdio.h>
#include<stdlib.h>
#include <time.h>
#include <windows.h>
#include<bitset>
#include <regex>
#include <fstream>
#include <vector>
#include<sstream>
using namespace std;
long long head, tail, freq;

void timestart()
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
}
void timestop()
{
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);	// end time
    cout << ((tail - head) * 1000.0 / freq) / 1000 << endl;


}
bool done = false;
bool start = false;
pthread_barrier_t barrier;
pthread_barrier_t barrier2;
bool is_bx = 1;
const int row = 8399;
const int a2 = 6375;
const int a1 = 4535;
const int THREAD_NUM= 2;
pthread_mutex_t  mutex_task;
pthread_mutex_t  mutex_start;
string a2p = "E:\\并行程序设计相关\\data\\Groebner\\测试样例7 矩阵列数8399，非零消元子6375，被消元行4535\\消元子.txt";//消元子文件位置
string a1p = "E:\\并行程序设计相关\\data\\Groebner\\测试样例7 矩阵列数8399，非零消元子6375，被消元行4535\\被消元行.txt";//被消元子文件位置
string result = "E:\\并行程序设计相关\\data\\Groebner\\测试样例7 矩阵列数8399，非零消元子6375，被消元行4535\\消元结果.txt";//结果文件位置
bitset<row> x[row];
bitset<row>y[a1];
bitset<row>re[a1];
ifstream file;
int temp = 0;
sem_t sem_main;
sem_t sem_start;
bool is_x[a1]{};
//读入文件
void readfile()
{
    string line;
    regex pat_regex("[[:digit:]]+");
    file.open(a2p);
    int i = 0;
    int flag = 0;
    while (getline(file, line)) {  //按行读取
        int i = 0;
        for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it;
            it != end_it; ++it) {  //表达式匹配，匹配一行中所有满足条件的字符
            int a = stoi(it->str());  //将数据转化为int型并存入一维vector中
            if (i == 0)
            {
                i = 1;
                flag = a;
            }
            x[flag][a] = 1;
        }

    }
    i = 0;
    file.close();
    file.open(a1p);
    while (getline(file, line)) {  //按行读取
        for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it;
            it != end_it; ++it) {  //表达式匹配，匹配一行中所有满足条件的字符
            int a = stoi(it->str());  //将数据转化为int型并存入一维vector中
            y[i][a] = 1;
        }
        i++;
    }
    i = 0;
    file.close();
    file.open(result);
    i = 0;
    while (getline(file, line)) {  //按行读取
        for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it;
            it != end_it; ++it) {  //表达式匹配，匹配一行中所有满足条件的字符
            int a = stoi(it->str());  //将数据转化为int型并存入一维vector中
            re[i][a] = 1;
        }
        i++;
    }
    i = 0;
    file.close();
}
//获取首个非0位
int GETF(bitset<row> bs)
{
    for (int i = row - 1; i >= 0; i--)
    {
        if (bs[i] == 1)
        {
            return i;
        }
    }
    return -1;
}
//线程数据结构定义
typedef struct {
    int t_id; // 线程 id
}threadParam_t;
bool is_right()
{
    bool flag = true;
    for (int i = 0; i < a1; i++)
    {
       
        if (y[i] != re[i])
        {

            //cout << i << endl;
            //cout << y[i] << endl << re[i]<<endl;
            flag = false;
        }
    }
    return flag;
}
bool is_right_2()
{
    bool flag[a1]{};

    for (int i = 0; i < a1; i++)
    {
        bool f = false;
        for (int j = 0; j < a1; j++)
        {
            if (re[j] == y[i] && flag[j] == false)
            {
                flag[j] = true;
                f = true;
                break;
            }
        }
        if (f == false)
        {
            return false;
        }
    }
    return true;
}
void cx()
{
    for (int i = 0; i < a1; i++)
    {
        while (y[i] != 0)
        {
            if (x[GETF(y[i])] != 0)
            {
                y[i] = y[i] ^ x[GETF(y[i])];
            }
            else
            {
                x[GETF(y[i])] = y[i];
                break;
            }
        }
    }
}
void* threadFunc(void* param)
{
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;
    for (int i = row - 1; i >= 0; i--)
    {
        if (x[i] != 0)
        {
            for (int t = t_id; t < a1; t += THREAD_NUM)
            {
                if (GETF(y[t]) == i)
                {
                    y[t] = y[t] ^ x[i];
                }
            }
        }
        else
        {
            pthread_barrier_wait(&barrier);
            if (t_id == 0)
            {
                for (int t = 0; t < a1; t++)
                {
                    if (GETF(y[t])==i)
                    {
                        x[i] = y[t];
                        temp = t + 1;
                        break;
                    }
                    temp = t + 2;
                }
            }
            pthread_barrier_wait(&barrier2);
            //跳过一定不需要计算的部分，减少计算量
            int t = t_id;
            while (t < temp)
            {
                t += THREAD_NUM;
            }
            for (; t < a1; t += THREAD_NUM)
            {
                if (GETF(y[t]) == i)
                {
                    y[t] = y[t] ^ x[i];
                }
            }
        }
 }
    pthread_exit(NULL);
    return 0;
}
void bx()
{
    pthread_barrier_init(&barrier, NULL, THREAD_NUM);
    pthread_barrier_init(&barrier2, NULL, THREAD_NUM);
    pthread_t handles[THREAD_NUM];
    threadParam_t param[THREAD_NUM];
    for (int t_id = 0; t_id < THREAD_NUM; t_id++) {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
    }
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(handles[i], NULL);
}
int main()
{
    readfile();
    timestart();
    if (is_bx)
    {
        bx();
    }
    else {
        cx();
    }
    timestop();
    //cout<<is_right();
    
}