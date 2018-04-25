/*
 * =====================================================================================
 *
 *       Filename:  autls.h
 *
 *    Description:  some methods
 *
 *        Version:  1.0
 *        Created:  04/20/18 10:31:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiaominfc (xiaoyang), xiaominfc@gmail.com
 *   Organization:  武汉鸣鸾信息科技有限公司
 *
 * =====================================================================================
 */
#ifndef autils_autils_h
#define autils_autils_h

#include "parson.h"


typedef struct _DataPack 
{
    char* data;
    unsigned int len;
    struct _DataPack* nextPack;
} DataPack;



DataPack * buildNodeForData(char* buffer,unsigned int len);

void freePack(DataPack* pack);

int json_get_integer(JSON_Value *json_value,char *key);
int json_dotget_integer(JSON_Value *json_value,char *key);

// sleep us
void sys_usleep(unsigned int us);
// sleep ms
void sys_msleep(unsigned int ms);
//
static int cpy_to_target(char *source, int start, int len, char *target, int index);

int buildGMTTime(char *showTime);

char* autils_base64_encode(const unsigned char *input, int length);

long current_time();

#endif