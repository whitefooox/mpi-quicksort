#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <stdlib.h>

int* getRandomIntArray(int len, int a, int b){
    srand(time(0));
    int* arr = (int *)malloc((len) * sizeof(int));
    for(int i = 0; i < len; i++){
        arr[i] = a + rand() % b;
    }
    return arr;
}

void printArray(int* arr, int len){
    for(int i = 0; i < len; i++){
        printf("%d  ", arr[i]);
    }
    printf("\n");
}

void quickSort(int *array, int low, int high)
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
        quickSort(array, low, j);
    if (i < high)
        quickSort(array, i, high);
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "Russian");
    int len = atoi(argv[1]);
    int* array = getRandomIntArray(len, 0, 100);
    //printArray(array, len);
    clock_t start = clock();
    quickSort(array, 0, len - 1);
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    //printArray(array, len);
    printf("|1|\t%d\t%f\n", len, seconds);
    free(array);
    return 0;
}