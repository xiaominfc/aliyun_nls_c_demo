
#mac os 也许需要指定openssl的路基
OPENSSL_INCLUDE=/usr/local/Cellar/openssl/1.0.2s/include
OPENSSL_LIB=/usr/local/Cellar/openssl/1.0.2s/lib

CC=gcc
CFLAGS=-g -c -Wall 
LDFLAGS=-L$(OPENSSL_LIB)  -lssl -lcrypto -lpthread  
INCLUDE=-I./include -I$(OPENSSL_INCLUDE) 
#SOURCES=client.c autils.c token.c parson.c base64.c wsclient.c sha1.c test.c

SRC_DIR=./src

SOURCES=$(shell find $(SRC_DIR) -name "*.c")
OBJECTS=$(SOURCES:.c=.o)
SHARE_LIB=libworkasr.so
EXECUTABLE=test
LIB_OUT=lib


all:cmd  $(SOURCES) $(SHARE_LIB) $(EXECUTABLE)

cmd:
	mkdir -p $(LIB_OUT)

$(EXECUTABLE): $(SHARE_LIB)
	$(CC) -L$(LIB_OUT) -lworkasr  $(INCLUDE)  $(EXECUTABLE).c  -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE)  $< -o $@

$(SHARE_LIB): $(OBJECTS)
	-$(CC)  $(INCLUDE) $(LDFLAGS)  -fPIC -shared  $(OBJECTS) -o $(LIB_OUT)/$(SHARE_LIB)


clean:
	rm -f $(SRC_DIR)/*.o
	rm -f $(EXECUTABLE)
	rm -rf $(EXECUTABLE).dSYM
	rm -rf $(SHARE_LIB)
