#include "client.h"
#include "wsclient.h"
#include <fcntl.h>
#include "autils.h"
static NLSClient *nls_client;


int main( int argc, char *argv[] )
{
    nls_client = (NLSClient*)malloc(sizeof(NLSClient));
    nls_client->state = CREATED;
    nls_client->writePack = NULL;
    nls_client->controlPack = NULL;
    nls_client->app_id = "你申请的AccessKeyId";
    nls_client->app_key = "你申请的AccessKeySecret";
    nlsConnect("nls-trans.dataapi.aliyun.com", 443, "/realtime",nls_client);
    while(1)
    {
    
    }
    return 0;
}