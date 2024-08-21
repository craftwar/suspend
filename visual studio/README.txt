Usage:
suspend.exe [-n] [-r] [-t] ProcessName1 [ProcessName2] .....

Options
switch can be specified in any postion (first ones are recommended)
default mode is suspend
-n	no operation mode (for test or whatever, don't suspend or resume)
-r	resume mode
-t	toogle mode (suspend then resume after pressing any key)
-m	turn off monitor after suspending process and specified milliseconds (can be combined with suspend, resume or toogle)
-s	suspend computer, the system enters a suspend (sleep) state
-p	pid mode, all arguments after this option are interpreted as pid

ex:
to suspend
suspend.exe a.exe b.exe

to suspend and turn off monitor after 5000 milliseconds
suspend.exe a.exe b.exe -m 5000

to resume
suspend.exe -r a.exe b.exe


.cmd/.bat batch file usage: (save in UTF-8, not UTF-8 BOM, cmd can't handle UTF-8 BOM)
sample file: suspend.cmd (I use this script to fast suspend/resume)
modify the second line targets to yours
"suspend with monitor off.cmd" add -m option to suspend.cmd (turn off monitor)

Recommend UTF-8 editor: NotePad++
Don't use windows built-in Notepad. (It will add BOM to UTF-8 file)
NotePad supports UTF-8 no BOM since Win10 1903
