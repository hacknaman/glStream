#AutoIt3Wrapper_Change2CUI=y
$TransVizPath = "E:\agent\BuildAgent\work\TransViz\build\"
FileChangeDir ( $TransVizPath & "mothership\configs" )
#include <Constants.au3>
$mothership = Run("python crRelease.conf city " & $TransVizPath & "apps localhost scenegraph")
FileChangeDir ($TransVizPath & "bin" )
Sleep(1000)

$CMD = "scenegraphapp"
$client = Run($CMD, "", "", $STDERR_CHILD + $STDOUT_CHILD)

Sleep(1000)
$CMD = "crappfaker"
$server = Run($CMD, "", "", $STDERR_CHILD + $STDOUT_CHILD)

Sleep(10000)

ProcessClose($mothership);
WinClose("TransViz Client", "")
WinClose("city", "")
