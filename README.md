# Negmanity
A text editor similar to GNU Nano, but with the ability to run files (among other things I want to implement in it). 

-------------------------
#    Description
The program is in development, it is not ready to be used yet, it has certain bugs or failures that I still don't know how to
solve and I bring this program here in case someone with more experience or knowledge, can finish/improve it (I know that there
must be several things badly programmed).

This program uses a configuration file called "negmanity.conf", where you can change colors, key shortcuts, among other things.
If Negmanity can't finds this file, it will use its own default settings.

To be able to run files, it also uses a file called "runner.sh", which receives the path of the file to be executed + the
statement of whether to build or run. runner.sh check what is the extension of the file to run and depending on it, execute
what is necessary (gcc, g++, Python, etc), obviously you can edit it as you want it to run them and what languages it should
recognize.
