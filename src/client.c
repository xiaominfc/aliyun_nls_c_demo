#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <fcntl.h>
#include "client.h"

#ifdef DEBUG_TEST
pthread_t test_thread;
#define TEST_WAV_PATH "./test.wav"
#endif

//static char *AUTHBODY = "{\"app_key\":\"nls-service-realtime-8k\",\"format\":\"pcm\",\"response_mode\":\"streaming\",\"sample_rate\":8000}";
static char *AUTHBODY = "{\"app_key\":\"nls-service-realtime-8k\",\"format\":\"pcm\",\"response_mode\":\"streaming\",\"sample_rate\":8000,\"vocabulary_id\":\"b53bfe5a8b214b3c965ccecb8a0cf828\"}";

static int do_write = 0;
static int pack_count = 0;
static long start_time = 0;

void work_write_thread(void *arg) {
	NLSClient* client = (NLSClient*)arg;
	while(1) {
		if(client->state != TRANSFERRING) {
			sys_msleep(20);
			continue;
		}
		if(client->writePack!= NULL && client->writePack->nextPack!=NULL) {
			DataPack *writePack = client->writePack;
			libwsclient_send_fragment(client->ws_client, writePack->data, writePack->len, WS_BINARY_FRAGMENT);
			client->writePack = client->writePack->nextPack;
			freePack(writePack);
		}
		//sys_msleep(20);
	}
	
}

static int  buildDigestContent(char *showTime,char *content) {
	unsigned char digest[MD5_DIGEST_LENGTH];
	MD5((unsigned char*)AUTHBODY, strlen(AUTHBODY), (unsigned char*)&digest);
	return sprintf(content,"POST\napplication/json\n%s\napplication/json\n%s",autils_base64_encode(digest,MD5_DIGEST_LENGTH),showTime);
}


int buildAuthContent(NLSClient *client,char *result) 
{

	if(client->type == 2) {
		return buildAuthContent2(client,result);
	}
	//for old aliyun nls, not used;
	char buf[100];
	buildGMTTime(buf);
	char *content = malloc(1024);
	int contentLen = buildDigestContent(buf,content);
	char *access_key_secret = client->access_key_secret;
	char *access_key_id = client->access_key_id;
	unsigned char* digest = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
	unsigned int digest_len; 
	HMAC(EVP_sha1(), access_key_secret, strlen(access_key_secret), (unsigned char*)content, contentLen, digest, &digest_len);
	char *auth = autils_base64_encode(digest,digest_len);
	char *target= "{\"context\":{\"auth\":{\"headers\":{\"Authorization\":\"Dataplus %s:%s\",\"accept\":\"application/json\",\"content_type\":\"application/json\",\"date\":\"%s\"},\"method\":\"POST\"}},\"enableCompress\":false,\"request\":%s,\"sdkInfo\":{\"sdk_type\":\"java\",\"version\":\"2.0.0\"},\"version\":\"2.0\"}";
	int result_len = sprintf(result,target,access_key_id,auth,buf, AUTHBODY);
	printf("%s\n", result);
	free(digest);
	free(content);
	return result_len;
}


//send start control
static void auth_nls(NLSClient* client) {
	int len = 1024;
	char *str = (char *)malloc(len);
	len = buildAuthContent(client,str);
	str[len] = 0;
	if(client->state == CONNECTED) {
		client->state = HANDSHAKED;	
	}
	libwsclient_send_fragment(client->ws_client, str,len, WS_TEXT_FRAGMENT);
	free(str);
}



//build payload data
JSON_Value* buildPayload(NLSClient *client){
		
	JSON_Value* payload = json_value_init_object();
	JSON_Object* payload_o = json_object(payload);
	json_object_set_number(payload_o,"sample_rate",8000);
	json_object_set_string(payload_o,"format","pcm");
	json_object_set_boolean(payload_o,"enable_intermediate_result",true);
	json_object_set_boolean(payload_o,"enable_inverse_text_normalization",true);
	json_object_set_boolean(payload_o,"enable_punctuation_prediction",false);
	return payload;
}


//build context data
JSON_Value* buildContext(NLSClient *client){
	JSON_Value* context = json_value_init_object();
	JSON_Value* sdk = json_value_init_object();
	JSON_Object* sdk_o = json_object(sdk);
	json_object_set_string(sdk_o,"name","nls-sdk-java");
	json_object_set_string(sdk_o,"version","2.0.2");
	json_object_set_value(json_object(context),"sdk",sdk);
	return context;
}


//build header data
JSON_Value* buildHeader(NLSClient *client) {
	char *message_id = current_task_id();
	char *uuid = message_id;
	char *appkey = client->app_key;
	JSON_Value* header = json_value_init_object();
	JSON_Object* header_o = json_object(header);
	json_object_set_string(header_o,"namespace","SpeechTranscriber");
	json_object_set_string(header_o,"name","StartTranscription");
	json_object_set_string(header_o,"message_id",message_id);
	json_object_set_string(header_o,"appkey",appkey);
	json_object_set_string(header_o,"task_id",uuid);
	free(message_id);
	return header;
}


//package start control data
JSON_Value* buildStartControl(NLSClient *client){
	JSON_Value* payload = buildPayload(client);
	JSON_Value* context = buildContext(client);
	JSON_Value* header = buildHeader(client);
	JSON_Value* startControl = json_value_init_object();
	JSON_Object* startControl_o = json_object(startControl);
	json_object_set_value(startControl_o,"payload",payload);
	json_object_set_value(startControl_o,"context",context);
	json_object_set_value(startControl_o,"header",header);
	return startControl;
}




int buildAuthContent2(NLSClient *client,char *result) {
	JSON_Value* startControl = buildStartControl(client);
	size_t result_len = json_serialization_size(startControl);
	char *controlStr = json_serialize_to_string(startControl);
	memcpy(result,controlStr,result_len);
	json_free_serialized_string(controlStr);
	json_value_free(startControl);
	return result_len;
}



void addPackDataForClient(NLSClient *client, char* buffer, int len,bool autosend) {
	DataPack *pack = buildNodeForData(buffer, len);
	
	if(client->writePack == NULL && client->controlPack == NULL) {
		client->writePack = pack;
		client->controlPack = pack;
	}else {
		client->controlPack->nextPack = pack;
		client->controlPack = pack;
	}
}



#ifdef DEBUG_TEST
static void test_work(void *arg) {
	NLSClient* client = (NLSClient*)arg;
	printf("%s\n", "start test_work");
	int inFd = open(TEST_WAV_PATH,O_RDONLY,0);
	lseek(inFd,44,SEEK_SET);//read header
	int cache_buffer_size = 2000;
	char *buffer = malloc(cache_buffer_size);
	memset(buffer ,0,cache_buffer_size);
	int count = read(inFd,buffer ,cache_buffer_size);
	while(count > 0) {
		pack_count ++;
		addPackDataForClient(client,buffer,cache_buffer_size,false);
		sys_msleep(16000 /cache_buffer_size );
		memset(buffer ,0,cache_buffer_size);
		count = read(inFd,buffer ,cache_buffer_size);
	}
	printf("count:%d\n", pack_count);
	start_time = current_time();
	printf("%ld\n", start_time);
	memset(buffer ,0,cache_buffer_size);
	for(int i = 0; i <  20*1600/cache_buffer_size ; i ++) {
		addPackDataForClient(client,buffer,cache_buffer_size,false);
		sys_msleep(16000 /cache_buffer_size );
	}

	free(buffer);
}
#endif

int onnlsclose(wsclient *c) {
	fprintf(stderr, "onclose called: %d\n", c->sockfd);
	return 0;
}

int onnlserror(wsclient *c, wsclient_error *err) {
	fprintf(stderr, "onerror: (%d): %s\n", err->code, err->str);
	if(err->extra_code) {
		errno = err->extra_code;
		perror("recv");
	}
	return 0;
}


//for new aliyun nls
int onnlsmessage2(wsclient *c, wsclient_message *msg){
	printf("%s\n", msg->payload);
	NLSClient *nls_client = (NLSClient *)c->user_data;
	JSON_Value *json = json_parse_string(msg->payload);
	int code = json_dotget_integer(json,"header.status");
	if(code != 20000000) {
		printf("%s\n", msg->payload);
		return 1;
	}
	if(nls_client->state == HANDSHAKED) {
		nls_client->state = TRANSFERRING;
		#ifdef DEBUG_TEST
		pthread_create(&test_thread, NULL, test_work, nls_client);
		#endif
	}else if(nls_client->state == TRANSFERRING) {
		int status_code = json_dotget_integer(json,"header.status");
		printf("%s:%d\n", "nls status_code:", status_code);
		printf("%s:%s\n", "nls text:", json_dotget_string(json,"payload.result"));
		if(status_code == 0) {
			#ifdef DEBUG_TEST
			printf("%s:%ld\n","send time", current_time() - start_time);
			#endif
		}
	}
	return 0;
}


int onmessage_aliyun(wsclient *c, wsclient_message *msg) {
	NLSClient *nls_client = (NLSClient *)c->user_data;
	if(nls_client->type == 2) {
		return onnlsmessage2(c,msg);
	}

	//for old aliyun nls
	JSON_Value *json = json_parse_string(msg->payload);
	int code = json_get_integer(json,"status_code");
	if(code != 200) {
		printf("%s\n", msg->payload);
		return 1;
	}
	if(nls_client->state == HANDSHAKED) {
		nls_client->state = TRANSFERRING;
		#ifdef DEBUG_TEST
		pthread_create(&test_thread, NULL, test_work, nls_client);
		#endif
	}else if(nls_client->state == TRANSFERRING) {
		int status_code = json_dotget_integer(json,"result.status_code");
		printf("%s:%d\n", "nls status_code:", status_code);
		printf("%s:%s\n", "nls text:", json_dotget_string(json,"result.text"));
		if(status_code == 0) {
			#ifdef DEBUG_TEST
			printf("%s:%ld\n","send time", current_time() - start_time);	
			#endif
		}
	}
	return 0;
}

int onmessage_othernls(wsclient *c, wsclient_message *msg) {
	printf("%s\n",msg->payload);
}

int onnlsopen(wsclient *c) {
	fprintf(stderr, "onnlsopen called: %d\n", c->sockfd);
	NLSClient *nls_client = (NLSClient *)c->user_data;
	if(nls_client->type <= ALIYUN_NEW) {
		nls_client->state = CONNECTED;
		auth_nls(nls_client);
	}else{
		nls_client->state = TRANSFERRING;
		#ifdef DEBUG_TEST
		pthread_create(&test_thread, NULL, test_work, nls_client);
		#endif
	}
	return 0;
}

void nlsConnect(const char *host,const int port,const char* subpath,NLSClient *nls_client) {
	size_t TMP_SIZE = 1024;
	char * tmp_url = (char*)malloc(TMP_SIZE);
	memset(tmp_url,0,TMP_SIZE);
	int end = 0;
	if(port == 443) {
		snprintf(tmp_url,TMP_SIZE,"wss://%s/%s",host,subpath);
	}else {
		snprintf(tmp_url,TMP_SIZE,"ws://%s:%d/%s",host,port,subpath);
	}
	nlsUrlConnect(tmp_url,nls_client);
	free(tmp_url);
}


void nlsUrlConnect(const char * url,NLSClient *nls_client) {
	printf("connect url:%s\n",url);
	wsclient *client = libwsclient_new(url);
	client->user_data = nls_client;
	if(nls_client->type == ALIYUN_NEW) {
		client->have_token = 1;
		client->token = nls_client->token;
	}else {
		client->have_token = 0;
	}
	libwsclient_onopen(client, &onnlsopen);
	if(nls_client->type <= ALIYUN_NEW) {
		libwsclient_onmessage(client, &onmessage_aliyun);
	}else {
		libwsclient_onmessage(client, &onmessage_othernls);
	}
	libwsclient_onerror(client, &onnlserror);
	libwsclient_onclose(client, &onnlsclose);
	nls_client->ws_client = client;
	pthread_t write_thread;
	pthread_create(&write_thread, NULL, work_write_thread, (void *)nls_client);
	libwsclient_run(client);
}

void clientClose(NLSClient* client){
	libwsclient_close(client->ws_client);
	client->state = CLOSED;
}



