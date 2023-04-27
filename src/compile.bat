@echo off

echo Compiling libgeopack...

cd fortran
call compile.bat
cd ..

cd matrix
call compileobj.bat
cd ..

cd modelparams
call compileobj.bat
cd ..

cd tools
call compileobj.bat
cd ..

cd tracing
call compileobj.bat
cd ..


compileobj.bat

gfortran -fPIC -fno-automatic -ffree-line-length-none -shared -o libgeopack.dll -lm ..\build\*.o -lstdc++ -lgomp


echo Done
