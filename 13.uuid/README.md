# 概要

- UUID  芯片唯一标识
- 原厂提供三种读取UUID方式，可见官方文档
- UUID为6字节，mac设置需要6个字节数据，刚好设置成MAC，为小项目省去专门设置MAC的时间
- **以下三个DEMO参考原厂文档，读取UUID,并设置为eth0的mac地址**

## shell_uuid_to_mac

- 利用SDK 自带的riu_r/riu_w寄存器写入工具直接读取UUID，并设置成eth0的MAC
- 脚本形式，可直接在公板上运行
- 需要用到awk、ifconfig、grep 命令

## base_c_uuid_read

- 不使用MI函数，直接使用基础C读取寄存器中的UUID
- 读取后，并设置成eth0的MAC

## mi_sys_uuid_read

- 使用MI函数读取UUID
- 读取后，并设置成eth0的MAC

## 其他

- 经过验证，该DEMO，直接通过UUID设置的MAC可能会设置失败
- 处理方式可见 https://blog.csdn.net/qq_33611327/article/details/78357755