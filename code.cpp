#include <string.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;
struct Matrix
{
    int **matrix = nullptr;
    int Rows = 0;
    int Cols = 0;

    Matrix(int _rows, int _cols)
        : Rows(_rows), Cols(_cols)
    {
        //allocate matrix
        matrix = new int *[Rows];
        for (int i = 0; i < Rows; i++)
            matrix[i] = new int[Cols];
    }
};

struct ArrayParam
{
    int *array = nullptr;
    int startIndex;
    int endIndex;
    ArrayParam(int *_array, int _startIndex, int _endIndex)
    {
        array = _array;
        startIndex = _startIndex;
        endIndex = _endIndex;
    }
};
//from 1 to range
int getRandomValue(int range)
{
    return (rand() % range) + 1;
}
Matrix *getRandomMatrix(int Rows, int Cols)
{
    Matrix *matrix = new Matrix(Rows, Cols);
    srand((unsigned)time(0));
    int range = Rows + Cols; //arbitrary range for random values for matrix
    for (int i = 0; i < Rows; i++)
        for (int j = 0; j < Cols; j++)
            matrix->matrix[i][j] = getRandomValue(range);
    return matrix;
}
void print_matrix(Matrix *mat)
{
    if (!mat)
    {
        cout << "Matrix is null";
        exit(2);
    }
    printf("Rows = %d, Cols = %d\n", mat->Rows, mat->Cols);
    for (int i = 0; i < mat->Rows; i++)
    {
        for (int j = 0; j < mat->Cols; j++)
        {
            printf("%d\t", mat->matrix[i][j]);
        }
        cout << endl;
    }
}
void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
/*Places the pivot element at its correct place and places the smaller elements to its left
and larger ones to the right*/
int partition(int array[], int startIndex, int endIndex)
{
    int pivot = array[endIndex]; // It is the pivot element
    int i = (startIndex - 1);    //It is the index of the smaller element
    for (int j = startIndex; j <= endIndex - 1; j++)
    {
        if (array[j] <= pivot)
        {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[endIndex]);
    return (i + 1);
}

void print_array(ArrayParam *arr)
{
    cout << "StartIndex = " << arr->startIndex << "\t"
         << "EndIndex = " << arr->endIndex << endl;
    for (int i = arr->startIndex; i <= arr->endIndex; i++)
        cout << arr->array[i] << "\t";
    cout << endl;
}
void *quickSort(void *param)
{
    ArrayParam *arr = (ArrayParam *)param;

    if (arr->startIndex < arr->endIndex)
    {
        /*p is partitioning index*/
        int p = partition(arr->array, arr->startIndex, arr->endIndex);

        pthread_t id[2];
        if (pthread_create(&id[0], NULL, &quickSort, new ArrayParam(arr->array, arr->startIndex, p - 1)) == -1)
        {
            cout << "Thread Creation Failed!" << endl;
            return nullptr;
        }

        if (pthread_create(&id[1], NULL, &quickSort, new ArrayParam(arr->array, p + 1, arr->endIndex)) == -1)
        {
            cout << "Thread Creation Failed!" << endl;
            return nullptr;
        }

        for (int i = 0; i < 2; i++)
            pthread_join(id[i], NULL);
    }
}
void *ComputeRow(void *param)
{
    ArrayParam *arr = (ArrayParam *)param;
    quickSort(arr);
    //make sum
    int *sum = new int(0);
    for (int i = arr->startIndex; i <= arr->endIndex; i++)
        *sum += arr->array[i];
    pthread_exit(sum);
}
int main()
{
    int Rows,Cols;
    
    cin>>Rows;
    cin>>Cols;
    Matrix *matrix = getRandomMatrix(Rows, Cols);

    cout << "Input Matrix:\n";
    print_matrix(matrix);

    pthread_t id[Rows];
    for (int i = 0; i < Rows; i++)
    {
        if (pthread_create(&id[i], NULL, &ComputeRow, new ArrayParam(matrix->matrix[i], 0, Cols - 1)) == -1)
        {
            cout << "Thread Creation Failed!" << endl;
            return -1;
        }
    }
    int matrix_addition = 0;
    int *sum;
    for (int i = 0; i < Rows; i++)
    {
        pthread_join(id[i], (void **)&sum);
        matrix_addition += *sum;
    }

    cout << "Output Matrix:\n";
    print_matrix(matrix);

    cout << "Matrix Addition = " << matrix_addition << endl;
    exit(0);
}
//doneq2
