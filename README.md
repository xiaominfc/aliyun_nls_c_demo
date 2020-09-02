# 说明

阿里云的实时语音识别服务(ASR)没有提供C的SDK,项目中需要,看了它java sdk的实现,就做了个C版demo

#  编译

连接aliyun的NLS需要ssl 所以得先给自己的系统装上openssl

### for centos 

```
yum install openssl-devel
yum install uuid-devel
yum install libuuid-devel
```


## 测试

修改 test.c 

~~~
#define KEYID "{your key_id}"
#define KEYSECRET "{your key_secret}"
#define APPKEY "{your app_key}"

~~~

改成你申请的

~~~
make
./test
~~~
哈哈 就可以看到返回的结果了

ps:音频需要8000采样率单声道的pcm数据 存成wav的格式 方便测试 这些都写死在程序里了,可以改

# 感谢

[parson](https://github.com/kgabis/parson)


