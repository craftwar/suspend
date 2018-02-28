# suspend
suspend/resume a list of processes in Windows (***Not all process can be suspened/resumed properly, use in caution***)  
I write this for replacing pssuspend.exe in Sysinternals.  
add features that aren't in pssuspend.exe: unicode support, mutiple target processes  
  
Suspend processes which are not needed to save CPU usage.  
ex: suspend a game to play another game smoothly or use as fast mute. (suspend the other game when lol starts)  

[lastest build from appveyor](https://ci.appveyor.com/api/projects/craftwar_appveyor/suspend/artifacts/suspend%20with%20docs.7z?branch=master) (when I am lazy to make realease on github)  

If you get missing dll error, try to install [Visual C++ Redistributable for Visual Studio 2017](https://go.microsoft.com/fwlink/?LinkId=746571)  

usage guide and sample batch file  
https://github.com/craftwar/suspend/tree/master/suspend
