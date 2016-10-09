Zach Martin

README:

SMake is a basic version of GNU's Make. SMake aids in the creation of executables whenever a projects source files have been updated.
SMake determines how to build a program based on the "SMakefile", which contains rules consisting of targets, dependencies, and commands. 

A rule in an SMakefile must follow the following format:
target: dependencies...
   Command_1
   Command_2
   etc...
   
USAGE:
Running Smake with no other commandline arguments will check each rule and determine if the target needs to be updated/created.
Smake can take one optional commandline argument of the name of a target. If an existing target is given, SMake will begin building from that target and any others affected by it's update. If an invalid target is given, SMake will produce an error and not run.

