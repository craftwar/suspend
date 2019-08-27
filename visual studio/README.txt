Usage:
suspend.exe [-n] [-r] [-t] ProcessName1 [ProcessName2] .....

Options
switch can be specified in any postion (first ones are recommended)
default mode is suspend
-n	no operation mode (for test or whatever)
-r	resume mode
-t	suspend then resume after pressing any key

ex:
to suspend
suspend.exe a.exe b.exe

to resume
suspend.exe -r a.exe b.exe


.cmd/.bat batch file usage: (save in UTF-8, not UTF-8 BOM, cmd can't handle UTF-8 BOM)
sample file: suspend.cmd (I use this script to fast suspend/resume)
modify the second line targets to yours
Recommend UTF-8 editor: NotePad++
Don't use windows built-in Notepad. (It will add BOM to UTF-8 file)
NotePad supports UTF-8 no BOM since Win10 1903
