#include "client.h"
#include "wsclient.h"
#include <fcntl.h>
#include "autils.h"
#include "token.h"
static NLSClient *nls_client;



#define KEYID "{your key_id}"
#define KEYSECRET "{your key_secret}"
#define APPKEY "{your app_key}"

// callback for onmessage
int onmessage(struct _NLSClient* client, wsclient_message *msg){
    printf("%s\n", msg->payload);
    return 0;
}

// callback for onconnected
int onconnected(struct _NLSClient *client)
{	
    printf("onconnected");
}

// callback for onclose
int onclose(struct _NLSClient *client)		
{
    printf("onclose");
}

// callback for onerror
int onerror(struct _NLSClient *client, wsclient_error *err){
    printf("onerror");
}	
	


int main( int argc, char *argv[] )
{
    char *token = malloc(1024);
    memset(token,0,1024);
    int result = get_token(KEYID,KEYSECRET,token);
    printf("token:%s\n",token);

    nls_client = (NLSClient*)malloc(sizeof(NLSClient));
    nls_client->state = CREATED;
    nls_client->writePack = NULL;
    nls_client->controlPack = NULL;
    nls_client->access_key_id = KEYID;
    nls_client->access_key_secret = KEYSECRET;
    nls_client->token = token;
    nls_client->app_key = APPKEY;
    nls_client->type= 2;
    
    nls_set_onmessage(nls_client,onmessage);
    nls_set_onconnected(nls_client,onconnected);
    nls_set_onclose(nls_client,onclose);
    nls_set_onerror(nls_client,onerror);
    //nlsConnect("nls-trans.dataapi.aliyun.com", 443, "/realtime",nls_client);
    nlsConnect("nls-gateway.cn-shanghai.aliyuncs.com", 443, "ws/v1",nls_client);
    while(1)
    {
    
    }
    return 0;
}
