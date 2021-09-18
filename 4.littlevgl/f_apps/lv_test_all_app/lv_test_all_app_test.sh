#!/bin/sh
# 快速预览所有demo
# 每个demo预览 0.5秒 eg:./lv_test_all_app_test.sh 0.5
if [ $# != 1 ]; then
    echo "Error: eg:./lv_test_all_app_test.sh 0.5 #表示每个demo预览 0.5秒 "
    exit 1
fi

for i in $(seq 0 1000)
do   
    echo $i
    ./lv_test_all_app 1024 600 $i &
    sleep $1
    pkill lv_test_all_app
done  