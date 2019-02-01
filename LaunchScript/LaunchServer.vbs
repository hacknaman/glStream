Set oShell = CreateObject ("Wscript.Shell")
Dim strArgs
strArgs = "cmd /C LaunchServerApp.bat"
oShell.Run strArgs, 0, false