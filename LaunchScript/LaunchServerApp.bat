SET ApplcaitonPath=D:/Project/TransViz/build/bin/gl
SET ApplcaitonName=CNEXT
SET CLIENT_HOSTNAME=localhost
SET SPUName=scenegraph 

IF %ApplcaitonName% == review SET SPUName=avevascenegraph
IF %ApplcaitonName% == CNEXT SET SPUName=catiascenegraph
IF %ApplcaitonName% == composerplayer SET SPUName=catiacomposerplayer
echo %SPUName%

cd ..
SET @TransVizPath=%cd%
cd LaunchScript

SET TransVizInstalledPath=%@TransVizPath%
start /B cmd /k "runConfig.bat" 
runFaker.bat