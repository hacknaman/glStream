if %2==1 (
	start "Autoit" cmd /k Apptest.au3
)

SET currentdir=%cd%
cd ..
cd build
SET currentdir=%cd%
cd mothership
cd configs
start "testmothership" cmd /k python crRelease.conf %1 "%currentdir%\apps" localhost scenegraph
cd ..
cd ..
echo %cd%
cd bin	
start "testcrappfaker" cmd /k crappfaker
SET errlevel=0

IF %2==0  (
	Scenegraphapp -t 
)

IF %2==1 ( 
	Scenegraphapp 
)

IF %ERRORLEVEL% NEQ 0 (
    SET errlevel=%ERRORLEVEL%
)

taskkill /FI "WindowTitle eq testmothership*" /T /F
taskkill /FI "WindowTitle eq testcrappfaker*" /T /F

IF %2==1 taskkill /FI "WindowTitle eq Autoit*" /T /F

echo %errlevel%
cd ..
exit /b %errlevel%
