#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nxval.h"

int test_nx_arr() {
    nx_obj *arr = nx_arr_init();

    nx_arr_push_str(arr, "abcdefyyp", 9);
    nx_arr_push_str(arr, "string 2", 8);
    char *str3=malloc(15);
    bzero(str3, 15);
    str3=memcpy(str3, "ajajajajajajaj", 14);
    nx_arr_push_str(arr, str3, 14);
    free(str3);

    int c=nx_arr_count(arr);
    int i=0;
    printf("count:%d\n", c);
    nx_obj *tmp_item;
    for (i=0;i<c;++i) {
        char *str = nx_arr_str(arr, i);
        int strlen = nx_arr_strlen(arr, i);
        printf("str:%s, len:%d\n", str,strlen);
    }
    nx_obj_free(arr);
    return 0;
}
