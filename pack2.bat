copy main.cpp upload
copy src\*.cpp upload
copy src\homework\*.cpp upload
xcopy /E include upload\include\
cd upload
7z u -tzip upload.zip -mmt *