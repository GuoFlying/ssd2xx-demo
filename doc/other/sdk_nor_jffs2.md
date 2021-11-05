<!--
 * @Author: Flying
 * @Date: 2021-11-05 15:24:33
 * @LastEditors: Flying
 * @LastEditTime: 2021-11-05 15:31:49
 * @Description: file content
-->

# 背景

- 公板 SDK编译后，customer、config分区内容无法更改
- 原因是：分区格式配置了为 squashfs 格式，该格式为只读，修改成jffs2即可

## V30 修改分区配置文件

- 修改 `project`下的 `image/configs/i2m/nor.squashfs.p2.partition.boot.config`文件
- V30 版本SDK修改差异如下所示
- 修改后，公版默认的config放的东西较多，所以需要修改下config的分区大小
  
```shell
 miservice$(RESOUCE)   = $(OUTPUTDIR)/miservice/config
-miservice$(FSTYPE)    = squashfs
-miservice$(PATSIZE)   = 0x00370000
+miservice$(FSTYPE)    = jffs2
+miservice$(PATSIZE)   = 0x00670000
 miservice$(MOUNTTG)   = /config
-miservice$(MOUNTPT)   = /dev/mtdblock5
-miservice$(OTABLK)    = $(miservice$(MOUNTPT))
+miservice$(MOUNTPT)   = mtd:miservice
+miservice$(OTABLK)    = /dev/mtdblock5
 
 customer$(RESOUCE)   = $(OUTPUTDIR)/customer
-customer$(FSTYPE)    = squashfs
-customer$(PATSIZE)   = 0x00770000
+customer$(FSTYPE)    = jffs2
+customer$(PATSIZE)   = 0x00470000
 customer$(MOUNTTG)   = /customer
-customer$(MOUNTPT)   = /dev/mtdblock6
-customer$(OTABLK)   = $(customer$(MOUNTPT))
+customer$(MOUNTPT)   = mtd:customer
+customer$(OTABLK)   = /dev/mtdblock6

```

## 完整文件如下

- image/configs/i2m/nor.squashfs.p2.partition.boot.config

```shell
BOOT_IMAGE_LIST = ipl ipl_cust mxp uboot
IMAGE_LIST = boot kernel rootfs miservice customer appconfigs logo
OTA_IMAGE_LIST = boot kernel logo miservice customer appconfigs

FLASH_TYPE = nor 
BENCH = no
PHY_TEST = no
PAT_TABLE  = spi 

TFTPDOWNLOADADDR:=0x21000000
KERNELBOOTADDR:=0x22000000
INITRAMFSLOADADDR:=0x23000000

USR_MOUNT_BLOCKS:=miservice customer appconfigs
ENV_CFG = /dev/mtd0 0x6E000 0x1000 0x1000 1
ENV_CFG1 = /dev/mtd0 0x6F000 0x1000 0x1000 1

ipl$(RESOUCE) = $(PROJ_ROOT)/board/$(CHIP)/boot/ipl/IPL.bin
ipl$(PATSIZE) = 0x10000

ipl_cust$(RESOUCE) = $(PROJ_ROOT)/board/$(CHIP)/boot/ipl/IPL_CUST.bin
ipl_cust$(PATSIZE) = 0x10000

mxp$(RESOUCE) = $(IMAGEDIR)/boot/MXP_SF.bin
mxp$(PATSIZE) = 0x10000

mxp$(RESOUCE) = $(IMAGEDIR)/boot/MXP_SF.bin
mxp$(PATSIZE) = 0x10000
mxp$(BOOTTAB) = "$(ipl$(PATSIZE))(IPL),$(ipl_cust$(PATSIZE))(IPL_CUST),$(mxp$(PATSIZE))(MXPT),$(uboot$(PATSIZE))(UBOOT),0x2000(UBOOT_ENV)"
mxp$(SYSTAB) = "$(kernel$(PATSIZE))(KERNEL),$(key_cust$(PATSIZE))(KEY_CUST),$(logo$(PATSIZE))(LOGO),$(rootfs$(PATSIZE))(rootfs),$(miservice$(PATSIZE))(miservice),$(customer$(PATSIZE))(customer),$(appconfigs$(PATSIZE))(appconfigs)"

uboot$(RESOUCE) = $(PROJ_ROOT)/board/$(CHIP)/boot/$(FLASH_TYPE)/uboot/u-boot.xz.img.bin
uboot$(PATSIZE) = 0x3E000

boot$(RESOUCE) = $(IMAGEDIR)/boot.bin
boot$(PATSIZE) = $(shell printf 0x%x $(shell stat -c "%s" $(boot$(RESOUCE))))
boot$(OTABLK)  = /dev/mtdblock0

kernel$(RESOUCE)   = $(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/bin/kernel/$(FLASH_TYPE)/uImage.xz
kernel$(PATSIZE)   = 0x200000
kernel$(BOOTENV)   = $(KERNEL_BOOT_ENV)
kernel$(BOOTCMD)   = sf read $(KERNELBOOTADDR) \$${sf_kernel_start} \$${sf_kernel_size}\;
kernel$(OTABLK) = /dev/mtdblock1

key_cust$(PATSIZE) = 0x10000

logo$(RESOUCE) = $(IMAGEDIR)/logo
logo$(PATSIZE) = 0x20000
logo$(OTABLK) = /dev/mtdblock3

wifi24mclkcmd = mw 1f001cc0 11
wifirstoffcmd = gpio out 8 0
wifirstoncmd = gpio out 8 1

rootfs$(RESOUCE)   = $(OUTPUTDIR)/rootfs
rootfs$(FSTYPE)    = squashfs
rootfs$(PATSIZE)   = 0x1C0000
rootfs$(BOOTENV)   = root=/dev/mtdblock4 rootfstype=squashfs ro init=/linuxrc

miservice$(RESOUCE)   = $(OUTPUTDIR)/miservice/config
miservice$(FSTYPE)    = jffs2
miservice$(PATSIZE)   = 0x00670000
miservice$(MOUNTTG)   = /config
miservice$(MOUNTPT)   = mtd:miservice
miservice$(OTABLK)    = /dev/mtdblock5

customer$(RESOUCE)   = $(OUTPUTDIR)/customer
customer$(FSTYPE)    = jffs2
customer$(PATSIZE)   = 0x00470000
customer$(MOUNTTG)   = /customer
customer$(MOUNTPT)   = mtd:customer
customer$(OTABLK)   = /dev/mtdblock6

appconfigs$(RESOUCE)   = $(OUTPUTDIR)/appconfigs
appconfigs$(FSTYPE)    = jffs2
appconfigs$(PATSIZE)   = 0x00C0000
appconfigs$(MOUNTTG)   = /appconfigs
appconfigs$(MOUNTPT)   = mtd:appconfigs
appconfigs$(OTABLK)    = /dev/mtdblock7

```
