# 简介
- Perf3是一个主动测量IP网络上最大可实现带宽的工具。它支持与时间、缓冲区和协议(TCP、UDP、带有IPv4和IPv6的SCTP)相关的各种参数的调优。对于每个测试，它都会报告带宽、损耗和其他参数。这是一个新的实现，它不与原始iPerf共享代码，也不向后兼容。iPerf最初是由NLANR/DAST开发的。
- 官网: https://iperf.fr/
- 源码下载路径（也可通过官网自行查找）：https://github.com/esnet/iperf/releases 
- 以下以当前（2021-09-01）最新release版本为例（3.10.1）

# 交叉编译
## 环境
- ubuntu20.04
- 交叉编译工具 arm-linux-gnueabihf-gcc 8.2.1 20180802 (GNU Toolchain for the A-profile Architecture 8.2-2018-08 (arm-rel-8.23))
- SSD20X SDK V30 版本
- **res中为编译后的应用，可在SSD20x上直接运行**

### 解压、配置编译
- 不支持ssl，需要开启ssl支持的话，自行修改编译
```shell
# 
tar -zxvf iperf-3.10.1.tar.gz
cd iperf-3.10.1/
mkdir build

./configure --host=arm-linux-gnueabihf \
CC=arm-linux-gnueabihf-gcc \
CXX=arm-linux-gnueabihf-g++ \
RANLIB=arm-linux-gnueabihf-ranlib \
STRIP=arm-linux-gnueabihf-strip \
ac_cv_func_malloc_0_nonnull=yes \
CFLAGS=-static \
CXXFLAGS=-static \
--with-openssl=no \
--prefix=$PWD/build

make -j8
make install 
```

# 使用方法
### 测试命令
eg：
```
客户端：
iperf3 -c 192.168.1.100 -l50 -i1 -b100k -u -t14400 -p10000
服务器：
iperf3 -s -p10000 -i1 -V > D.txt
```
### iperf Done.
- 参数
```
-p, --port #，Server 端监听、Client 端连接的端口号；

-f, --format [kmgKMG]，报告中所用的数据单位，Kbits, Mbits, KBytes, Mbytes；

-i, --interval #，每次报告的间隔，单位为秒；

-F, --file name，测试所用文件的文件名。如果使用在 Client 端，发送该文件用作测试；如果使用在 Server 端，则是将数据写入该文件，而不是丢弃；

-A, --affinity n/n,m，设置 CPU 亲和力；

-B, --bind ，绑定指定的网卡接口；

-V, --verbose，运行时输出更多细节；

-J, --json，运行时以 JSON 格式输出结果；

--logfile f，输出到文件；

-d, --debug，以 debug 模式输出结果；

-v, --version，显示版本信息并退出；

-h, --help，显示帮助信息并退出。
```
- Server 端参数：
```
-s, --server，以 Server 模式运行；

-D, --daemon，在后台以守护进程运行；

-I, --pidfile file，指定 pid 文件；

-1, --one-off，只接受 1 次来自 Client 端的测试，然后退出。
```
- clinet 端参数
```
-c, --client ，以 Client 模式运行，并指定 Server 端的地址；

-u, --udp，以 UDP 协议进行测试；

-b, --bandwidth #[KMG][/#]，限制测试带宽。UDP 默认为 1Mbit/秒，TCP 默认无限制；

-t, --time #，以时间为测试结束条件进行测试，默认为 10 秒；

-n, --bytes #[KMG]，以数据传输大小为测试结束条件进行测试；

-k, --blockcount #[KMG]，以传输数据包数量为测试结束条件进行测试；

-l, --len #[KMG]，读写缓冲区的长度，TCP 默认为 128K，UDP 默认为 8K；

--cport ，指定 Client 端运行所使用的 TCP 或 UDP 端口，默认为临时端口；

-P, --parallel #，测试数据流并发数量；

-R, --reverse，反向模式运行（Server 端发送，Client 端接收）；

-w, --window #[KMG]，设置套接字缓冲区大小，TCP 模式下为窗口大小；

-C, --congestion ，设置 TCP 拥塞控制算法（仅支持 Linux 和 FreeBSD ）；

-M, --set-mss #，设置 TCP/SCTP 最大分段长度（MSS，MTU 减 40 字节）；

-N, --no-delay，设置 TCP/SCTP no delay，屏蔽 Nagle 算法；

-4, --version4，仅使用 IPv4；

-6, --version6，仅使用 IPv6；

-S, --tos N，设置 IP 服务类型（TOS，Type Of Service）；

-L, --flowlabel N，设置 IPv6 流标签（仅支持 Linux）；

-Z, --zerocopy，使用 “zero copy”（零拷贝）方法发送数据；

-O, --omit N，忽略前 n 秒的测试；

-T, --title str，设置每行测试结果的前缀；

--get-server-output，从 Server 端获取测试结果；

--udp-counters-64bit，在 UDP 测试包中使用 64 位计数器（防止计数器溢出）。
```