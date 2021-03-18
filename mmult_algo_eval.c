#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "mat.h"
#include <string.h>

struct timespec start;
struct timespec stop;
FILE *fptr;


void openFile(int choice);
void doMMult(double *c_calc, double *a, double *b, int i, int algo);


int main(int argc, char **argv) {
	if(argc <= 1){
		puts("Missing command line argument to send to correct mmult algorithm");
		exit(0);
	}

	//getting command line argument (which is choice of matrix multiplication algorithm
	char* algo = argv[1];
	int choice;
	if(strcmp("mmult", algo) == 0){
		choice = 1;
	} else if (strcmp("mmult_simd", algo) == 0){
		choice = 2;
	} else if(strcmp("mmult_omp", algo) == 0){
		choice = 3;
	} else {
		puts("Invalid matrix multiplication option");
		exit(0);
	}

	//sending to function to open proper file
	openFile(choice);

	//automation program
	fprintf(fptr,"#matrix size\tdelta time\n");
	for(int i=200; i<=2000; i+=200){
		char size[8];
                char file1[100] = "matrices/a/size_";
                char file2[100] = "matrices/b/size_";

                sprintf(size, "%d", i);

                strcat(file1, size);
                strcat(file1, ".txt");

                strcat(file2, size);
                strcat(file2, ".txt");

		double *a = read_matrix_from_file(file1);
		double *b = read_matrix_from_file(file2);
		double *c_calc = malloc(i * i* sizeof(double));
		//get start time and hold in variable
		clock_gettime( CLOCK_REALTIME, &start);
		//send to matrix multiplication function
		doMMult(c_calc, a, b, i, choice);
		//get end time and hold in variable
		clock_gettime( CLOCK_REALTIME, &stop);
		//send times to deltaTime function and hold in variable
		double delta = deltaTime(&start, &stop);
		//write data to file
		fprintf(fptr,"%i\t %lf\n", i, delta);
		//free memory
		free(a);
		free(b);
		free(c_calc);
	}
	//close file
	fclose(fptr);
	puts("Finished!!");
}


	void openFile(int choice){
	switch(choice){
                case 1:
			fptr = fopen("mmult_data.txt","w");
			break;
                case 2:
                        fptr = fopen("mmult_simd_data.txt","w");
			break;
		case 3:
			fptr = fopen("mmult_omp_data.txt","w");
			break;
	        default:
                        puts("invalid option");
                        break;

                }
	if(fptr == NULL) {
      		printf("Error!");
      		exit(1);
   		}
	}


	void doMMult(double *c_calc, double *a, double *b, int i, int algo){
	switch(algo){
		case 1:
			mmult(c_calc, a, i, i, b, i, i);
			break;
		case 2:
			mmult_simd(c_calc, a, i, i, b, i, i);
			break;
		case 3:
			mmult_omp(c_calc, a, i, i, b, i, i);
			break;
		default:
			puts("invalid option");
			break;

		}
	}
