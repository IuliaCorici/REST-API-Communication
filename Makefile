CC=g++
CFLAGS=-I.

client: client.cpp requests.cpp helpers.cpp buffer.cpp parson.cpp
	$(CC) -o client client.cpp requests.cpp helpers.cpp buffer.cpp parson.cpp -Wall -Wno-write-strings

run: client
	./client

clean:
	rm -f *.o client
