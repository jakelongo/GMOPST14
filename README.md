GMOPST14
========

Example programs for research related to GMOPST14 presented at AsiaCrypt 2014 
(https://eprint.iacr.org/2014/357)

As a general note, these programs are just example code for the S<sub>c</sub>
simulator and the cross-correlation distinguisher. They are not in any way
resilient to undefined inputs -- they will crash!

The code should compile in windows, osx and linux (although it has only been 
tested on a windows machine). Compiling on linux and osx should

-------------------------------------------------------------------------------
s_c --- split and concat program
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

_crossCorrelation.dwfm
_intermediateStdDev.dwfm
_intermediateSum.dwfm
_intermediateSum2.dwfm

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




