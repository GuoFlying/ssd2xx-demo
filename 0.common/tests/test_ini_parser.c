/*
 * Copyright (c) 2020 YuQing <384681@qq.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the Lesser GNU General Public License, version 3
 * or later ("LGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "logger.h"
#include "shared_func.h"
#include "ini_file_reader.h"

static int iniAnnotationFuncExpressCalc(IniContext *context,
        struct ini_annotation_entry *annotation, const IniItem *item,
        char **pOutValue, int max_values)
{
    int count;
    int result;
    char cmd[512];
    static char output[256];

    count = 0;
    sprintf(cmd, "echo \'%s\' | bc -l", item->value);
    if ((result=getExecResult(cmd, output, sizeof(output))) != 0)
    {
        logWarning("file: "__FILE__", line: %d, "
                "exec %s fail, errno: %d, error info: %s",
                __LINE__, cmd, result, STRERROR(result));
        return count;
    }
    if (*output == '\0')
    {
        logWarning("file: "__FILE__", line: %d, "
                "empty reply when exec: %s", __LINE__, item->value);
    }
    pOutValue[count++] = fc_trim(output);
    return count;
}

int main(int argc, char *argv[])
{
	int result;
    IniContext context;
    const char *szFilename = "test.ini";
    AnnotationEntry annotations[1];

    if (argc > 1) {
        szFilename = argv[1];
    }
	
	log_init();

    memset(annotations, 0, sizeof(annotations));
    annotations[0].func_name = "EXPRESS_CALC";
    annotations[0].func_get = iniAnnotationFuncExpressCalc;

    //printf("sizeof(IniContext): %d\n", (int)sizeof(IniContext));
    result = iniLoadFromFileEx(szFilename, &context,
            FAST_INI_ANNOTATION_WITH_BUILTIN, annotations, 1,
            FAST_INI_FLAGS_SHELL_EXECUTE);
    if (result != 0)
    {
        return result;
    }

    iniPrintItems(&context);
    iniDestroyAnnotationCallBack();
    iniFreeContext(&context);
	return 0;
}
