#ifndef client_client_h
#define client_client_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "autils.h"
#include "wsclient.h"

#define WS_TEXT_FRAGMENT 0x81
#define WS_BINARY_FRAGMENT 0x82

#define DEBUG_TEST

#define EXAMPLE_RX_BUFFER_BYTES (1024)

typedef enum _NLSState
{
    CREATED=0,
    CONNECTED,
    HANDSHAKED,
    TRANSFERRING,
    FINISHED,
    FAILED
}NLSState;

typedef struct _NLSClient
{
    wsclient *ws_client;
    NLSState state;
    char* access_key_id;
    char* access_key_secret;
    char* app_key;
    DataPack* writePack;
    DataPack* controlPack;
    char* token;
    int type;
}NLSClient;

void nlsUrlConnect(const char * url,NLSClient *nls_client);
void nlsConnect(const char *host,const int port,const char* subpath,NLSClient *nls_client);
void addPackDataForClient(NLSClient *client, char* buffer, int len,bool autosend);
int buildAuthContent(NLSClient *client,char *result);
#endif
