
#mac os 也许需要指定openssl的路基
OPENSSL_INCLUDE=/usr/local/Cellar/openssl/1.0.2q/include
OPENSSL_LIB=/usr/local/Cellar/openssl/1.0.2q/lib

CC=gcc
CFLAGS=-g -c -Wall 
LDFLAGS=-L$(OPENSSL_LIB)  -lssl -lcrypto -lpthread  
INCLUDE=-I. -I$(OPENSSL_INCLUDE) 
SOURCES=client.c autils.c token.c parson.c base64.c wsclient.c sha1.c test.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE)  $< -o $@ 

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)
	rm -rf $(EXECUTABLE).dSYM
