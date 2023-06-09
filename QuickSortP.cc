#include <iostream>
#include <time.h>
#include <math.h>
#include <algorithm>
//#undef SEEK_SET
//#undef SEEK_CUR
//#undef SEEK_END
#include <mpi.h>
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

// Funcion recursiva que llama a la funcion particion y luego se llama asi misma con los nuevos limites que genera el pivote
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int piv = partition(arr, low, high);  
        quickSort(arr, low, piv - 1);   
        quickSort(arr, piv + 1, high);  
    }
}

void quickSortP(int arr[], int size){
    int id, nproc;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    if(size > 1){
        int piv = partition(arr, 0, size-1);
        if (nproc >= 2*id+2){
            int cont = 0;
            int* arrIzq = new int[piv];;
            int* arrDer = new int[size-piv];;

            for(int i = 0; i < piv; i++){
                arrIzq[i] = arr[cont];
                cont = cont + 1;
            }
            
            for(int i = 0; i < size-piv; i++){
                arrDer[i] = arr[cont];
                cont = cont + 1;
            }

            if(nproc >= 2*id+3){
                //Envia los tamaños de los arrays
                int sizeIzq = piv;
                int sizeDer = size-piv;
                MPI_Send(&sizeIzq, 1, MPI_INT, 2*id+1, 0, MPI_COMM_WORLD);
                MPI_Send(&sizeDer, 1, MPI_INT, 2*id+2, 0, MPI_COMM_WORLD);

                //Envia los arrays
                MPI_Send(arrIzq, sizeIzq, MPI_INT, 2*id+1, 1, MPI_COMM_WORLD);
                MPI_Send(arrDer, sizeDer, MPI_INT, 2*id+2, 1, MPI_COMM_WORLD);

                //Recibe lor arrays
                MPI_Recv(arrIzq, sizeIzq, MPI_INT, 2*id+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(arrDer, sizeDer, MPI_INT, 2*id+2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
            {
                MPI_Send(&piv, 1, MPI_INT, 2*id+1, 0, MPI_COMM_WORLD);
                MPI_Send(arrIzq, piv, MPI_INT, 2*id+1, 1, MPI_COMM_WORLD);
                quickSort(arrDer, 0, size-piv-1);
                MPI_Recv(arrIzq, piv, MPI_INT, 2*id+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            cont = 0;
            for(int i = 0; i < piv; i++){
                arr[cont] = arrIzq[i];
                cont = cont + 1;
            }
            for(int i = 0; i < size-piv; i++){
                arr[cont] = arrDer[i];
                cont = cont + 1;
            }
            delete[] arrIzq;
            delete[] arrDer;
        }
        else
        {
            quickSort(arr, 0, size-1);
        }
    }
}

int main(int argc, char** argv) {
    int id, nproc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    double start;
    if(id == 0)
    {
        start = MPI_Wtime();
    }
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
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
    if (id == 0)
    {
        for (int i = 0; i < size; i++) {
            arr[i]=rand()%1001;
        }
        quickSortP(arr, size);
        delete[] arr;
        cout << "largo de la lista: " << size << "\n";
        
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else
    {
        if(id%2 == 0)
        {
            MPI_Recv(&size, 1, MPI_INT, (id/2)-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(arr, size, MPI_INT, (id/2)-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            quickSortP(arr, size);
            MPI_Send(arr, size, MPI_INT, (id/2)-1, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Recv(&size, 1, MPI_INT, (id-1)/2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(arr, size, MPI_INT, (id-1)/2, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            quickSortP(arr, size);
            MPI_Send(arr, size, MPI_INT, (id-1)/2, 0, MPI_COMM_WORLD);
        } 

        MPI_Barrier(MPI_COMM_WORLD);
    }

    if(id == 0)
    {
        double end = MPI_Wtime();
        double tiempo_ejec = end - start;
        cout << "tiempo de ejecucion: " << fixed << tiempo_ejec << " seg.\n";
    }
    MPI_Finalize();
    return 0;
}
