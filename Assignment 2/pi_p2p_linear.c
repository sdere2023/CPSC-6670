#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define SEED 921
#define NUM_ITER 1000000000
#define MESSAGE_TAG 1

int main(int argc, char* argv[])
{
    int rank, num_ranks, provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

    srand(SEED * rank); // Seed for random number generation

    int count = 0;
    double x, y, distance, pi_estimate;
    double start_time = MPI_Wtime(), end_time;

    // Monte Carlo simulation to estimate PI
    for (int i = 0; i < NUM_ITER / num_ranks; ++i) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        distance = sqrt(x * x + y * y);

        if (distance <= 1.0) {
            ++count;
        }
    }

    if (rank == 0) {
        int temp_count;
        long total_count = count;

        for (int source = 1; source < num_ranks; ++source) {
            MPI_Recv(&temp_count, 1, MPI_INT, source, MESSAGE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += temp_count;
        }

        pi_estimate = 4.0 * total_count / NUM_ITER;
        end_time = MPI_Wtime();
        printf("Calculated PI = %f, Time Elapsed = %f seconds\n", pi_estimate, end_time - start_time);
    } 
    
    else 
    {
        MPI_Send(&count, 1, MPI_INT, 0, MESSAGE_TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
