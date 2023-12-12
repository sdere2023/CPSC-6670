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
    int circle_hits = 0;
    double coord_x, coord_y, distance, calculated_pi;
    int rank, total_processes, iter, mpi_thread_support;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &mpi_thread_support);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

    srand(SEED * rank);

    double time_begin, time_end;
    time_begin = MPI_Wtime();

    int hit_counts[total_processes];

    for (iter = 0; iter < NUM_ITER / total_processes; iter++)
    {
        coord_x = (double)rand() / RAND_MAX;
        coord_y = (double)rand() / RAND_MAX;
        distance = sqrt(coord_x * coord_x + coord_y * coord_y);

        if (distance <= 1.0) {
            circle_hits++;
        }
    }

    MPI_Gather(&circle_hits, 1, MPI_INT, hit_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < total_processes; i++) {
            circle_hits += hit_counts[i];
        }

        calculated_pi = 4.0 * circle_hits / NUM_ITER;
        time_end = MPI_Wtime();
        printf("Calculated PI = %f, Execution Time = %f seconds\n", calculated_pi, time_end - time_begin);
    }

    MPI_Finalize();
    return 0;
}
