echo "Running test 1"
SET currentdir=%cd%
cd ..
cd build
SET currentdir=%cd%
cd mothership
cd configs
start "testmothership" cmd /k python crRelease.conf Atlantis "%currentdir%\apps" localhost scenegraph
cd ..
cd ..
echo %cd%
cd bin	
start "testcrappfaker" cmd /k crappfaker
SET errlevel=0
Scenegraphapp -t
IF %ERRORLEVEL% NEQ 0 (
    SET errlevel=%ERRORLEVEL%
)

taskkill /FI "WindowTitle eq testmothership*" /T /F
taskkill /FI "WindowTitle eq testcrappfaker*" /T /F

echo %errlevel%
cd ..
exit /b %errlevel%
