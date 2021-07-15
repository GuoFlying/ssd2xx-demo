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
