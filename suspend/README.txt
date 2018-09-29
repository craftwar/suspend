Usage:
suspend.exe [-n] [-r] [-s][-t] ProcessName1 [ProcessName2] .....

Options
switch can be specified in any postion (first ones are recommended)
-n	no operation mode (for test or whatever)
-r	resume mode
-s	case sensitive (case insensitive if not specified)
-t	suspend then resume after pressing any key

ex:
to suspend
suspend.exe a.exe b.exe

to resume
suspend.exe -r a.exe b.exe


.cmd/.bat batch file usage: (save in UTF-8, not UTF-8 BOM)
sample file: suspend.cmd (I use this script to fast suspend/resume)
modify the second line after "set suspend_targets=" to yoour targets
Use NotePad++ to edit it. 
Don't use windows built-in notepad. (It will add BOM to UTF-8 file)
