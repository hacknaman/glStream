if %2==1 (
	start "FakeHuman" cmd /k FakeHuman.au3
)

if %2==catia (
	start "FakeHuman" cmd /k FakeHumanCatia.au3
)

SET testdir=%cd%
cd ..
cd build
SET builddir=%cd%
cd mothership
cd configs
SET appPath=%builddir%\apps
IF NOT %3==NUL ( 
	SET appPath=%3
)
start "testmothership" cmd /k python crRelease.conf %1 %appPath% localhost scenegraph
cd ..
cd ..
echo %cd%
cd bin	
start "testcrappfaker" cmd /k crappfaker
SET errlevel=0

SET checkmodelpara="" 
SET selftest="" 

IF NOT [%4]==[]  (
	SET checkmodelpara=--ModelTest %testdir%\%4
) 
IF %2==0  (
	SET selftest=-t --appkilltime 10
)

Scenegraphapp %selftest% %checkmodelpara%

IF %ERRORLEVEL% NEQ 0 (
    SET errlevel=%ERRORLEVEL%
)

taskkill /FI "WindowTitle eq testmothership*" /T /F
taskkill /FI "WindowTitle eq testcrappfaker*" /T /F

taskkill /FI "WindowTitle eq Administrator: testmothership*" /T /F
taskkill /FI "WindowTitle eq Administrator: testcrappfaker*" /T /F

IF NOT %2==0 taskkill /FI "WindowTitle eq FakeHuman*" /T /F
IF NOT %2==0 taskkill /FI "WindowTitle eq Administrator: FakeHuman*" /T /F

echo %errlevel%
cd ..
exit /b %errlevel%
