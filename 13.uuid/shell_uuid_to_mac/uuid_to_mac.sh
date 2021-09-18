#!/bin/sh
# 需要用到awk、ifconfig、grep 命令

if [ ! -f "/config/riu_r" ];then
    echo "/config/riu_r nonexistence"
    exit 1
fi

if [ ! -f "/config/riu_w" ];then
    echo "/config/riu_w nonexistence"
    exit 2
fi

eth0=`ifconfig -a | grep eth0`
if [ -z "$eth0" ];then
    echo "eth0 not fount "
    exit 3
fi

#读bank20 offset 03的值
f_tmp=`/config/riu_r 20 3 | awk '{print $0}' | awk 'NR == 2'`
# 设置第8bit为0 
f_tmp1=`echo $f_tmp | awk '{print and($0,0xfeff)}'`
# 读成16进制数
f_tmp2=`echo $f_tmp1 |  awk '{printf("%0x\n",$0)}'`
# 将第8bit清0后，写入寄存器中
/config/riu_w 20 3 $f_tmp2 >> /dev/null

if [ $? -eq 0 ]; then
    echo "bank20 offset 03 write succeed"
else
    echo "bank20 offset 03 write failed"
fi

mac=""
add_mac_byte(){
    #高位
    offset=$1

    f_tmp=`/config/riu_r 20 $offset | awk '{print $0}' | awk 'NR == 2'`
    f_tmp1=`echo $f_tmp | awk '{print rshift($0,8)}'` #$0 >> 8
    f_tmp2=`echo $f_tmp1 |  awk '{printf("%02x\n",$0)}'`
    if [ "$mac" == "" ];then
        mac=$f_tmp2
    else
        mac=$mac":"$f_tmp2
    fi
    #低位
    f_tmp=`/config/riu_r 20 $offset | awk '{print $0}' | awk 'NR == 2'`
    f_tmp1=`echo $f_tmp | awk '{print and($0,0x00ff)}'`
    f_tmp2=`echo $f_tmp1 |  awk '{printf("%02x\n",$0)}'`
    mac=$mac":"$f_tmp2
}

add_mac_byte 18
add_mac_byte 17
add_mac_byte 16

ifconfig eth0 hw ether $mac
if [ $? -eq 0 ]; then
    echo "write mac success" $mac
else
    echo "write mac faild" $mac
fi

# 查看eth0 mac信息
#ifconfig 
