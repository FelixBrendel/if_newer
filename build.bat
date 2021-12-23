@echo off
pushd %~dp0

echo.
echo compiling ...

cmake -S . -B .build && cmake --build .build || exit /b 1

move .\.bin\Debug\if_newer.exe .\if_newer.exe
rd /S /Q .bin
rd /S /Q .build

popd
