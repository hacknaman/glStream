:: SET OSS ROOT PATH
set OSSROOT=%cd%/ExternalLibrary
set "OSSROOT=%OSSROOT:\=/%"

:: copy osg plugins
robocopy %OSSROOT%\osg-3.4\bin .\build\bin *.dll /MT:25
robocopy %OSSROOT%\osg-3.4\bin\osgPlugins-3.4.2 .\build\bin *.dll /MT:25

:: copy glut
robocopy %OSSROOT%\glut\bin\x64 .\build\bin *.dll /MT:25