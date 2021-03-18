/**
 * This program multiplies a matrix aa times a vector b to produce a vector c using MPI.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mat.h"

#define min(x, y) ((x)<(y)?(x):(y))

FILE *fptr;

int main(int argc, char* argv[])
{
    int nrows, ncols;

    double *aa, *bb, *cc;

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
	fptr = fopen("mmult_mpi_data.txt","w");
	fprintf(fptr,"#matrix size\tdelta time\n");

	for(int round = 200; round <=2000; round+=200){

        nrows = round;
        ncols = nrows;

	char size[8];
        char file1[100] = "matrices/a/size_";
        char file2[100] = "matrices/b/size_";

        sprintf(size, "%d", round);

        strcat(file1, size);
        strcat(file1, ".txt");

        strcat(file2, size);
        strcat(file2, ".txt");

        aa = read_matrix_from_file(file1);
        bb = read_matrix_from_file(file2);

        cc = (double*)malloc(sizeof(double) * nrows * ncols);
        buffer = (double*)malloc(sizeof(double) * nrows );
	ans = (double*)malloc(sizeof(double) * nrows );


        master = 0;

        if (myid == master) {
            // Master Code goes here
            starttime = MPI_Wtime();

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
          endtime = MPI_Wtime();
	  fprintf(fptr,"%i\t %lf\n", nrows, (endtime - starttime));
	  //freeing memory
	  free(buffer);
	  free(ans);
	  free(aa);
	  free(bb);
	  free(cc);
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
    }
	} else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    fclose(fptr);
    MPI_Finalize();
    puts("finished");
    return 0;
}

