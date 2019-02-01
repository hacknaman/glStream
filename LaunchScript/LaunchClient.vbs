Set oShell = CreateObject ("Wscript.Shell")
Dim strArgs
strArgs = "cmd /C LaunchClientApp.bat"
oShell.Run strArgs, 0, false