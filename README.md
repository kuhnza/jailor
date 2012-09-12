Jailor
======

Jailor is a simple program for launching applications as a given user (usually
unprivileged) inside a chroot jail. 

Installation
------------

`./configure && make`

`[sudo] make install`

Usage
-----

jailor [OPTION]... /path/to/program [ARGS...]

Options:  
&nbsp;&nbsp;-u   The user to run the program as. Defaults to "nobody".  
&nbsp;&nbsp;-j 	Path to your chroot jail directory. Defaults to "/var/jail".  

Notes
-----

* Jailor must be run with root permissions otherwise system calls to `setuid` and `chroot` will fail.
* This program does not create a chroot jail for you. You must create this first yourself. For 
  Ubuntu/Debian systems [https://help.ubuntu.com/community/DebootstrapChroot](https://help.ubuntu.com/community/DebootstrapChroot) 
  is a good resource and will show you how to create a minimal chroot environment.
* Jailor uses [execvp](http://linux.about.com/library/cmd/blcmdl3_execvp.htm) meaning that once your 
  program is launched it overwrites the jailor process.