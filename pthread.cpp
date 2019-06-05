#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <pthread.h>
#include <vector>
#include <bits/stdc++.h>
 using namespace std;


typedef struct
{
    int **matr;
    int matrSize;
} thread_params;
pthread_mutex_t nextColumnMutex;
int nextColumn;
int** readMatrix(int *size, string filename)
{

    ifstream file;
    file.open(filename);
    int matrix_size;
    int **smatrix;
    if (file.is_open())
        { 
            file >> matrix_size;
            smatrix= new int*[matrix_size];
            for (int i = 0; i <matrix_size; i++) 
                {
                    smatrix[i] = new int[matrix_size];
                }
            for (int i = 0; i < matrix_size; i++)
                for (int j = 0; j <matrix_size; j++)
                    file >> smatrix[i][j];
        }
    file.close();
    *size=matrix_size;
    return smatrix;
}

void printMatrix(int **matr, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            cout<< matr[i][j] << " ";
        cout<<endl;
    }
}
void getCofactor(int **matr, int **temp, int p, int q, int n)
{
    int i = 0, j = 0;
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (row != p && col != q)
            {
                temp[i][j++] = matr[row][col];
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}



int determinant (int **matr, int size)
{   
    int D=0;
    if (size==1)
        return matr[0][0];
    int **temp;
    temp= new int*[size];
    for (int i = 0; i <size; i++) 
        {
            temp[i] = new int[size];
        }
    int sign = 1;
    for (int f=0; f< size;f++)
        {
            getCofactor(matr, temp, 0, f, size);
            D += sign * matr[0][f] * determinant(temp, size-1);
            sign = -sign;
        }
    for (int i = 0; i < size; i++) 
    {
        delete[] temp[i];
    }
    delete[] temp;
    return D;
}

void* func1(void *arg)
{
    int det = 0;
    thread_params *param = (thread_params*) arg;
    int col;
    while (1)
    {
        pthread_mutex_lock(&nextColumnMutex);
        col = nextColumn;
        nextColumn++;
        pthread_mutex_unlock(&nextColumnMutex);
        if (col >= param->matrSize)
            break;
        int **temp;
         temp= new int*[param->matrSize-1];
        for (int i = 0; i <param->matrSize-1; i++) 
        {
            temp[i] = new int[param->matrSize-1];
        }
        for (int i = 0; i <param->matrSize-1; i++) 
            for (int j = 0; j <param->matrSize-1; j++) 
                temp[i][j]=0;
        getCofactor(param->matr, temp, 0, col, param->matrSize);
        det += pow(-1.0, col) * param->matr[0][col] *determinant(temp, param->matrSize-1);
        for (int i = 0; i < param->matrSize-1; i++) 
        {
        delete[] temp[i];
        }
        delete[] temp;
    }
    return (void*) det;
}

int main(int argc, char *argv[])
{   int size;
    int d=0;
    int thread_count =0;
    for (thread_count=1;thread_count<=4; thread_count++)
    {   
        int **matrix;
        matrix=readMatrix(&size, argv[2]);
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<double>  time_array; 
        for (int i=0;i<stoi(argv[1]);i++)
            time_array.push_back(0.0);
        for (int i=0;i<stoi(argv[1]);i++)
        {
        auto start_of_i = std::chrono::high_resolution_clock::now();
        pthread_t thread[thread_count];
        nextColumn =0;
        pthread_mutex_init(&nextColumnMutex, NULL);
        thread_params param;
        param.matr=matrix;
        param.matrSize=size;
        d=0;
        for (int i=0; i < thread_count;i++)
            {
               pthread_create(&thread[i], NULL, &func1, &param);
            }
        for (int i=0; i < thread_count;i++)
            {
            int thrDet=0;
            pthread_join (thread[i], (void**) &thrDet);
            d+=thrDet;
            }
        auto end_of_i = std::chrono::high_resolution_clock::now(); 
        time_array[i]=std::chrono::duration<double>(end_of_i-start_of_i).count();
        }
        auto end = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; i++) 
        	{
          		delete[] matrix[i];
       		}
        delete[] matrix;
        double  time=std::chrono::duration<double>(end-start).count();;
        cout<<d<<endl;
        cout<<"parallel pthread algorithm time with "<<thread_count<<" threads"<<endl;
        cout<< time << endl;
        sort(time_array.begin(),time_array.end());
        double ex_value=0;
        for (int i=0;i<stoi(argv[1]);i++)
            ex_value+=(time_array[i]);
        ex_value= ex_value/stoi(argv[1]);
        cout<<"expected value "<< ex_value <<endl;
        double  variance=0;
        for (int i=0;i<stoi(argv[1]);i++)
            variance+=(time_array[i] - ex_value)*(time_array[i] - ex_value);
        cout<<"variance "<< variance/stoi(argv[1])<<endl;
    }
    return 0;
}

