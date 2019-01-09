:: SET OSS ROOT PATH
set OSSROOT=D:\WorkArea\TransViz\ExternalLibrary


:: set OSG Directories
set OSG_DIR=%OSSROOT%\osg-3.4
set OSG_BIN=%OSG_DIR%\bin
set OSG_PLUGIN_DIR=%OSG_BIN%\osgPlugins-3.4.2

:: set flexera libraries
set FLEXERA_DIR=%OSSROOT%/flexera/11.12
set FLEXERA_BIN=%FLEXERA_DIR%/bin

::set GLUT Libraries
set GLUT_DIR=%OSSROOT%\glut
set GLUT_BIN=%GLUT_DIR%\bin\x64

:: set libjpeg libraries
set LIBJPEG_DIR=%OSSROOT%\libjpeg-turbo64
set LIBJPEG_BIN=%LIBJPEG_DIR%\bin

:: set Review API libraries
set REVIEW_DIR=%OSSROOT%\ReviewApi
set REVIEW_BIN=%REVIEW_DIR%\bin

:: set Catia API libraries
set CatiaCppApi_DIR=%OSSROOT%\CatiaCppApi
set CatiaCppApi_BIN=%CATIACPPAPI_DIR%\bin

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