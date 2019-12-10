copy main.cpp upload
copy src\*.cpp upload
copy src\homework\*.cpp upload
xcopy /E include upload\include\
cd upload
del upload.zip
7z u -tzip upload.zip -mmt *