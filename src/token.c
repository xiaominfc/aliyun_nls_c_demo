/*
 * token.c
 * Copyright (C) 2019 xiaominfc(武汉鸣鸾信息科技有限公司) <xiaominfc@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "token.h"
#include <openssl/hmac.h>


int str2int(char *str,int len) {
	int result = 0;
	int index = 0;
	while(index < len) {
		result = result * 10 + (str[index] - '0');
		index ++;
	}
	
	return result;
}



int get_token(char *akId,char *akSecrete,char *token){
	char gmttime[100];  
	buildGMTTime(gmttime);
	char *content = malloc(1024);
	memset(content,0,1024);
	int content_len = sprintf(content,"POST\napplication/json\n1B2M2Y8AsgTpgAmY7PhCfg==\napplication/octet-stream;charset=utf-8\n%s\nx-acs-signature-method:HMAC-SHA1\nx-acs-signature-version:1.0\n/pop/2018-05-18/tokens",gmttime);
	unsigned char* digest = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
	unsigned int digest_len; 
	HMAC(EVP_sha1(), akSecrete, strlen(akSecrete), (unsigned char*)content, content_len, digest, &digest_len);
	char *auth = autils_base64_encode(digest,digest_len);
	//printf("%s\n",auth); 
	char *header = malloc(1024);
	memset(header,0, 1024);
	int header_len = sprintf(header,"POST /pop/2018-05-18/tokens HTTP/1.1\r\nHost: nls-meta.cn-shanghai.aliyuncs.com\r\nAuthorization: acs %s:%s\r\nx-sdk-invoke-type: common\r\nAccept: application/json\r\nx-sdk-client: Java/2.0.0\r\nx-acs-signature-version: 1.0\r\nx-acs-signature-method: HMAC-SHA1\r\nDate: %s\r\nContent-MD5: 1B2M2Y8AsgTpgAmY7PhCfg==\r\nContent-Type: application/octet-stream;charset=utf-8\r\nContent-Length: 0\r\n\r\n",akId,auth,gmttime);

	free(digest);
	free(auth);
	int socketfd = libwsclient_open_connection("nls-meta.cn-shanghai.aliyuncs.com","80");
	if(socketfd > 0) {
		send(socketfd, header, header_len, 0);
		free(header);
		unsigned int cache_size = 1024;
		char *buf = malloc(cache_size);
		char *response = malloc(2048);
		memset(response,0,2048);
		int rcount = recv(socketfd, buf, cache_size, 0);
		int size = 0;
		while(rcount > 0) {
			memcpy(response + size, buf, rcount);
			size = size + rcount;
			if(rcount < cache_size) {
				break;
			}
			rcount = recv(socketfd, buf, cache_size, 0);
		}
		close(socketfd);
		free(buf);
		int index = 1;
		char *line = malloc(1024);
		memset(line,0,1024);
		int line_len = 0;
		int status = 0;
		int data_len = 0;
		while(index < size){
			if(response[index] == '\n' && response[index-1] == '\r'){
				if(strncmp(line,"HTTP/1.1 200 OK",line_len) == 0) {
					status = 200;	   
				}else if(strncmp(line,"Content-Length:",15) == 0) {
					data_len = str2int(line+16,line_len - 16);
					break; 
				}
				memset(line,0,256);
				index = index + 1;
				line_len = 0;
			}else {
				line[line_len] = response[index-1];
				line_len = line_len + 1;
			}
			index++;
		} 
	  //  printf("%s\n",response);
		if(status == 200) {
			JSON_Value *json = json_parse_string(response + size - data_len);
			char *token_id = json_dotget_string(json,"Token.Id");
			int token_len = strlen(token_id);
			memcpy(token, token_id, token_len);
			return token_len;
		}
	}else {
		printf("connect failed\n");
	}
	return 0;
}
