
#mac os 也许需要指定openssl的路基
OPENSSL_INCLUDE=/usr/local/Cellar/openssl/1.0.2n/include
OPENSSL_LIB=/usr/local/Cellar/openssl/1.0.2n/lib

CC=gcc
CFLAGS=-g -c -Wall 
LDFLAGS=-L$(OPENSSL_LIB) -L./libwsclient/.libs  -lwsclient -lssl -lcrypto -lpthread  
INCLUDE=-I. -I$(OPENSSL_INCLUDE) -I./libwsclient
SOURCES=client.c autils.c parson.c test.c
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
