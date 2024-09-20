


/*
    STATIC
    gcc -c main.c -o b1.o
    ar crs lib1.a b1.o b2.o -- collect object files to static lib
    ar -t lib1.a -- check objects files in lib
    gcc main.o -lib.a -lm -o res.exe -- build prog and linking an lib
*/

/*
    DLL
    gcc -c f1.c -o b1.o -- creating an object files
    gcc -shared b1.o -o lib1.dll -- creating sahred library based on object files
    gcc -o prog.exe main.c -L. ./lib1.dll --build programm with link an dll lib (lib1.dll)
*/