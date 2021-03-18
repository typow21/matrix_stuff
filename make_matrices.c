//program used to make matrices of different sizes and write them to files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mat.h"
FILE *fptr1;
FILE *fptr2;



	int main(void){
	for(int i = 200; i <= 2000; i+=200){
		int nrows = i;
		int ncols = nrows;

		double *a = gen_matrix(nrows, ncols);
		double *b = gen_matrix(nrows, ncols);

		char size[8];
		char file1[100] = "matrices/a/size_";
		char file2[100] = "matrices/b/size_";

		sprintf(size, "%d", nrows);

		strcat(file1, size);
		strcat(file1, ".txt");

		strcat(file2, size);
                strcat(file2, ".txt");

		fptr1 = fopen(file1, "w");
		fprintf(fptr1,"%d\t %ld\n", nrows, ncols);

		for(int j = 0; j < nrows; j++) {
			for(int k = 0; k < ncols; k++) {
				fprintf(fptr1, "%5lf ", a[ncols * j + k]);
			}
			fprintf(fptr1, "\n");
		}


		fptr2 = fopen(file2, "w");
		fprintf(fptr2,"%d\t %ld\n", nrows, ncols);

		for(int j = 0; j < nrows; j++) {
                        for(int k = 0; k < ncols; k++) {
                                fprintf(fptr2, "%5lf ", b[ncols * j + k]);
                        }
                        fprintf(fptr2, "\n");
                }
		free(a);
		free(b);

	}
	fclose(fptr1);
	fclose(fptr2);
	return 0;
	}
