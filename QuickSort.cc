#include <iostream>
#include <time.h>
#include <math.h>
using namespace std;

// Funcion que intercamvia 2 elementos
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Funcion que ordena el arra segun un pivote
int partition(int arr[], int low, int high) {
    int pivv = arr[high];  // El ultimo elemento es el pivote
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivv) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Funcion recursiva que llama a la funcion particion y luego se llama asi misma con los nuevos limites que generl e pivote
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int piv = partition(arr, low, high);  
        quickSort(arr, low, piv - 1);   
        quickSort(arr, piv + 1, high);  
    }
}


int main(int argc, char** argv) {

    clock_t start = clock();
    int intVariable = atoi(argv[1]), size;
    if (intVariable <= 6)
    {
        size=pow(10,intVariable);
    }
    else
    {
        size=pow(10,6)*(intVariable-5);
    }
    int* arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i]=rand()%1001;
    }
    quickSort(arr, 0, size - 1);

    delete[] arr;
    cout << "largo de la lista: " << size << "\n";

    clock_t end = clock();
    double tiempo_ejec = double(end - start)/CLOCKS_PER_SEC;

    cout << "tiempo de ejecucion: " << fixed << tiempo_ejec << " seg.\n";
    return 0;
}

