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
#include "json_parser.h"

int main(int argc, char *argv[])
{
	int result;
    int json_type;
    char error_info[256];
    string_t input;
    string_t output;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <json_string | json_array | json_map>\n",
                argv[0]);
        return EINVAL;
    }
	
	log_init();

    input.str = argv[1];
    input.len = strlen(input.str);
    json_type = detect_json_type(&input);
    if (json_type == FC_JSON_TYPE_ARRAY) {
        json_array_t array;

        if ((result=decode_json_array(&input, &array, error_info,
                        sizeof(error_info))) != 0)
        {
            fprintf(stderr, "decode json array fail, %s\n", error_info);
            return result;
        }
                
        if ((result=encode_json_array(&array, &output,
                            error_info, sizeof(error_info))) != 0)
        {
            fprintf(stderr, "encode json array fail, %s\n", error_info);
            return result;
        }

        printf("%s\n", output.str);
        free_json_string(&output);
        free_json_array(&array);
    } else if (json_type == FC_JSON_TYPE_MAP) {
        json_map_t map;

        if ((result=decode_json_map(&input, &map, error_info,
                        sizeof(error_info))) != 0)
        {
            fprintf(stderr, "decode json map fail, %s\n", error_info);
            return result;
        }
                
        if ((result=encode_json_map(&map, &output,
                            error_info, sizeof(error_info))) != 0)
        {
            fprintf(stderr, "encode json map fail, %s\n", error_info);
            return result;
        }

        printf("%s\n", output.str);
        free_json_string(&output);
        free_json_map(&map);
    } else {
        fprintf(stderr, "string\n");
    }

	return 0;
}
