@ECHO OFF
REM Build Everything

ECHO "Inciando..."


PUSHD engine
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

PUSHD testEngine
CALL build.bat
POPD
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "Tudo foi iniciado com sucesso."