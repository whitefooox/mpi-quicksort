gcc quicksort.c -o ./quicksort.out
mpicc quicksort-mpi.c -o quicksort-mpi.out
echo -n > ./results.txt
for((n = 10; n < 1000000000; n *= 10))
do
    ./quicksort.out $n >> ./results.txt
    mpiexec -n 2 ./quicksort-mpi.out $n >> ./results.txt
    mpiexec -n 4 ./quicksort-mpi.out $n >> ./results.txt
    mpiexec -n 8 ./quicksort-mpi.out $n >> ./results.txt
done

rm ./quicksort.out
rm ./quicksort-mpi.out