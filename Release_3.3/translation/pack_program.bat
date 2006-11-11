rem prepare folder with all necessary content for program translation

md pack
cd pack

rem ---- PO files and installer ----------------------
md program
cd program
copy ..\..\..\locale\en\*.po
copy ..\..\..\packages\wxMidi\locale\wxmidi_xx.po
copy ..\..\..\installer\msw\locale\english.nsh
cd ..

rem ---- Hel files ---------------------
md help
cd help
copy ..\..\..\locale\en\help\*.*
cd ..

rem ---- eBooks -------------------------
md eBooks
cd eBooks

md L2_MusicReading
cd L2_MusicReading
copy ..\..\..\..\locale\en\books\L2_MusicReading\*.*
cd ..

cd ..

rem ---- Documents ---------------------
md documents
cd documents
copy ..\..\..\docs\html\licence.htm
copy ..\..\..\docs\html\installation.htm
copy ..\..\..\installer\msw\license_english.txt
cd ..

rem ---- End ----------------------------
