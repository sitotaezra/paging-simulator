# Paging-Simulator

This repository is part of the "Rescuing the Future of Memory with Data-Oblivious Algorithms" research project I am working on along side Professor Valeria Bertacco and Nicholas Wendt at the University of Michigan, Department of Computer Science and Engineering


## Usage
Input to the simulator is any complied version of a data oblivious algorithm. There are six already complied versions from the [VIP-Bench](https://bitbucket.org/vip-benchmarks/vip-bench/src/master/) repository namely Bubble-Sort, Flood-fill-On2, Distinctness, Knapsack, Image Filters and NMNIST-CNN

** Inputs for Bubble Sort and Distinctness modified with an array of 1000 random integers for testing purposes all the other algorithms were kept the same

Pagesize is a power of 2 and passed in as an argument to the simulator which can run in 3 differnet modes
*  lru - implemented based on the least recently used algorithm, simulates paging replacing the page which hasn't been used for the longest amount of time. This mode does not utilize the knowledge of future memory accesses.
*  orcl - our implementation of Oracle an optimal paging schedule utilizing the knowledge future page accesses.
*  orclpref - Oracle + Prefetech , extended version of oracle that fetches pages from remote memory ahead of time. 

### Sample
To run the simulator, for instance bubble-sort with page size of 4KB and mode orcl

```sh
./valgrindScipt.sh bubble-sort.do
./simulate.sh bubble-sort.do 4096 orclpref
```

##### Output
```sh
MODE : orclpref
Total No of Pages : 4288615
No of Unique Pages : 271
-------------------------------------------------------
5% - RAM CAPACITY = 14
Time : 414.553 ms
----------------------------------
Total Stall : 49.5993 ms
Total No of Replacements : 15040
Total Time : 364.954 ms
Total Time for Local Accesses : 64.1035 ms
Total Time for Remote Accesses : 150.45 ms
Total Time for Evicting Dirty Pages : 150.4 ms
----------------------------------
Total No of Replacements Prefetching : 45507
Total Time Prefetching : 910.23 ms
Total Time for Remote Accesses Prefetching : 455.16 ms
Total Time for Evicting Dirty Pages Prefetching : 455.07 ms

 
-------------------------------------------------------
25% - RAM CAPACITY = 68
Time : 64.3891 ms
----------------------------------
Total Stall : 0.00991 ms
Total No of Replacements : 0
Total Time : 64.3791 ms
Total Time for Local Accesses : 64.3291 ms
Total Time for Remote Accesses : 0.05 ms
Total Time for Evicting Dirty Pages : 0 ms
----------------------------------
Total No of Replacements Prefetching : 664
Total Time Prefetching : 13.91 ms
Total Time for Remote Accesses Prefetching : 7.27 ms
Total Time for Evicting Dirty Pages Prefetching : 6.64 ms

 
-------------------------------------------------------
50% - RAM CAPACITY = 136
Time : 64.3891 ms
----------------------------------
Total Stall : 0.00991 ms
Total No of Replacements : 0
Total Time : 64.3791 ms
Total Time for Local Accesses : 64.3291 ms
Total Time for Remote Accesses : 0.05 ms
Total Time for Evicting Dirty Pages : 0 ms
----------------------------------
Total No of Replacements Prefetching : 171
Total Time Prefetching : 4.73 ms
Total Time for Remote Accesses Prefetching : 3.02 ms
Total Time for Evicting Dirty Pages Prefetching : 1.71 ms

 
-------------------------------------------------------
75% - RAM CAPACITY = 204
Time : 64.3891 ms
----------------------------------
Total Stall : 0.00991 ms
Total No of Replacements : 0
Total Time : 64.3791 ms
Total Time for Local Accesses : 64.3291 ms
Total Time for Remote Accesses : 0.05 ms
Total Time for Evicting Dirty Pages : 0 ms
----------------------------------
Total No of Replacements Prefetching : 67
Total Time Prefetching : 3.33 ms
Total Time for Remote Accesses Prefetching : 2.66 ms
Total Time for Evicting Dirty Pages Prefetching : 0.67 ms

 
-------------------------------------------------------
100% - RAM CAPACITY = 271
Time : 64.3891 ms
----------------------------------
Total Stall : 0.00991 ms
Total No of Replacements : 0
Total Time : 64.3791 ms
Total Time for Local Accesses : 64.3291 ms
Total Time for Remote Accesses : 0.05 ms
Total Time for Evicting Dirty Pages : 0 ms
----------------------------------
Total No of Replacements Prefetching : 0
Total Time Prefetching : 2.66 ms
Total Time for Remote Accesses Prefetching : 2.66 ms
Total Time for Evicting Dirty Pages Prefetching : 0 ms

 

```

