SET PATH=C:\Qt\5.5\mingw492_32\bin;%PATH%
SET PATH=C:\Qt\Tools\mingw492_32\bin;%PATH%
qmake.exe -makefile -win32 FLViz.pro
mingw32-make.exe clean
mingw32-make.exe mocclean
mingw32-make.exe
mingw32-make.exe distclean
