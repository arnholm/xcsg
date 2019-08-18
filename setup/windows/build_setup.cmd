echo off
setlocal
REM set CPDE_USR=E:\cpde3\usr
REM === This is how elegant backticks are on windows :-)
for /f "tokens=1* delims=" %%a in ('xcsg --version') do set XCSG_VER=%%a
REM ===
echo CPDE_USR=%CPDE_USR%
echo XCSG_VER=%XCSG_VER%
mkdir windows-x86_64
set FILENAME=windows-x86_64\xcsg-%XCSG_VER%-windows-x86_64
del %FILENAME%.zip
"C:\Program Files\7-Zip\7z.exe" a %FILENAME%.zip LICENSE.txt
"C:\Program Files\7-Zip\7z.exe" a %FILENAME%.zip README.txt
"C:\Program Files\7-Zip\7z.exe" a %FILENAME%.zip %CPDE_USR%\bin\xcsg.exe
endlocal
