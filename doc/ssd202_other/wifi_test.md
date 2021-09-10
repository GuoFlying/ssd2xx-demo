
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

![wifi2](https://gitee.com/mFlying/flying_picgo_img/raw/master/PicGo-updata-img/wifi2.png)
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
![wifi1](https://gitee.com/mFlying/flying_picgo_img/raw/master/PicGo-updata-img/wifi1.png)


