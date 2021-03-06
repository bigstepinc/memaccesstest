#Memaccesstest

*Memory Access Time Benchmark - Test L1, L2,L3, Main memory access times, TLB misses and prefetch effects. - Alfa* 

Version 0.3


This application is a benchmark designed to test the performance of various systems while accessing memory. It outputs data as CSVs so it cannot be used stand-alone, an analysis tool will be required outside of it. It also requires recompilation on the different target machines that are tested and with various settings. 

Provided is a matlab/octave source file to interpret the data.
To compile the code run:
```
make
``` 
To execute and record the data run:
```
./memaccesstest > output.csv
```
To interpret the data using **octave**:
```
./octave --eval "plot_csv_log('output.csv')"
```


It measures the time it takes to traverse and/or do additional operations on data stored in a linked list (bi-directional). It does so while gradually increasing the working set size (the total capacity of the list). As it does so, it will eventually start increasing the miss ratios for the L1,L2 and L3 caches with obvious effects on time it takes to access that particular memory. 

The default record size is 64 bytes. To change this modify NPAD constant to a different number.
```
struct l{
	struct l *n;
	struct l *p;
	long int pad[NPAD-1];

};
```



The test can also do random access vs sequencial (essencially invalidating the prefetch mechanism) and multiple threads in parallel. To change these settings one must edit the config.h file. 
 
A plot from my mac(L1 cache: 32KB 2^15 L2 cache: 256KB 2^18 L3 cache: 3MB 2^20 8GB RAM) shows the difference between different memory access times PER RECORD. The plot shows the power of 2 of the working set size of the X axis and the number of 'cpu' cycles as returned by clock_gettime CLOCK_MONOTONIC_RAW. The timing before 2^13 should be ignored because of noise and jitter making impossible to measure acurately for working set sizes smaller than that.  

![Alt text](/img/memaccesstest.png?raw=true "memaccesstest plot")

The project was inspired by the following paper: http://www.akkadia.org/drepper/cpumemory.pdf


