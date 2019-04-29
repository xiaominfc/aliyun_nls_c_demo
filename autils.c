/*
 * =====================================================================================
 *
 *       Filename:  autls.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/18 10:37:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiaominfc (xiaoyang) 
 *         Eamil:   xiaominfc@gmail.com
 *   Organization:  武汉鸣鸾信息科技有限公司
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include "autils.h"

DataPack * buildNodeForData(char* buffer,unsigned int len) {
    DataPack *pack = (DataPack*)malloc(sizeof(DataPack));
    pack->nextPack = NULL;
    pack->data = malloc(sizeof(char) * len + 4);
    memset(pack->data, 0, len  + 4 );
    pack->data[0] = (len >> 24);
    pack->data[1] = ((len >> 16) & 0xFF);
    pack->data[2] = ((len >> 8) & 0xFF);
    pack->data[3] = (len & 0xFF);
    memcpy(pack->data + 4 ,buffer,len);
    pack->len = len + 4;
    return pack;
}

void freePack(DataPack* pack) {
    free(pack->data);
    free((void*)pack);
}


void sys_usleep(unsigned int us)
{
    if (!us)
        return;

#ifdef WIN32
    Sleep(us / 1000);
#elif defined(HAVE_SELECT)
    do {
        struct timeval tv;

        tv.tv_sec  = us / 1000000;
        tv.tv_usec = us % 1000000;

        (void)select(0, NULL, NULL, NULL, &tv);
    } while (0);
#else
    (void)usleep(us);
#endif
}

void sys_msleep(unsigned int ms)
{
    sys_usleep(ms * 1000);
}



int cpy_to_target(char *source,int start,int len, char *target,int index) {
    int i = 0;
    while(i < len) {
        target[index + i] = source[start + i];
        i ++;
    }
    return index + len;
}


int buildGMTTime(char *showTime) {
    time_t timep;
    time(&timep);
    char* tmpTime = asctime(gmtime(&timep));
    int len = strlen(tmpTime) - 1;
    tmpTime[len]=0;
    int index = 0;
    index = cpy_to_target(tmpTime,0,3,showTime,index);
    index = cpy_to_target(", ",0,2,showTime,index);
    index = cpy_to_target(tmpTime,8,2,showTime,index);
    index = cpy_to_target(" ",0,1,showTime,index);
    index = cpy_to_target(tmpTime,4,3,showTime,index);
    index = cpy_to_target(" ",0,1,showTime,index);
    index = cpy_to_target(tmpTime,20,4,showTime,index);
    index = cpy_to_target(" ",0,1,showTime,index);
    index = cpy_to_target(tmpTime,11,8,showTime,index);
    index = cpy_to_target(" GMT",0,4,showTime,index);
    return index;
}


char* autils_base64_encode(const unsigned char *input, int length)
{
  BIO *bmem, *b64;
  BUF_MEM *bptr;
  b64 = BIO_new(BIO_f_base64());
  bmem = BIO_new(BIO_s_mem());
  b64 = BIO_push(b64, bmem);
  BIO_write(b64, input, length);
  BIO_flush(b64);
  BIO_get_mem_ptr(b64, &bptr);
  char *buff = (char *)malloc(bptr->length);
  memcpy(buff, bptr->data, bptr->length-1);
  buff[bptr->length-1] = 0;
  BIO_free_all(b64);
  return buff;
}


int json_get_integer(JSON_Value *json_value,char *key) {
    return (int)json_object_get_number(json_value_get_object(json_value),key);
}

int json_dotget_integer(JSON_Value *json_value,char *key) {
    return (int)json_object_dotget_number(json_value_get_object(json_value),key);   
}

char* json_dotget_string(JSON_Value *json_value,char *key) {
    return json_object_dotget_string(json_value_get_object(json_value),key);
}


long current_time(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


void build_uuid(char *uuid_str){
    uuid_t uuid_bind;
    uuid_generate_random(uuid_bind);

// #ifdef capitaluuid
//     /* Produces a UUID string at uuid consisting of capital letters. */
//     uuid_unparse_upper(uuid_bind, uuid_str);
// #elif lowercaseuuid
//     /* Produces a UUID string at uuid consisting of lower-case letters. */
//     uuid_unparse_lower(uuid_bind, uuid_str);
// #else
//     /*
//      * Produces a UUID string at uuid consisting of letters
//      * whose case depends on the system's locale.
//      */
//     uuid_unparse(uuid_bind, uuid_str);
// #endif
    uuid_unparse_lower(uuid_bind, uuid_str);
}


void remove_str_char(char *str,char key,int len) {
    int index = 0;
    int position = 0;
    while(index < len) {
        if(str[index] != key) {
            str[position] = str[index];
            position ++ ;
            index ++;
        }else {
            index ++;
        }
        
    }
    while(position < len) {
        str[position] = 0;
        position ++ ;
    }
}

char *current_task_id(){
    int maxLen = 37;
    char *uuid_str = malloc(maxLen);
    build_uuid(uuid_str);
    //printf("uuid:%s\n",uuid_str);
    remove_str_char(uuid_str,'-',maxLen);
    return uuid_str;
}