LIBS = -lsndfile -lportaudio

mmai: src/main.c
	$(CC) $(LIBS) src/main.c -o build/mmai
dummy: src/main_dummy.c
	$(CC) src/main_dummy.c -o build/dummy
