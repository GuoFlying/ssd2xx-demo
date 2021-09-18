# valgrind 简介
- algrind是一套Linux下，开放源代码（GPL V2）的仿真调试工具的集合。Valgrind由内核（core）以及基于内核的其他调试工具组成。内核类似于一个框架（framework），它模拟了一个CPU环境，并提供服务给其他工具；而其他工具则类似于插件 (plug-in)，利用内核提供的服务完成各种特定的内存调试任务（内存泄漏检测工具valgrind神器 - 知乎 https://zhuanlan.zhihu.com/p/75416381）
- 官网 https://www.valgrind.org/
- 用户手册 	https://www.valgrind.org/docs/manual/valgrind_manual.html.tar.bz2
- 源码下载 https://sourceware.org/pub/valgrind/valgrind-3.17.0.tar.bz2 （最新release版本：2021-08-20）
- 2.2 命令介绍
   - 用法:valgrind[options] prog-and-args [options]: 常用选项，适用于所有Valgrind工具
   - -tool=<name> 最常用的选项。运行 valgrind中名为toolname的工具。默认memcheck。
   - h –help 显示帮助信息。
   - -version 显示valgrind内核的版本，每个工具都有各自的版本。
   - q –quiet 安静地运行，只打印错误信息。
   - v –verbose 更详细的信息, 增加错误数统计。
   - -trace-children=no|yes 跟踪子线程? [no]
   - -track-fds=no|yes 跟踪打开的文件描述？[no]
   - -time-stamp=no|yes 增加时间戳到LOG信息? [no]
   - -log-fd=<number> 输出LOG到描述符文件 [2=stderr]
   - -log-file=<file> 将输出的信息写入到filename.PID的文件里，PID是运行程序的进行ID
   - -log-file-exactly=<file> 输出LOG信息到 file
   - -log-file-qualifier=<VAR> 取得环境变量的值来做为输出信息的文件名。 [none]
   - -log-socket=ipaddr:port 输出LOG到socket ，ipaddr:port
   - LOG信息输出:
      - -xml=yes 将信息以xml格式输出，只有memcheck可用
      - -num-callers=<number> show <number> callers in stack traces [12]
      - -error-limit=no|yes 如果太多错误，则停止显示新错误? [yes]
      - -error-exitcode=<number> 如果发现错误则返回错误代码 [0=disable]
      - -db-attach=no|yes 当出现错误，valgrind会自动启动调试器gdb。[no]
      - -db-command=<command> 启动调试器的命令行选项[gdb -nw %f %p]
   - 适用于Memcheck工具的相关选项：
      - -leak-check=no|summary|full 要求对leak给出详细信息? [summary]
      - -leak-resolution=low|med|high how much bt merging in leak check [low]
      - -show-reachable=no|yes show reachable blocks in leak check? [no]
# ubuntu 环境搭建
- 下载好valgrind依次执行如下命令安装
```shell
tar -xvf valgrind-3.17.0.tar.bz2 
cd valgrind-3.17.0/
./configure
sudo make -j8
sudo make install
```
- 执行` valgrind --version`命令查看是否安装成功

# 测试用例
### 1. 内存越界，内存泄漏
- 测试代码
```c
  1 #include <stdio.h>
  2 #include <stdlib.h>
  3 void func()
  4 {
  5         char *a= malloc(8);
  6         a[8] = 9; //内存越界
  7 }//内存泄漏
  8 
  9 int main(int argc, char *argv[])
 10 {
 11         func();
 12         return 0;
 13 }
```
- 编译测试
```shell
# -g ：表示使用gdb，在valgrind时才能现实出具体出异常位置
gcc main.c -o main -g
# --tool=memcheck ： 表示使用内存检测工具
# --leak-check=full ： 表示所有泄露检查
valgrind --tool=memcheck --leak-check=full ./main
```
- 输出结果如下
   - `Invalid write of size 1`表示内存越界一个字节，以下的at可跟踪到具体是那个位置出现的问题`at 0x10916B: func (main.c:6)`在man.c文件的第六行
   - `8 bytes in 1 blocks are definitely lost in loss record 1 of 1`表示申请的内存丢失了（内存泄漏），后面可看到申请内存的位置
   - `HEAP SUMMARY` heap总结
   - `LEAK SUMMARY` 泄漏总结
```c
==54392== Memcheck, a memory error detector
==54392== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==54392== Using Valgrind-3.17.0 and LibVEX; rerun with -h for copyright info
==54392== Command: ./main
==54392== 
==54392== Invalid write of size 1
==54392==    at 0x10916B: func (main.c:6)
==54392==    by 0x10918D: main (main.c:11)
==54392==  Address 0x4a5a048 is 0 bytes after a block of size 8 alloc'd
==54392==    at 0x483C855: malloc (vg_replace_malloc.c:380)
==54392==    by 0x10915E: func (main.c:5)
==54392==    by 0x10918D: main (main.c:11)
==54392== 
==54392== 
==54392== HEAP SUMMARY:
==54392==     in use at exit: 8 bytes in 1 blocks
==54392==   total heap usage: 1 allocs, 0 frees, 8 bytes allocated
==54392== 
==54392== 8 bytes in 1 blocks are definitely lost in loss record 1 of 1
==54392==    at 0x483C855: malloc (vg_replace_malloc.c:380)
==54392==    by 0x10915E: func (main.c:5)
==54392==    by 0x10918D: main (main.c:11)
==54392== 
==54392== LEAK SUMMARY:
==54392==    definitely lost: 8 bytes in 1 blocks
==54392==    indirectly lost: 0 bytes in 0 blocks
==54392==      possibly lost: 0 bytes in 0 blocks
==54392==    still reachable: 0 bytes in 0 blocks
==54392==         suppressed: 0 bytes in 0 blocks
==54392== 
==54392== For lists of detected and suppressed errors, rerun with: -s
==54392== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```
### 2. 内存释放后还进行读取
- 测试代码
```c
  1 #include <stdio.h>
  2 #include <stdlib.h>
  3 void func()
  4 {
  5     char *a= malloc(1);
  6     *a = '1';
  7     printf("%c\r\n",*(1+a));
  8     free(a);
  9     printf("%c\r\n",*a);
 10 }
 11 
 12 int main(int argc, char *argv[])
 13 {
 14         func();
 15         return 0;
 16 }
```
- 编译测试
```shell
# -g ：表示使用gdb，在valgrind时才能现实出具体出异常位置
gcc main.c -o main -g
# --tool=memcheck ： 表示使用内存检测工具
# --leak-check=full ： 表示所有泄露检查
valgrind --tool=memcheck --leak-check=full ./main
```
- 输出结果如下
   - `Invalid read of size 1`表示内存释放掉了之后，还去读取(或是越界读)，之后也可看到具体异常位置
```c
==54947== Memcheck, a memory error detector
==54947== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==54947== Using Valgrind-3.17.0 and LibVEX; rerun with -h for copyright info
==54947== Command: ./main
==54947== 
==54947== Invalid read of size 1
==54947==    at 0x1091B2: func (main.c:7)
==54947==    by 0x109213: main (main.c:14)
==54947==  Address 0x4a5a041 is 0 bytes after a block of size 1 alloc'd
==54947==    at 0x483C855: malloc (vg_replace_malloc.c:380)
==54947==    by 0x10919E: func (main.c:5)
==54947==    by 0x109213: main (main.c:14)
==54947== 

==54947== Invalid read of size 1
==54947==    at 0x1091DB: func (main.c:9)
==54947==    by 0x109213: main (main.c:14)
==54947==  Address 0x4a5a040 is 0 bytes inside a block of size 1 free'd
==54947==    at 0x483F0C3: free (vg_replace_malloc.c:755)
==54947==    by 0x1091D6: func (main.c:8)
==54947==    by 0x109213: main (main.c:14)
==54947==  Block was alloc'd at
==54947==    at 0x483C855: malloc (vg_replace_malloc.c:380)
==54947==    by 0x10919E: func (main.c:5)
==54947==    by 0x109213: main (main.c:14)
==54947== 
1
==54947== 
==54947== HEAP SUMMARY:
==54947==     in use at exit: 0 bytes in 0 blocks
==54947==   total heap usage: 2 allocs, 2 frees, 1,025 bytes allocated
==54947== 
==54947== All heap blocks were freed -- no leaks are possible
==54947== 
==54947== For lists of detected and suppressed errors, rerun with: -s
==54947== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

# 交叉编译方式
- 以SSD20x V030版本为例

### 解压、配置编译
```shell
# 
tar -xvf valgrind-3.17.0.tar.bz2
cd valgrind-3.17.0/
mkdir build
# vim 修改如下armv7成arm，如下
vim configure
./configure CC=arm-linux-gnueabihf-gcc --host=arm-linux --prefix=$PWD/build
make -j8
make install 
```
- vim configure 修改如下
   - 修改第5968行为如下所示
```makefile
 5968      arm*)
 5969     { $as_echo "$as_me:${as_lineno-$LINENO}: result: ok (${host_cpu})" >&5
 5970 $as_echo "ok (${host_cpu})" >&6; }
 5971     ARCH_MAX="arm"
 5972     ;;

```
### 报错说明
- 执行时提示`valgrind: failed to start tool 'memcheck' for platform 'arm-linux': Not a directory`错误是因为找不到lib库，使用时需要额外设置libexec的路径，如下所示
- libexec太大了，这边不上传，有需要的自行交叉编译
```
# 设置libexec环境变量位置，否则无法使用。需要根据自己板子的位置，自行修改
export VALGRIND_LIB="/vendor/res/libexec/valgrind"
# 测试 ，能正常打印出版本好才行。
./valgrind --version
```

-  版本可正常打印，但之后测试命令`/valgrind ls -l`后会会报错，如下所示
```
==870== Memcheck, a memory error detector
==870== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==870== Using Valgrind-3.17.0 and LibVEX; rerun with -h for copyright info
==870== Command: ../../main
==870== 

valgrind:  Fatal error at startup: a function redirection
valgrind:  which is mandatory for this platform-tool combination
valgrind:  cannot be set up.  Details of the redirection are:
valgrind:  
valgrind:  A must-be-redirected function
valgrind:  whose name matches the pattern:      index
valgrind:  in an object with soname matching:   ld-linux-armhf.so.3
valgrind:  was not found whilst processing
valgrind:  symbols from the object with soname: ld-linux-armhf.so.3
valgrind:  
valgrind:  Possible fixes: (1, short term): install glibc's debuginfo
valgrind:  package on this machine.  (2, longer term): ask the packagers
valgrind:  for your Linux distribution to please in future ship a non-
valgrind:  stripped ld.so (or whatever the dynamic linker .so is called)
valgrind:  that exports the above-named function using the standard
valgrind:  calling conventions for this platform.  The package you need
valgrind:  to install for fix (1) is called
valgrind:  
valgrind:    On Debian, Ubuntu:                 libc6-dbg
valgrind:    On SuSE, openSuSE, Fedora, RHEL:   glibc-debuginfo
valgrind:  
valgrind:  Note that if you are debugging a 32 bit process on a
valgrind:  64 bit system, you will need a corresponding 32 bit debuginfo
valgrind:  package (e.g. libc6-dbg:i386).
valgrind:  
valgrind:  Cannot continue -- exiting now.  Sorry.

```
- 以上错误是因为没有libc6-dbg，所以需要再次安装libc6-dbg，也就需要gdb，后续有使用到gdb后在进行移植。

