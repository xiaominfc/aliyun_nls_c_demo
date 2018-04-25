# 说明

阿里云的实时语音服务(ASR)没有提供C的SDK,项目中需要,看了它java sdk的实现,就做了个C版demo

#  编译

## 编译ws依赖[https://github.com/payden/libwsclient](https://github.com/payden/libwsclient)

~~~
git clone https://github.com/payden/libwsclient.git
cd libwsclient
~~~

稍微改一下代码让ws可以跟别的数据体关联
编辑wsclient.h(96-116行)
~~~
typedef struct _wsclient {
    pthread_t helper_thread;
    pthread_t handshake_thread;
    pthread_t run_thread;
    pthread_mutex_t lock;
    pthread_mutex_t send_lock;
    char *URI;
    int sockfd;
    int flags;
    int (*onopen)(struct _wsclient *);
    int (*onclose)(struct _wsclient *);
    int (*onerror)(struct _wsclient *, wsclient_error *err);
    int (*onmessage)(struct _wsclient *, wsclient_message *msg);
    wsclient_frame *current_frame;
    struct sockaddr_un helper_sa;
    int helper_sock;
#ifdef HAVE_LIBSSL
    SSL_CTX *ssl_ctx;
    SSL *ssl;
#endif
} wsclient;
~~~

~~~
typedef struct _wsclient {
    pthread_t helper_thread;
    pthread_t handshake_thread;
    pthread_t run_thread;
    pthread_mutex_t lock;
    pthread_mutex_t send_lock;
    char *URI;
    int sockfd;
    int flags;
    int (*onopen)(struct _wsclient *);
    int (*onclose)(struct _wsclient *);
    int (*onerror)(struct _wsclient *, wsclient_error *err);
    int (*onmessage)(struct _wsclient *, wsclient_message *msg);
    wsclient_frame *current_frame;
    struct sockaddr_un helper_sa;
    int helper_sock;
    void *user_data;//这个就是新加的指针
#ifdef HAVE_LIBSSL
    SSL_CTX *ssl_ctx;
    SSL *ssl;
#endif
} wsclient;
~~~

把wsclient.h 定义的errors(141-163行) 移到 wsclient.c
这样就可以进行编译了

~~~
./autogen.sh
./configure --enable-static=yes --enable-shared=no
make #不执行 make install 这样就不用安装到系统了 我们需要的依赖库已经生成在当前目录下的.libs(是个隐藏目录)下了 
cd ..
~~~

ps: 连接aliyun的NLS需要ssl 所以得先给自己的系统装上openssl

## 测试

修改 test.c 

~~~
 nls_client->app_id = "你申请的AccessKeyId";
 nls_client->app_key = "你申请的AccessKeySecret";
~~~

改成你申请的

~~~
make
./test
~~~
哈哈 就可以看到返回的结果了

# 感谢

[libwsclient](https://github.com/payden/libwsclient)
[parson](https://github.com/kgabis/parson)


