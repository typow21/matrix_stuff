set terminal png size 800, 600
set output 'out.png'

set xlabel 'Matrix size'
set ylabel 'Time (sec)'
set title 'Matrix multiplication speed on Wolfgang Cluster'
plot "mmult_data.txt" using 1:2 title "mmult" with linespoint, "mmult_simd_data.txt" using 1:2 title "mmult SIMD" with linespoint, "mmult_simd_O3_data.txt" using 1:2 title "mmult SIMD 03" with linespoint, "mmult_omp_data.txt" using 1:2 title "mmult OMP" with linespoint, "mmult_mpi_data.txt" using 1:2 title "mmult MPI" with linespoint, "java/mmult_parallel_java.txt" using 1:2 title "java parallel" with linespoint, "mmult_mpi_omp_data.txt" using 1:2 title "mpi and omp" with linespoint
