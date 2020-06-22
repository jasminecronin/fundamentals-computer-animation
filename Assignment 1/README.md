Flying Susan

Author: Andrew Robert Owens
Date: January 30, 2018
Copyright (2018)

Description

Flying Susan...

Note:
This version has no dependencies on C++17, nor std::optional.

The project now includes its own version of glad AND glfw that need to be
compiled with project. This should happen automatically with any of the options
below for installation.

**INSTALL**

Option 1:

1) Load up in QT Creator


Option 2 (untested, but typical for cmake):

1) From the project director in terminal

mkdir build
cd build

#Unix makefile (Tested)
cmake ..

#Mac OSX (Tested)
cmake -G "Xcode" ..

Note: You may have to set you working director of your project manually to the directory where
your curves and shaders folders are inorder for the executable to find them.

#Microsoft Windows (Tested)
cmake -G "Visual Studio 15" ..
cmake -G "Visual Studio 15 Win64" ..


**USAGE**

./FlyingSusan <curve-file-path>

If no arguments are passed in, the executable will default to a simple curve input. Otherwise, 
the curve is loaded from the provided file.


**USER INTREFACE**

w	move forward
a	move left
s	move backward
d	move right
q	move down
e	move up

ctrl+left-click+drag	rotate camera around focus point

[	decrease move speed
]	increase move speed
{	decrease rotate speed
}	increase rotate speed

ctrl+s	stop
ctrl+r	run
esc	escape


