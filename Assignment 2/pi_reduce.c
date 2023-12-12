#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MESSAGE_TAG 1
#define SEED 921
#define NUM_ITER 1000000000

int main(int argc, char* argv[])
{
    int count_within_circle = 0;
    double point_x, point_y, point_distance, estimated_pi;

    int rank, number_of_processes, mpi_provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &mpi_provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    
    srand(rank * SEED);
    
    double begin_time, end_time;
    begin_time = MPI_Wtime();

    for (int iteration = 0; iteration < NUM_ITER / number_of_processes; iteration++)
    {
        point_x = (double)rand() / RAND_MAX;
        point_y = (double)rand() / RAND_MAX;
        point_distance = sqrt((point_x * point_x) + (point_y * point_y));
        
        if (point_distance <= 1.0) count_within_circle++;
    }    

    long aggregate_count;
    MPI_Reduce(&count_within_circle, &aggregate_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        estimated_pi = ((double)aggregate_count / NUM_ITER) * 4.0;
        end_time = MPI_Wtime();
        printf("Estimated PI = %f, Execution Time = %f seconds\n", estimated_pi, end_time - begin_time);
    }

    MPI_Finalize();
    return 0;
}
