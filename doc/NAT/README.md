# 1、NAT 简介
- NAT（Network Address Translation，网络地址转换）是1994年提出的。当在专用网内部的一些主机本来已经分配到了本地IP地址（即仅在本专用网内使用的专用地址），但又想和因特网上的主机通信（并不需要加密）时，可使用NAT方法。
## 1.1、 参考
1、[ARM WIFI AP 模式 使用 iptables nat 转发 通过 LAN 网线上网](https://www.cnblogs.com/ningci/p/6834640.html)

2、[iptables 交叉编译](https://www.cnblogs.com/eleclsc/p/11686287.html)

3、[网络地址转换NAT原理](https://blog.csdn.net/weixin_38044696/article/details/80282162)

4、[iptables 详解](https://blog.csdn.net/wangyunfeis/article/details/77725964)


# 2、linux双网卡配置数据包转发
- 本文主要介绍使用iptables实现linux平台下NAT功能。在传统的标准的TCP/IP通信过程中，所有的路由器仅仅是充当一个中间人的角色，也就是通常所说的存储转发，路由器并不会对转发的数据包进行修改，更为确切的说，除了将源MAC地址换成自己的MAC地址以外，路由器不会对转发的数据包做任何修改。
- NAT(Network Address Translation网络地址翻译)恰恰是出于某种特殊需要而对数据包的源ip地址、目的ip地址、源端口、目的端口进行改写的操作。将内网数据包中的地址信息更改成统一的对外地址信息，不让内网主机直接暴露在因特网上，保证内网主机的安全，同时，该功能也常用来实现共享上网。
![net1](https://gitee.com/mFlying/flying_picgo_img/raw/master/PicGo-updata-img/net1.png)
- 当IP包经过NAT网关时，NAT Gateway会将IP包的源IP转换为NAT Gateway的公共IP并转发到公共网，此时IP包（Dst=202.20.65.4，Src=202.20.65.5）中已经不含任何私有网IP的信息。由于IP包的源IP已经被转换成NAT Gateway的公共IP，Web Server发出的响应IP包（Dst= 202.20.65.5,Src=202.20.65.4）将被发送到NAT Gateway。
---------------
- Netfilter/IPTables是Linux2.4.x之后引入的一个子系统，其重要工具模块IPTables从用户态的iptables连接到内核态的Netfilter的架构中，Netfilter与
IP协议栈是无缝契合的，并允许使用者对数据报进行过滤、地址转换（NAT）、处理等操作。iptables是一个应用程序，可以对netfilter进行配置（包过滤
规则，NAT等等）。
![net2](https://gitee.com/mFlying/flying_picgo_img/raw/master/PicGo-updata-img/net2.png)
- 这幅图很直观的反应了用户空间的iptables和内核空间的基于Netfilter的ip_tables模块之间的关系和其通讯方式，以及Netfilter在这其中所扮演的角色。
---------------
- 所以在linux平台要实现NAT就要从两方面来着手：
   - 1）内核支持netfilter
   - 2）用户层的iptables配置命令
   - **Netfilter**是官方内核中提供对报文数据包过滤和修改的一个功能，它位于内核中的tcp/ip协议栈的报文处理框架，它可以用于在不同阶段将某些钩子函数（hook）作用域网络协议栈。Netfilter 本身并不对数据包进行过滤，它只是允许可以过滤数据包或修改数据包的函数挂接到内核网络协议栈中的适当位置。这些函数是可以自定义的。
   - **iptables**是用户层的工具，它提供命令行接口，能够向 Netfilter 中添加规则策略，从而实现报文过滤，修改等功能。在linux中的这样的工具有很多，在centos6上面是iptables，在centos7上面则是firewalld，不过在7上面的时候，依习惯依然会称为iptables。
   - 可以简单的这样理解，我们驱车旅游，Netfilter是车，而iptables是坐在车里面的人，车并没有对路线选择的能力，但是有车能提供行驶及改变路线的能力，决定是否行驶以及行驶到哪里去，这则是由坐在车里面的人来决定的。
## 2.1、 SSD20X kernel配置、内核支持netfilter
- SDK V030版本
- kernel 配置
```
Linux Kernel Configuration
    -> Networking support
        -> Networking options
            -> Network packet filtering framework(netfilter)
                -> Core netfilter configuration
                    -> Netfilter connection tracking support
                    -> NetBIOS name service protocal support
                    -> Netfilter Xtables support (required for ip_tables)


Linux Kernel Configuration
    -> Networking support
        -> Networking options
            -> Network packet filtering framework(netfilter)
                -> IP: Netfilter Configuration
                    -> IPv4 connection tracking support (require for NAT) 
                    -> IPv4 NAT
                    -> IP tables support (required for filtering/masq/NAT)
                    -> iptables NAT support
                         ->MASQUERADE target support
                         ->NETMAP target support
                         ->REDIRECT target support
                    -> raw table support (required for NOTRACK/TRACE)
```

## 2.2、iptables的配置和使用
#### 2.2.1、 源码下载
- ubuntu 下使用git下载最新release版本。
```
git clone git://git.netfilter.org/iptables
cd iptables/
# git tag 可查看当前最新release版本
git checkout v1.8.7
```

#### 2.2.2、 交叉编译
- SSD20X 为例
- 进入iptables，执行autogen.sh(`./autogen.sh`)，得到configure文件。
- 执行命令如下

```
./autogen.sh
mkidr build
./configure --enable-static --prefix=$PWD/build CC=arm-linux-gnueabihf-gcc --host=arm-linux-gnueabihf --disable-nftables --disable-shared
make -j8 
make install
```
- 之后在build文件夹下可看到编译的文件。主要文件为sbin已拷贝至当前文档中的iptables_sbin文件夹中

#### 2.2.3、 使用说明
- 编译出来的文件在当前路径下的bin/sbin里，最终文件是xtables-legacy-multi，但是不能直接运行xtables-legacy-multi，必须建立软连接或者重命名为iptables。
- iptables命令基本参数和用法：
   - iptables –[A|I 链] [-i|o 网络接口] [-p 协议] [-s 来源ip/网域] [-d 目标ip/网域] –j[ACCEPT|DROP]
   - 详情百度
   - eg：查看当前NAT表`./iptables -t nat -L`

