LIBS = -lsndfile -lportaudio

mmai: src/main.c
	$(CC) $(LIBS) src/main.c -o build/mmai
dummy: src/main_dummy.c
	$(CC) src/main_dummy.c -o build/dummy

#clean:
	#rm -r build/CMakeFiles
	#rm build/CMakeCache.txt build/cmake_install.cmake build/Makefile
