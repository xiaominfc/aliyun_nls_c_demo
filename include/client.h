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
//#define DEBUG_TEST


//nlsclient type
#define ALIYUN_OLD 1
#define ALIYUN_NEW 2
#define ALIYUN_OTHER 3

#define EXAMPLE_RX_BUFFER_BYTES (1024)


/**
 * @brief enum NLSState mark asr state
 */
typedef enum _NLSState
{
	CREATED=0,		// init 
	CONNECTED,		// connected
	HANDSHAKED,		// connecting
	TRANSFERRING,	// send data
	FINISHED,		
	FAILED,			// failed
	CLOSED			// close
}NLSState;


/**
 * @brief struct NLSClient store info for asr
 */
typedef struct _NLSClient
{
	wsclient *ws_client;		// websocket connection
	NLSState state;				// work state
	char* access_key_id;		// access_key_id for aliyun
	char* access_key_secret;	// access_key_secret for aliyun
	char* app_key;				// app_key for aliyun
	DataPack* writePack;		// for write thread store all data
	DataPack* controlPack;		// for control send data
	int (*onconnected)(struct _NLSClient *);	// callback for onconnected
	int (*onclose)(struct _NLSClient *);		// callback for onclose
	int (*onerror)(struct _NLSClient *, wsclient_error *err);	// callback for onerror
	int (*onmessage)(struct _NLSClient *, wsclient_message *msg);	// callback for onmessage
	char* token;				// token for auth aliyun
	int type;					// mark asr service type (aliyun or other)
	void *user_data;			// store other data
}NLSClient;


/**
 * @brief connect websocket by url and nlsclient
 * @param [const char*] url like ws://ip:port/path
 * @param [NSLClient*] nls_client  store info for asr
 */
void nlsUrlConnect(const char * url,NLSClient *nls_client);

/**
 * @brief connect websocket by host, post, subpath and nlsclient
 * @param [const char*] host ip or domain
 * @param [const int] port tcp connect port
 * @param [const char*] path url path
 * @param [NLSClient*] nls_client
 */
void nlsConnect(const char *host,const int port,const char* subpath,NLSClient *nls_client);

/**
 * @brief add data for send to asr server
 * @param [NSLClient*] client store info for asr
 * @param [char*] buffer data for send
 * @param [int] len  data length
 * @param [bool] autosend  data cached or send now
 */
void addPackDataForClient(NLSClient *client, char* buffer, int len,bool autosend);

/**
 * @brief build auth data for old aliyun asr 
 * @param  [NLSClient*] client  
 * @param  [char*] result  auth data
 * @return [int] auth data length
 */
int buildAuthContent(NLSClient *client,char *result);


/**
 * @brief build auth data for new aliyun asr 
 * @param  [NLSClient*] client  
 * @param  [char*] result  auth data
 * @return [int] auth data length
 */
int buildAuthContent2(NLSClient *client,char *result);

/**
 * @brief close nlsclient
 * @param [NLSClient*] client
 */
void clientClose(NLSClient* client);

/**
 * @brief set onconnected callback for nlsclient
 * @param [NLSClient*] client
 * @param [function*] cb
 */
void nls_set_onconnected(NLSClient *client, int (*cb)(struct _NLSClient *c));

/**
 * @brief set onmessage callback for nlsclient
 * @param [NLSClient*] client
 * @param [function*] cb
 */
void nls_set_onmessage(NLSClient *client, int (*cb)(struct _NLSClient *c, wsclient_message *msg));


/**
 * @brief set onerror callback for nlsclient
 * @param [NLSClient*] client
 * @param [function*] cb
 */
void nls_set_onerror(NLSClient *client, int (*cb)(struct _NLSClient *c, wsclient_error *err));


/**
 * @brief set onclose callback for nlsclient
 * @param [NLSClient*] client
 * @param [function*] cb
 */
void nls_set_onclose(NLSClient *client, int (*cb)(struct _NLSClient *c));

#endif
