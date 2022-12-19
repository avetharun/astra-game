SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.
Run, cmd.exe /c "setlocal && path=lib;%cd%;%path% && echo %cd% && start lib\cw.exe && endlocal"