#include "nxobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef NX_CHECK_USAGE
int64_t nx_obj_alloc_num=0;
int64_t nx_obj_dealloc_num=0;
int64_t nx_str_alloc_num=0;
int64_t nx_str_dealloc_num=0;
#endif

nx_obj *nx_obj_init() {
    nx_obj *obj = malloc(sizeof(nx_obj));
    #ifdef NX_CHECK_USAGE
    ++nx_obj_alloc_num;
    #endif
    obj->refcount=0;
    return obj;
}

bool nx_is_str(nx_obj *obj) {
    return obj->type==NX_TYPE_STR;
}
bool nx_is_int(nx_obj *obj) {
    return obj->type==NX_TYPE_INT;
}
bool nx_is_arr(nx_obj *obj) {
    return obj->type==NX_TYPE_ARR;
}

nx_obj *nx_arr_init() {
    nx_obj *obj = nx_obj_init();
    obj->type=NX_TYPE_ARR;
    
    obj->value.arr.count=0;
    obj->value.arr.objs=NULL;

    return obj;
}
bool nx_arr_push(nx_obj *arr, nx_obj *obj) {
    ++obj->refcount;
    arr->value.arr.objs = realloc(arr->value.arr.objs, (arr->value.arr.count+1)*sizeof(nx_obj *));
    nx_obj **tmp_obj;
    tmp_obj=(nx_obj **)((nx_obj **)arr->value.arr.objs+arr->value.arr.count);
    *tmp_obj=obj;
    //(nx_obj *)(arr->value.arr.objs)[arr->value.arr.count]=obj;
    ++arr->value.arr.count;
    return true;
}
bool nx_arr_push_str(nx_obj *arr, char *str, int64_t len) {
    nx_obj *obj = nx_str_init(str, len);
    nx_arr_push(arr, obj);
    --obj->refcount;
    return true;
}

int64_t nx_arr_count(nx_obj *arr) {
    return arr->value.arr.count;
}

nx_obj *nx_arr_obj(nx_obj *arr, int64_t index) {
    if (!nx_is_arr(arr)) {
        return NULL;
    }
    if (arr->value.arr.count<(index+1)) {
        return NULL;
    }
    return ((nx_obj **)arr->value.arr.objs)[index];
}

char *nx_arr_str(nx_obj *arr, int64_t index) {
    nx_obj *obj = nx_arr_obj(arr, index);
    return nx_str(obj);
}
int64_t nx_arr_strlen(nx_obj *arr, int64_t index) {
    nx_obj *obj = nx_arr_obj(arr, index);
    return nx_strlen(obj);
}

nx_obj *nx_str_init(char *str, int64_t len) {
    nx_obj *obj = nx_obj_init();

    if (len<0) {
        len = strlen(str);
    }

    obj->type=NX_TYPE_STR;

    obj->value.s.val = malloc(len+1);
    #ifdef NX_CHECK_USAGE
    ++nx_str_alloc_num;
    #endif
    bzero(obj->value.s.val, len+1);
    strncpy(obj->value.s.val, str, len);
    
    obj->value.s.len = len;

    return obj;
}

char *nx_str(nx_obj *obj) {
    if (nx_is_str(obj)) {
        return obj->value.s.val;
    }
    return NULL;
}
int64_t nx_strlen(nx_obj *obj) {
    if (nx_is_str(obj)) {
        return obj->value.s.len;
    }
    return -1;
}

void nx_obj_free(nx_obj *obj) {
    if (obj->refcount>0) {
        --obj->refcount;
        return;
    }
    if (nx_is_str(obj)) {
        free(obj->value.s.val);
        //fprintf(stdout, "%s", "str free\n");
        #ifdef NX_CHECK_USAGE
        ++nx_str_dealloc_num;
        #endif
    } else if (nx_is_arr(obj)) {
        int c=obj->value.arr.count;
        int i;
        nx_obj *tmp_obj;
        //fprintf(stdout, "arr count%d\n",c);

        for (i=0;i<c;++i) {
            tmp_obj = nx_arr_obj(obj, i);
            //fprintf(stdout, "go into a free block%u\n",tmp_obj->refcount);
            nx_obj_free(tmp_obj);
        }
    }
    //fprintf(stdout, "%s", "obj free\n");
    free(obj);
    #ifdef NX_CHECK_USAGE
    ++nx_obj_dealloc_num;
    #endif
}

#ifdef NX_CHECK_USAGE
void nx_print_usage() {
    fprintf(stdout, "obj alloc: %lld, dealloc:%lld, left:%lld\n", nx_obj_alloc_num, nx_obj_dealloc_num, nx_obj_alloc_num-nx_obj_dealloc_num);
    fprintf(stdout, "str alloc: %lld, dealloc:%lld, left:%lld\n", nx_str_alloc_num, nx_str_dealloc_num, nx_str_alloc_num-nx_str_dealloc_num);
}
#endif
