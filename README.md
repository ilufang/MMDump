MMDump
======

**MMD Answer Dumping Binary**

**This file must be compiled for Android in order to work. Do NOT compile&run in XCode as I only use XCode as a convenient source code editor**

###About

MMDump listens on the first available network and looks for packets containing json-formatted answers for MeMeDa. When found, it prints the answers in number format into the stdout.

Although you can run it from Terminal simulator or adb shell and read answer from the console, it is recommended that you use it with an Android application that provides a friendly user interface. My version is displaying a toast when a new line is created. It watches on the DataInputStream returned by Runtime.exec(). (I will release the apk a bit later~)

###Library

MMDump used Libpcap, developed by the TCPDump Team.  
[TCPDump & Libpcap Home Page](http://www.tcpdump.org)  
Libpcap provides packet capturing capabilities for the project.

###Notes

To build and run properly, you have to compile the cpp and libpcap for ANDROID. You will need root permission to execute properly. (therefore it is impossible to have packets captured inside the Android app...)

Made in my spare time, free to use ANY part of this repo, just add a notice in your project that it is made by me, thx!

Have Fun~
