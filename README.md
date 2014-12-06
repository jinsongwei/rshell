<<<<<<< HEAD
RSHELL 
===

Licensing Information: READ LICENSE
---

Author & Contributor List

Mike Lzbichi

Songwei Jin

File list
---
```
.:

Makefile

LICENSE

README.md

src

tests
```

```
/src:

rshell.cpp

ls.cpp

cp.cpp

Timer.h

```

```
/tests:

exec.script

ls.script

pipe.script

path.script
```

bugs:
---
1: cann't use up-down-left-right button on the keyboard. 

2: some feature of git command doesn't work well, especially " symbols

3: when the server is busy, the command will not execute immediately, and need to type again.

4: the maximun command strings can only contain 100. It is not unlimit.'	if the commands doesn't exist then return notice, but if there is unformatted symbols or spaces in the first position, then return "wrong format"

5: when using 2>> and 2> must be connect to each other, can not run such as 2 >> or 2 >, must have no space.

6: cann't place > < | >> <<< or 2> on the first or last argument.

7: some g++ compiler (hammer cs100) works weird. So this program did not free three two dimensional pointer array.

8: when you using ^C to exit the current program, the current path will change to initial path where the rshell started. 

9: everytime using C-c or exit the program. the shell will prompt "killed" means the current process was terminated.

10: everytime using C-c, there is a empty line between new rshell start.

11: after compiling, when run the a.out executable file, you don't need type ./  , instead you only type the executable file. 

ls.cpp
---

this function only for ls command.

bugs:
---
1: cann't use up-down-left-right keyboard. 

2: there is a limit for commands, it's not unlimit. 


