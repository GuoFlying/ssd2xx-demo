# 概要
- 需要开启wifi功能，并拥有wifi相关库、工具。

## wifi_demo1
-  SSD202 - v030版本的SDK验证
-  使用MI的WLAN的接口
-  阻塞
-  连接超时为10秒

```shell
# 表示需要操作的 wifi名称为 xxxxx 密码为12345678
# 创建出来的ap名称为：xxxxx_ssd2xx
./wifi_demo1 xxxxx 12345678

输入‘m‘， 切换STA/AP模式，默认是STA模式；
printf("please input option command:\n");
printf("m. switch STA/AP mode, input 'm'\n");
printf("c. connect wifi hotspot, input 'n'\n");
printf("d. disconnect wifi hotspot, input 'd'\n");
printf("p. print wifi hotspot's info, input 'p'\n");
printf("h. help\n");
printf("q. exit, input 'q'\n\n");
```

## wifi_sta_saync 
-  SSD202 - v030版本的SDK验证
-  使用MI的WLAN的接口
-  异步方式连接wifi
-  MI接口都是阻塞的，所以该DMEO中打开一个线程专门处理wifi配置，之后再通过回调函数返回给主线程。
-  使用到common-list 的接口

```shell
# 当wifi打开成功后，会提示输入如下命令操作。按提示操作即可
./wifi_sta_saync

    printf("please input option command:\n");
    printf("g. 扫描wifi\n");
    printf("c. 连接wifi,之后按提示输入\n");
    printf("d. 手动断开wifi连接\n");
```
