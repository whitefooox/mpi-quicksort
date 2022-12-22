#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

void quicksort(int *array, int low, int high)
{
    int i = low;
    int j = high;
    int pivot = array[(i + j) / 2];
    int temp;
    while (i <= j)
    {
        while (array[i] < pivot)
            i++;
        while (array[j] > pivot)
            j--;
        if (i <= j)
        {
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
            i++;
            j--;
        }
    }
    if (j > low)
        quicksort(array, low, j);
    if (i < high)
        quicksort(array, i, high);
}

int *getRandomIntArray(int len, int a, int b)
{
    srand(time(0));
    int *arr = (int *)malloc((len) * sizeof(int));
    for (int i = 0; i < len; i++)
    {
        arr[i] = a + rand() % b;
    }
    return arr;
}

void printArray(int *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d  ", arr[i]);
    }
    printf("\n");
}

// функция для объединения двух массивов
int *merge(int *arr1, int n1, int *arr2, int n2)
{
    int *result = (int *)malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;

    for (k = 0; k < n1 + n2; k++)
    {
        if (i >= n1)
        {
            result[k] = arr2[j];
            j++;
        }
        else if (j >= n2)
        {
            result[k] = arr1[i];
            i++;
        }

        else if (arr1[i] < arr2[j])
        {
            result[k] = arr1[i];
            i++;
        }

        else
        {
            result[k] = arr2[j];
            j++;
        }
    }
    return result;
}

// основной код
int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "RUSSIAN");

    int len;
    int *data = NULL;
    int chunkSize, ownChunkSize;
    int *chunk;
    double time;
    MPI_Status status;

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        // получаем количество данных (размер массива)
        len = atoi(argv[1]);
        data = getRandomIntArray(len, 0, 100);
        // printArray(data, len);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // отправляем всем процессам размер массива
    MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // вычисляем размера подмассива
    chunkSize = (len % size == 0) ? (len / size) : (len / (size - 1));

    // создаем подмассив
    chunk = (int *)malloc(chunkSize * sizeof(int));

    // активируем таймер
    time -= MPI_Wtime();

    // распределяем массив по процессам
    MPI_Scatter(data, chunkSize, MPI_INT, chunk,
                chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    free(data);
    data = NULL;

    // вычисляем размер собственно подмассива
    ownChunkSize = (len >= chunkSize * (rank + 1)) ? chunkSize : (len - chunkSize * rank);

    // сортировка на каждом процессе
    quicksort(chunk, 0, ownChunkSize - 1);

    for (int step = 1; step < size; step = 2 * step)
    {
        if (rank % (2 * step) != 0)
        {
            MPI_Send(chunk, ownChunkSize, MPI_INT,
                     rank - step, 0,
                     MPI_COMM_WORLD);
            break;
        }
        if (rank + step < size)
        {
            int receivedChunkSize = (len >= chunkSize * (rank + 2 * step))
                                        ? (chunkSize * step)
                                        : (len - chunkSize * (rank + step));
            int *chunkReceived;
            chunkReceived = (int *)malloc(receivedChunkSize * sizeof(int));
            MPI_Recv(chunkReceived, receivedChunkSize,
                     MPI_INT, rank + step, 0,
                     MPI_COMM_WORLD, &status);
            data = merge(chunk, ownChunkSize,
                         chunkReceived,
                         receivedChunkSize);
            free(chunk);
            free(chunkReceived);
            chunk = data;
            ownChunkSize = ownChunkSize + receivedChunkSize;
        }
    }

    // останавливаем время
    time += MPI_Wtime();

    // выводим итоговую информацию
    if (rank == 0)
    {
        // printArray(chunk, len);
        printf("|%d|\t%d\t%f\n", size, len, time);
    }

    MPI_Finalize();
    return 0;
}