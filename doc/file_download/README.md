# 说明
开发过程中，需要经常讲应用拷贝到板子中运行，若通过project image 打包的方式，然后下载，则浪费时间，而且没必要，这边列举几个在**在公板SDK**上就能直接实现拷贝应用的方法。

## 通过USB/SD卡拷贝
-  该方式速度较慢，调试起来还是比较费时

1. 将应用拷贝到USB/SD卡
2. 将USB/SD卡插入板卡中
3. 挂载USB/或SD卡，命令可参考如下：
```shell
# 挂载SD卡，/dev/mmcblk0p1在插入SD卡会USB时，会有对应的log显示
mkdir /customer/sd
mount -t vfat /dev/mmcblk0p1 /customer/sd/
```
4. 挂载完成后，将来自己的应用cp到板子上即可


## 通过网络的方式拷贝
- 设置好板卡IP,之后通过现有的shell命令即可实现应用的下载/上传功能
- 网口配置方式如下
```shell
# dhcpc方式自动获取ip
ifconfig eth0 up                           
ifconfig eth0 hw ether 00:70:27:00:00:01
udhcpc -i eth0 -s /etc/init.d/udhcpc.script

# 设置静态ip
ifconfig eth0 up  
ifconfig eth0 172.19.24.186 netmask 255.255.255.0
ifconfig eth0 hw ether 00:70:27:00:00:01
```
- 通过wifi也可以，但wifi配置的相对复杂，这边不具体说明。

#### 1.通过wget 命令下载
- wget 命令可以从http中下载文件命令如下`wget http://192.168.204.174:8888/http-server.bin`
- 前提是需要192.168.204.174的主机开了8888端口的http server服务
- 开启http server服务的方式很多，可自行百度，这边提供一个python的简单命令可快速实现开启http server。

```shell
# 在window上，安装好python3之后，``win+R`开启`cmd`端口后输入如下命令可在终端当前文件夹中开启http server服务
# ip地址为自己的windows ip地址
python -m http.server 8888

# 在ubuntu 上可直接通过如下命令，即可在当前文件夹中开启http server服务
python3 -m http.server 8888
```
#### 2.通过 tftp 命令下载
- tftp下载命令如`tftp -g -l songti.ttf -r songti.ttf 192.168.204.174`
- 前提是需要开启tftp服务，ubuntu上课自行百度如何开启tftp服务
- windows上，在SDK中，就有`Tftpd32.rar`应用，打开设置好IP，设置好文件夹即可
- ps: 上传命令： `tftp -p -l songti.ttf -r songti.ttf 192.168.204.174`

#### 3.通过nfs命令 下载
- nfs下载命令如`：mount -t nfs -o nolock 192.168.204.174:/h/tmp tmp`
- 前提是需要开启nfs服务，ubuntu上课自行百度如何开启nfs服务
- windows上，在SDK中，就有`win_nfs.rar`应用，打开设置好设置好文件夹即可
- 该方式配置好之后，使用最为方便