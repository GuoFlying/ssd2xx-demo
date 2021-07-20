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

## 测试命令
# SSD202 wifi 测试命令
## 依次执行下面命令后，可连接到 帐号为 SKY 密码为 12345678的wifi
## 注意
- 若各命令返回的为FAIL则证明命令错误，需要注意调整下命令
### 0、环境准备
- 依次执行
1、`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/customer/lib:/customer/libdns:/lib:/config/wifi/`
2、`/config/wifi/ssw01bInit.sh`

### 1、启动wifi
`/config/wifi/wpa_supplicant  -iwlan0 -Dnl80211 -c/appconfigs/wpa_supplicant.conf -B`

### 2、扫描ap
`/config/wifi/wpa_cli -i wlan0 scan -p /tmp/wifi/run/wpa_supplicant/`

### 3、获取扫描结果（查看是否有搜索到待连接的wifi，并查看信号强度，太弱可能连不上）
`/config/wifi/wpa_cli -i wlan0 scan_result -p /tmp/wifi/run/wpa_supplicant`

![](https://www.showdoc.cc/server/api/attachment/visitfile/sign/00dd64312750e8a75dc2e3a601adb959?showdoc=.jpg)
### 4、添加网络-必须记住返回值后续有用（`假设返回值为0`）
`/config/wifi/wpa_cli -i wlan0 add_network -p /tmp/wifi/run/wpa_supplicant/`

### 5、设置要连接的ssid
- 网络为0 ，刚才获取的。 SKY 为ssid（wifi名称）

`/config/wifi/wpa_cli -i wlan0 set_network 0 ssid '"SKY"' -p /tmp/wifi/run/wpa_supplicant/`

### 6、设置要连接的wifi密码
- 网络为0 ，刚才获取的。 12345678 为wifi密码

`/config/wifi/wpa_cli -i wlan0 set_network 0 psk '"12345678"' -p /tmp/wifi/run/wpa_supplicant/`

### 7、选择网络，
`/config/wifi/wpa_cli -i wlan0 select_network 0 -p /tmp/wifi/run/wpa_supplicant/`

### 8、使能网络
`/config/wifi/wpa_cli -i wlan0 enable_network 0 -p /tmp/wifi/run/wpa_supplicant/`

### 9、开启wifi dhcp自动连接获取ip
`udhcpc  -q -i wlan0 -s /etc/init.d/udhcpc.script &`

### 10、保存刚才配置
`/config/wifi/wpa_cli -i wlan0 save_config -p /tmp/wifi/run/wpa_supplicant/`

### 11、查看是否连接wifi成功
`ifconfig`
- 若wlan0出现ip则证明连接成功
![](https://www.showdoc.cc/server/api/attachment/visitfile/sign/a0177966fbce281ce44346a12238ee1a?showdoc=.jpg)


