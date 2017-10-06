all:
	$(CC) main.c $(CFLAGS) -o bambam

install: all
	install bambam /usr/local/bin/

uninstall:
	rm /usr/local/bin/bambam

clean:
	rm -rf bambam
