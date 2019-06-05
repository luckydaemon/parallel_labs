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
#include <vector>
#include <ctime>
#include <bits/stdc++.h>
 using namespace std;


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
    // Looping for each element of the matrix
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            //  Copying into temporary matrix only those element
            //  which are not in given row and column
            if (row != p && col != q)
            {
                temp[i][j++] = matr[row][col];
                // Row is filled, so increase row index and
                // reset col index
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

int main(int argc, char *argv[])
{	
    int size;
    int **matrix;
    matrix = readMatrix(&size, argv[2]);
    //printMatrix(matrix, size);
    //cout<<endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<double>  time_array; 
    for (int i=0;i<stoi(argv[1]);i++)
    	time_array.push_back(0.0);
    int d=0;
    for (int i=0;i<stoi(argv[1]);i++)
    {	 auto start_of_i = std::chrono::high_resolution_clock::now();
         d=determinant (matrix, size);
         auto end_of_i = std::chrono::high_resolution_clock::now();
         time_array[i]=std::chrono::duration<double>(end_of_i-start_of_i).count();
	}
    for (int i = 0; i < size; i++) 
    	{
      		delete[] matrix[i];
   		}
    delete[] matrix;
    auto end = std::chrono::high_resolution_clock::now();
     double  time=std::chrono::duration<double>(end-start).count();;
    cout<<d<<endl;
    cout<<"sequential algorithm time :";
    cout<< time << endl;
    double ex_value=0;
    sort(time_array.begin(),time_array.end());
    for (int i=0;i<stoi(argv[1]);i++)
        ex_value+=(time_array[i]);
    ex_value= ex_value/stoi(argv[1]);
    cout<<"expected value "<< ex_value <<endl;
    double  variance=0;
    for (int i=0;i<stoi(argv[1]);i++)
    variance+=(time_array[i] - ex_value)*(time_array[i] - ex_value);
    cout<<"variance "<< variance/stoi(argv[1])<<endl;
    return 0;
}
