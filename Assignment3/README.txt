Firstly we calculated the machine access traces per thread. Now this traces are feed into particular processor as per the thread id.

To generate the Machine Access traces we need to run the PIN tool named as addrtrace.cpp

Building addrtrace.so file : make obj-intel64/addrtrace.so

compile and run our PIN tool : ../../../pin -t obj-intel64/addrtrace.so -- ./prog1 8  (for program 1)

============================================================================================================================================================
============================================================================================================================================================

Now this address traces given to the processors as input and now we are able to run our Simulator.

Compile : g++ cacheSimulator.cpp

Run : ./a.out

This will generate many outputs and they all are mentioned in report.

============================================================================================================================================================
============================================================================================================================================================
