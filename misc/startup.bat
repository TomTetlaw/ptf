@echo off
subst w: "C:\Users\Tom\Documents\Visual Studio 2015\Projects\ptf"
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
set path=w:\misc;%path%
w: