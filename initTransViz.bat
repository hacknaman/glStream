:: SET OSS ROOT PATH
set OSSROOT=%cd%/ExternalLibrary

set "OSSROOT=%OSSROOT:\=/%"

::set GLUT Libraries
set GLUT_DIR=%OSSROOT%/glut
set GLUT_BIN=%GLUT_DIR%/bin/x64

:: set libjpeg libraries
set LIBJPEG_DIR=%OSSROOT%/libjpeg-turbo64
set LIBJPEG_BIN=%LIBJPEG_DIR%/bin

:: add external DLLs to the PATH
:: dynamic libs paths
set PATH=%OSG_BIN%;%OSG_PLUGIN_DIR%;%FLEXERA_BIN%;%GLUT_BIN%;%LIBJPEG_BIN%;%REVIEW_BIN%;%CatiaCppApi_BIN%;%PATH%

:: set development mode
:: This is used for licensing
set Development=false
set 'DBG='
set 'RLS='

call GenerateSolution.bat
cd ..