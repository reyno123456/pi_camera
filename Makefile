

CC=arm-linux-gnueabihf-

LDFLAGS += -el

target:
	$(CC)gcc -g -mlittle-endian -o video_main  video_main.c -lpthread
clean:
	@rm -vf video_main
