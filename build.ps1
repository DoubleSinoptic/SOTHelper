
Invoke-WebRequest https://github.com/mstorsjo/llvm-mingw/releases/download/20240404/llvm-mingw-20240404-ucrt-x86_64.zip -OutFile ./tools.zip
Expand-Archive ./tools.zip -DestinationPath ./
./llvm-mingw-20240404-ucrt-x86_64/bin/clang++ SotHelper.cpp -std=c++20 -O3 -fno-exceptions -static -o ./SOT_helper.exe -lkernel32 -luser32 -lgdi32 -mwindows
pause