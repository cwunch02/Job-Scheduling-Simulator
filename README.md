# Job Scheduling Simulator

This project was one of the last two of my Operating Systems class. This job simulates how the CPU schedules jobs during running time.
The task was to model the MLFQ (Multi-level Feedback Queue) which many processors use to keep away starvation. In addition to implementing
MLFQ, Round-Robin scheduling would need to be implemented as a precursor. These can be found in the mlfq.cc and rr.cc, respectively.
Feel free to try it out with some of the data files given, as both files can be run using

```
make
./jssim -p [policy] -q [list-of-quantums] -w [workload]
```

For example,

```
make
	./jssim -p fifo -w data/w1
	./jssim -p rr -q 2 -w data/w1
	./jssim -p mlfq -q 2 4 -w data/w1
	./jssim -p srtf -w data/w1
```
