/**
 * This programdoes matrix multiplication on two input text files using mpi and verifies it works correctly
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mat.h"

#define min(x, y) ((x)<(y)?(x):(y))

int main(int argc, char* argv[])
{
    int nrows, ncols;

    double *aa, *bb, *cc,  *dd;

    double *buffer, *ans;

    double *times;

    double total_times;

    int run_index;

    int nruns;

    int myid, master, numprocs;

    double starttime, endtime;

    MPI_Status status;

    int i, j, numsent, sender;

    int anstype, row;

    srand(time(0));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (argc <= 1) {
        nrows = 5;
        ncols = nrows;
	aa = read_matrix_from_file("a.txt");
	bb = read_matrix_from_file("b.txt");
	dd = read_matrix_from_file("c.txt");
	cc = malloc(nrows * ncols * sizeof(double));
        buffer = (double*)malloc(sizeof(double) * nrows );
	ans = (double*)malloc(sizeof(double) * nrows );
        master = 0;

        if (myid == master) {
            // Master Code goes here
            numsent = 0;

            MPI_Bcast(bb, ncols*nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);


	   for (i = 0; i < min(numprocs-1, nrows); i++) {
                for(j = 0; j < ncols; j++){
                    buffer[j] = aa[i * ncols + j];
                }
		MPI_Send(buffer, nrows, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
		numsent++;
            }


            for (i = 0; i < nrows; i++) {
		MPI_Recv(ans, nrows, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                sender = status.MPI_SOURCE;
                anstype = status.MPI_TAG;

                for(j = 0; j<ncols; j++){
                    cc[(anstype-1)*nrows+j] = ans[j];
                }
                if (numsent < nrows) {
                    for (j = 0; j < ncols; j++) {
                        buffer[j] = aa[numsent*ncols + j];
                    }
                    MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent+1,
                        MPI_COMM_WORLD);
                    numsent++;
                } else {
                    MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, master, MPI_COMM_WORLD);
                }
            }
    	  mmult(dd, aa, nrows, ncols, bb, nrows, ncols);
	  puts("Testing MPI matrix multiplication");
    	  compare_matrices(cc,dd,nrows, ncols);
	  free(aa);
	  free(bb);
	  free(cc);
	  free(dd);
	  free(ans);
	  free(buffer);
        } else {
            // Slave Code goes here
            MPI_Bcast(bb, nrows*ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            if (myid <= nrows) {
                while(1) {
                    MPI_Recv(buffer, nrows, MPI_DOUBLE, master, MPI_ANY_TAG,
                        MPI_COMM_WORLD, &status);
                    if (status.MPI_TAG == 0){
                        break;
                    }
                    row = status.MPI_TAG;
                    for(j=0; j<ncols; j++){
                        ans[j] = 0.0;
                        for(int k=0; k<nrows; k++){
                            ans[j] += buffer[k] * bb[k * nrows + j];
                        }
                    }
                    MPI_Send(ans, ncols, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
                }
            }
        }
    } else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    MPI_Finalize();
    return 0;
}
