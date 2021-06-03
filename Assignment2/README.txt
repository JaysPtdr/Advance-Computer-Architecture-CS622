PART 1

generating traces for the given programs.

Building addrtrace.so file : make obj-intel64/addrtrace.so

compile and run our PIN tool : ../../../pin -t obj-intel64/addrtrace.so -- ./prog1 8  (for program 1)

=======================================================================================================================================
=======================================================================================================================================

PART 2

Genrating machine access from the above generated traces

Compile command : g++ Que2.cpp

Run command : ./a.out

a file having cumulative density with access distances is generating after running this program,now plot the graph as follows,

plot command : python3 plotCDF.py


========================================================================================================================================
========================================================================================================================================

PART 3

Here we will compute Miss traces of cache,

Compile Command : g++ Que3.cpp

Run Command : ./a.out

Now we will pass this above miss trace generated file to Que2.cpp and it results a file having cumulative density with access distances.

Now to plot the graph run command : python3 plotCDF.py 

=========================================================================================================================================
=========================================================================================================================================

PART 4

Here we will compute block per thread

Compile Command : g++ Que4.cpp

Run command : ./a.out