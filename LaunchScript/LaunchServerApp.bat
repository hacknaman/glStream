SET ApplcaitonPath="C:\Program Files\Dassault Systemes\B20\win_b64\code\bin"
SET ApplcaitonName=CNEXT
SET CLIENT_HOSTNAME=DESKTOP-RGHON0Q

SET TransVizInstalledPath=C:/Program Files (x86)/VRPlay/

::SET TransVizInstalledPathConfig=%TransVizInstalledPath%LaunchScript/runconfig.bat
::SET fakerpath=%TransVizInstalledPath%LaunchScript/runfaker.bat
SET PATH=%PATH%;%TransVizInstalledPath%LaunchScript
start /B cmd /c "runconfig.bat"

runFaker.bat
