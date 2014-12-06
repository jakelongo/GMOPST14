GMOPST14
========

Example programs for research related to GMOPST14 presented at AsiaCrypt 2014 
(https://eprint.iacr.org/2014/357)

As a general note, these programs are just example code for the S<sub>c</sub>
simulator and the cross-correlation distinguisher. They are not in any way
resilient to undefined inputs -- they will crash!

The source should compile in windows, osx and linux (although it has only
been tested on a windows machine). Compiling on linux and osx is as simple as
calling g++ on s_c.cpp and xcorr.cpp. Compiling on a windows box has also
been tested with VS2013. Note that xcorr.exe will take very long to execute
for large traces (hence the compression for the ARM traces). Compiler
optimisations for SSE/AVX have been noted to greatly improve performance.

-------------------------------------------------------------------------------
s_c --- split and concat simulator
-------------------------------------------------------------------------------

Usage:

./s_c [data-width] [traceFileListPath_A.txt] [traceFileListPath_B.txt] [outputPrefix] [concatPoint]

[data-width]                -- defines how the trace data is stored (1,2 and 8 for uint8, uint16 and double respectively)  
[traceFileListPath_A.txt]   -- path to a trace file list for the start of the trace (l<sup>p</sup>)  
[traceFileListPath_B.txt]   -- path to a trace file list for the end of the trace (l<sup>c</sup>)  
[outputPrefix]              -- the prefix name for directory and output of concatenated traces  
[concatPoint]               -- the concatenation point of the two sets of traces

The output will be a set of traces of the form l<sup>p</sup> || l<sup>c</sup> concatenated at
[concatPoint].

-------------------------------------------------------------------------------
xcorr --- cross-correlation distinguisher
-------------------------------------------------------------------------------

Usage:

./xcorr [data-width] [traceFileListPath.txt] [outputDirectory] [traceStartPoint] [traceStopPoint] [windowSize]

[data-width]        -- defines how the trace data is stored (1,2 and 8 for uint8, uint16 and double respectively)  
[outputDirectory]   -- path where output files will be placed  
[traceStartPoint]   -- the start point for cross-correlation analysis  
[traceStopPoint]    -- the stop point for cross-correlation analysis  
[windowSize]        -- window size w (generally about a clock cycle for microcontrollers)

The output will be 4 traces (all of data-width double):

[outputDirectory]/_crossCorrelation.dwfm  
[outputDirectory]/_intermediateStdDev.dwfm  
[outputDirectory]/_intermediateSum.dwfm  
[outputDirectory]/_intermediateSum2.dwfm  

they are pretty self explanitory.

-------------------------------------------------------------------------------
Usage for released data sets.
-------------------------------------------------------------------------------

Data set information:

ARM: Raw traces are of data-width uint16. Compressed traces are of data-width 
double and are the sum of all points within each clock cycle. 

SASEBO-GII: traces are of data-width uint16. The bit stream is the default
config available online with AES0 selected 
(http://satoh.cs.uec.ac.jp/SAKURA/hardware/SASEBO-GII.html).

SASEBO-R: traces are of data-width uint16. AES0 selected 
(http://satoh.cs.uec.ac.jp/SAKURA/hardware/TSMC90-130nm.html).

Example batch file (exampleRun.bat) demonstrates running the simulator and 
distinguisher on a windows machine with cygwin for all the trace sets provided.

-------------------------------------------------------------------------------
Errata notes on data sets.
-------------------------------------------------------------------------------

- The data storage system at UoB does not facilitate downloading entire
directories. To account for this, we provide the script downloadDataset.sh to
download the data sets required for each device to run the distinguisher
experiments. Note, the script requires wget to be installed on the system.

- The SASEBO_R traces were part of a larger collect and the excess traces
were not removed prior to upload. Please ignore traces from index 300 upwards.

- The README.txt for the data sets suggests the SASEBO_R and SASEBO_GII are 
compressed. This is not the case, compression was only used on the ARM set.


