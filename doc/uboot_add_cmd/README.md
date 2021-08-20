# 在`boot/common`下创建自己的命令源码文件如`cmd_pwm_test.c`
在`cmd_pwm_test.c`中添加自己的命令源码
eg:
```c
#include <common.h>
#include <command.h>
#include "MsTypes.h"
#include "../drivers/mstar/pwm/mdrv_pwm.h"

static int do_pwm_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong id;
	ulong val;
	if (argc != 6)
		return CMD_RET_USAGE;

	id = simple_strtoul(argv[1], NULL, 10);
	DrvPWMInit(id);

	val = simple_strtoul(argv[2], NULL, 10);
	DrvPWMPadSet(id, val);

	val = simple_strtoul(argv[3], NULL, 10);
	DrvPWMSetPeriod(id, val);

	val = simple_strtoul(argv[4], NULL, 10);
	DrvPWMSetDuty(id, val);

	val = simple_strtoul(argv[5], NULL, 10);
	DrvPWMEnable(id, val);
	return 0;
}

U_BOOT_CMD(
	pwm_test ,    6,    1,     do_pwm_test,
	"pwm_test 2 11 500 50 1",
	"N\n"
);
```
# 修改`boot/common/Makefile`
添加自己对应的源码文件
如`cmd_pwm_test.c`则对应`cmd_pwm_test.o`
eg:
```makefile
#test
obj-y += cmd_pwm_test.o
```
# 测试
之后重新编译uboot，烧录至板子即可。