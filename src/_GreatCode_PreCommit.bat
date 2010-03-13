@echo off
echo Formatting m0d Code...
echo.

:: set current directory
SET thisDir="%~dp0"
:: goto proper operational mode
IF '%1'=='' GOTO doall


:single
SET thisFile="%~dpnx1"
echo Indenting file %thisFile%
%thisDir%_utilities\greatcode -file-%thisFile%
GOTO exit


:doall
FOR /F "tokens=*" %%G IN ('DIR /B %thisDir%\*.*') DO (
echo Indenting file "%%G"
%thisDir%_utilities\greatcode -file-"%%G"
)
GOTO exit


:exit
echo.
echo ---------------------------------------------------------
echo.
pause