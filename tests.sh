#!/bin/sh

echo "==============================================================="
echo "///                    PERFORMANCE TEST                     ///"
echo "==============================================================="
echo " "
cpu=`cat /proc/cpuinfo | grep "model name" | cut -d':' -f2 | uniq | sed -E "s/^[ ]+//g" | sed -E "s/[ ]+/_/g"`
echo "CPU is $cpu"

gpu=`nvidia-smi -q | grep "Product Name" | cut -d ':' -f2 | sed -E "s/^[ ]+//g" | sed -E "s/[ ]+/_/g"`
echo "GPU is $gpu"
echo " "


problem="examples/2315_solutions.txt"

output_dir="results/${cpu}_${gpu}"
mkdir -p $output_dir

echo "---------------------------------------------------------------"
echo "Implementation           USER  ELAPSED SOLUTIONS"
echo "---------------------------------------------------------------"

/usr/bin/echo -n "CPU Sequential Recursive "
/usr/bin/time -f "time=%U  %e" build/bin/sudoku_cpu_recursive.exe -i $problem >${output_dir}/cpu_seq_rec.txt 2>&1
t=`cat ${output_dir}/cpu_seq_rec.txt | grep "time=" | cut -d'=' -f 2`
s=`cat ${output_dir}/cpu_seq_rec.txt | grep "number of solutions=" | cut -d'=' -f 2`
echo "$t      $s"


/usr/bin/echo -n "CPU Sequential Iterative "
/usr/bin/time -f "time=%U  %e" build/bin/sudoku_cpu_iterative.exe -i $problem >${output_dir}/cpu_seq_itr.txt 2>&1
t=`cat ${output_dir}/cpu_seq_itr.txt | grep "time=" | cut -d'=' -f 2`
s=`cat ${output_dir}/cpu_seq_itr.txt | grep "number of solutions=" | cut -d'=' -f 2`
echo "$t      $s"

/usr/bin/echo -n "CPU   Parallel Recursive "
/usr/bin/time -f "time=%U  %e" build/bin/sudoku_cpu_recursive_parallele.exe -i $problem -b 3 >${output_dir}/cpu_par_rec.txt 2>&1
t=`cat ${output_dir}/cpu_par_rec.txt | grep "time=" | cut -d'=' -f 2`
s=`cat ${output_dir}/cpu_par_rec.txt | grep "number of solutions=" | cut -d'=' -f 2`
echo "$t      $s"

/usr/bin/echo -n "CPU   Parallel Iterative "
/usr/bin/time -f "time=%U  %e" build/bin/sudoku_cpu_iterative_parallele.exe -i $problem -b 3 >${output_dir}/cpu_par_itr.txt 2>&1
t=`cat ${output_dir}/cpu_par_itr.txt | grep "time=" | cut -d'=' -f 2`
s=`cat ${output_dir}/cpu_par_itr.txt | grep "number of solutions=" | cut -d'=' -f 2`
echo "$t      $s"

/usr/bin/echo -n "GPU   Parallel Iterative "
/usr/bin/time -f "time=%U  %e" build/bin/sudoku_gpu_iterative_parallele.exe -i $problem -b 3 >${output_dir}/gpu_par_itr.txt 2>&1
t=`cat ${output_dir}/gpu_par_itr.txt | grep "time=" | cut -d'=' -f 2`
s=`cat ${output_dir}/gpu_par_itr.txt | grep "number of solutions=" | cut -d'=' -f 2`
echo "$t      $s"

echo "---------------------------------------------------------------"
echo "number of solutions expected is 2315"
echo " "
echo "results sent to directory:"
echo " "
echo "  $output_dir "
echo " "
