ConsoleWrite ( "Fake Catia Human Running ..." )

; Load Model
Local $hWndTC = WinWait("TransViz Client")
Local $hWndCat = WinWait("CATIA V5 - [Product1]")
Local $hWndGL = WinWait("Warning")

WinActivate($hWndGL)
WinClose($hWndGL)

WinActivate($hWndCat)
Send("!s")
Send("{UP")
Send("{UP}")
Send("{UP}")
Send("{UP}")
Send("{UP}")
Send("{UP}")
Send("{ENTER}")

; Camera Shake
Local $hWndCat = WinWait("CATIA V5 - [Engine_4_Cylinders.CATProduct]")
WinActivate($hWndTC)
Sleep(2000)
Send("f")

WinActivate($hWndCat)
Sleep(1000)
MouseMove(924,917)
Sleep(2000)
MouseDown("Middle")
MouseMove(918,843)
MouseUp("Middle")

Sleep(5000)

WinClose($hWndCat)
WinClose($hWndTC)
