REM doDevice %deviceName %dataWidth %splitpoint %xcorrWindow %xcorrStart %xcorrStop
REM doDevice %1          %2         %3          %4           %5          %6

call :doDevice SASEBO_GII 2 930 20 0 1900
call :doDevice SASEBO_R 2 440 10 0 1000
call :doDevice ARM 8 4610 100 400 8300

goto :EOF

:doDevice
call :doLogs %1/DATA
call :makeFingerprint %1 %2 %4 %5 %6
call :doSC %1 %2 %3 %4 %5 %6
call :doNULL %1 %2 %3 %4 %5 %6
goto :EOF

:makeFingerprint
md %1\DATA\fingerprint
cd %1
..\xcorr.exe %2 DATA/aes_traceList.txt DATA/fingerprint %4 %5 %3
..\xcorr.exe %2 DATA/beg_traceList.txt DATA/beg %4 %5 %3
cd ..
goto :EOF

:doSC
cd %1
..\s_c.exe %2 DATA/beg_traceList.txt DATA/end_traceList.txt sc %3
..\xcorr.exe %2 DATA/sc_traceList.txt DATA/sc %5 %6 %4
cd ..
goto :EOF

:doNULL
cd %1
..\s_c.exe %2 DATA/aes_traceList.txt DATA/NullAes_traceList.txt null %3
..\xcorr.exe %2 DATA/null_traceList.txt DATA/null %5 %6 %4
cd ..
goto :EOF

:doLogs
call :extractLogs %1/aes
call :extractLogs %1/beg
call :extractLogs %1/end
call :extractLogs %1/NullAes
goto :EOF

:extractLogs
gawk ' { print $1 } ' %1Log.dwdb > %1_traceList.txt
gawk ' { print $2 } ' %1Log.dwdb > %1_mes.txt
gawk ' { print $3 } ' %1Log.dwdb > %1_key.txt
gawk ' { print $4 } ' %1Log.dwdb > %1_cip.txt
goto :EOF