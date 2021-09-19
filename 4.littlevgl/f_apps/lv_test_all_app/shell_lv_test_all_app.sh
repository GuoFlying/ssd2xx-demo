#!/bin/sh
# 快速预览所有demo
# 每个demo预览 0.5秒 eg:./lv_test_all_app_test.sh 1024 600 0.5
if [ $# != 3 ]; then
    echo "Error: eg:./lv_test_all_app_test.sh 1024 600 0.5 #表示每个demo预览 0.5秒 "
    exit 1
fi

for i in $(seq 0 1000)
do   
    echo $i
    ./lv_test_all_app $1 $2 $i &
    sleep $3
    pkill lv_test_all_app
done  