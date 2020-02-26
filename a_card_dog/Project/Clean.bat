@echo off

color 0a
CA_Replace Project.uvprojx DCM.DLL DARMSTM.DLL
CA_Replace Project.uvprojx -pCM3 -pSTM32F103ZE
CA_Replace Project.uvprojx TCM.DLL TARMSTM.DLL

color 1a
echo Keil :

del *.Administrator /s
del *.lst /s
del *.scvd /s
forfiles /s /m DebugConfig /c "cmd /c if @isdir == TRUE rd /s /q @path"
forfiles /s /m Debug /c "cmd /c if @isdir == TRUE rd /s /q @path"
forfiles /s /m Release /c "cmd /c if @isdir == TRUE rd /s /q @path"

color 2b
echo IAR:

del *.dep /s
del *.ewd /s
del *.ewt /s
forfiles /s /m Debug /c "cmd /c if @isdir == TRUE rd /s /q @path"
forfiles /s /m Release /c "cmd /c if @isdir == TRUE rd /s /q @path"
forfiles /s /m settings /c "cmd /c if @isdir == TRUE rd /s /q @path"

exit
