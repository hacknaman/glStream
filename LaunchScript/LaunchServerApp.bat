SET ApplcaitonPath=D:/Project/TransViz/build/bin/gl
SET ApplcaitonName=city
SET CLIENT_HOSTNAME=localhost
cd ..
SET @TransVizPath=%cd%
cd LaunchScript

SET TransVizInstalledPath=%@TransVizPath%
start /B cmd /k "runConfig.bat" 
runFaker.bat