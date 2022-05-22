@echo off
setlocal
path=lib;%cd%;%path%
echo %cd%
start lib\cw.exe
endlocal
@echo on