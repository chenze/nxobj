#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/nxobj.h"

int test_nx_arr() {
    nx_obj *arr = nx_arr_init();

    nx_arr_push_str(arr, "123456789", 9);
    nx_arr_push_str(arr, "12345678", 8);
    char *str3 = malloc(15);
    bzero(str3, 15);
    str3 = memcpy(str3, "12345678901234", 14);
    nx_arr_push_str(arr, str3, 14);
    free(str3);

    int c = nx_arr_count(arr);
    int i = 0;
    printf("count:%d\n", c);
    for (i = 0; i < c; ++i) {
        char *str = nx_arr_str(arr, i);
        int strlen = nx_arr_strlen(arr, i);
        printf("str:%s, len:%d\n", str, strlen);
    }
    nx_obj_free(arr);
    return 0;
}

void test_dict() {
    nx_obj *obj = nx_dict_init();

    nx_obj *sub_dict = nx_dict_init();
    nx_dict_add_str(sub_dict, "sub_dict", "123456789", 9);

    nx_dict_add(obj, "test_dict", sub_dict);
    nx_dict_add_str(obj, "test_str", "12345", 5);

    nx_dump(obj);
}


int main() {
    test_nx_arr();
    test_dict();
    return 0;
}