Usage:

suspend.exe [-n] [-r] [-s] ProcessName1 [ProcessName2] .....

Options
switch can be specified in any postion (first ones are recommended)
-n	no operation mode (for test or whatever)
-r	resume mode
-s	case sensitive (case insensitive if not specified)

ex:
to suspend
suspend.exe a.exe b.exe

to resume
suspend.exe -r a.exe b.exe


use in .cmd/.bat batch file: (save in UTF-8, not UTF-8 BOM)
sample file: suspend.cmd (I use this script to fast suspend/resume)
modify the first line after "set suspend_targets=" to yoour targets
