echo on
setlocal

cd ..
md lenmus_sources
cd lenmus_sources
md build
md docs
md src
md locale
md res
md scores
md xrc

cd build
md msw
cd c:\usr\desarrollo_wx\lenmus
copy .\build\msw\lenmus.sln .\lenmus_sources\build\msw\
copy .\build\msw\lenmus.vcproj .\lenmus_sources\build\msw\

xcopy .\src\*.* .\lenmus_sources\src\*.* /E /Y
xcopy .\docs\release\*.* .\lenmus_sources\docs\*.* /E /Y
xcopy .\locale\*.* .\lenmus_sources\locale\*.* /E /Y
xcopy .\res\*.* .\lenmus_sources\res\*.* /E /Y
xcopy .\scores\*.* .\lenmus_sources\scores\*.* /E /Y
xcopy .\xrc\*.* .\lenmus_sources\xrc\*.* /E /Y

endlocal
