#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI_TAG 1
#define SEED 921
#define NUM_ITER 1000000000

int main(int argc, char* argv[])
{
    int count_in_circle = 0;
    double coord_x, coord_y, radius, pi_estimate;

    int rank, num_processes, thread_support;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &thread_support);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    
    srand(rank * SEED);
    
    double start_time = MPI_Wtime(), end_time;

    for (int i = 0; i < NUM_ITER / num_processes; i++)
    {
        coord_x = (double)rand() / RAND_MAX;
        coord_y = (double)rand() / RAND_MAX;
        radius = sqrt(coord_x * coord_x + coord_y * coord_y);
        
        if (radius <= 1.0) count_in_circle++;
    }    

    int depth = 0;
    while(num_processes >>= 1) {
        int remainder = rank % (2 << depth);

        if(remainder)
            MPI_Send(&count_in_circle, 1, MPI_INT, rank - remainder, PI_TAG, MPI_COMM_WORLD);
        else {
            int received_count, sender_id = rank + (1 << depth);
            MPI_Recv(&received_count, 1, MPI_INT, sender_id, PI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count_in_circle += received_count;
        }
        depth++;
    }

    end_time = MPI_Wtime();
    
    if(rank == 0) {
        pi_estimate = 4.0 * count_in_circle / NUM_ITER;
        printf("Estimated PI = %f, Time = %f seconds\n", pi_estimate, end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
