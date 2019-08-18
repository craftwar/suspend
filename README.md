# suspend
suspend/resume a list of processes in Windows (***Not all process can be suspened/resumed properly, use in caution***)  
I write this for replacing pssuspend.exe in Sysinternals.  
add features that aren't in pssuspend.exe: unicode support, multiple target processes  
  
Suspend processes temporarily to save resource usage (CPU, GPU...etc) and resume them later. It is faster than closing and restarting.  
ex: suspend a game to play another game smoothly or use as fast game specific mute. (suspend the other game when lol starts)  

This program may be blocked by antivirus.(Add it to whitelist/exception if this happens)  
[lastest build](https://github.com/craftwar/suspend/releases/tag/git)


If you wana terminate mutiple processes.
Taskkill /T /F /IM Firefox.exe /IM Iexplorer.exe /IM Chrome.exe
