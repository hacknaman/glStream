:: SET OSS ROOT PATH
set OSSROOT=C:/Project/TransViz/ExternalLibrary

:: set OSG Directories
set OSG_DIR=%OSSROOT%\osg-3.4
set OSG_BIN=%OSG_DIR%\bin
set OSG_PLUGIN_DIR=%OSG_BIN%\osgPlugins-3.4.2

:: add external DLLs to the PATH
:: dynamic libs paths
set PATH=%OSG_BIN%;%OSG_PLUGIN_DIR%;%PATH%

SET TrasVizPath=C:\Project\TransViz\build\
cd %TrasVizPath%bin
ScenegraphApp -mothership DESKTOP-PDG0UKJ