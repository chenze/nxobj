#include "nxobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef NX_CHECK_USAGE
int64_t nx_obj_alloc_num = 0;
int64_t nx_obj_dealloc_num = 0;
int64_t nx_str_alloc_num = 0;
int64_t nx_str_dealloc_num = 0;
int64_t nx_ptr_alloc_num = 0;
int64_t nx_ptr_dealloc_num = 0;
int64_t nx_dict_alloc_num = 0;
int64_t nx_dict_dealloc_num = 0;
#endif


nx_obj *nx_obj_init(void) {
    nx_obj *obj = malloc(sizeof(nx_obj));
#ifdef NX_CHECK_USAGE
    ++nx_obj_alloc_num;
#endif
    obj->refcount = 0;
    return obj;
}

void nx_obj_retain(nx_obj *obj) {
    if (!obj) {
        return;
    }
    ++obj->refcount;
}

bool nx_is_str(nx_obj *obj) {
    if (!obj) {
        return false;
    }
    return obj->type == NX_TYPE_STR;
}

bool nx_is_int(nx_obj *obj) {
    if (!obj) {
        return false;
    }
    return obj->type == NX_TYPE_INT;
}

bool nx_is_arr(nx_obj *obj) {
    if (!obj) {
        return false;
    }
    return obj->type == NX_TYPE_ARR;
}

bool nx_is_dict(nx_obj *obj) {
    if (!obj) {
        return false;
    }
    return obj->type == NX_TYPE_DICT;
}

bool nx_is_ptr(nx_obj *obj) {
    if (!obj) {
        return false;
    }
    return obj->type == NX_TYPE_PTR;
}


nx_obj *nx_arr_init(void) {
    nx_obj *obj = nx_obj_init();
    obj->type = NX_TYPE_ARR;

    obj->value.arr.count = 0;
    obj->value.arr.objs = NULL;

    return obj;
}

nx_result_t nx_arr_push(nx_obj *arr, nx_obj *obj) {
    ++obj->refcount;
    arr->value.arr.objs = realloc(arr->value.arr.objs, (arr->value.arr.count + 1) * sizeof(nx_obj *));
    nx_obj **tmp_obj;
    tmp_obj = (nx_obj **) ((nx_obj **) arr->value.arr.objs + arr->value.arr.count);
    *tmp_obj = obj;
    //(nx_obj *)(arr->value.arr.objs)[arr->value.arr.count]=obj;
    ++arr->value.arr.count;
    return true;
}

nx_result_t nx_arr_push_str(nx_obj *arr, const char *str, int64_t len) {
    nx_obj *obj = nx_str_init(str, len);
    nx_arr_push(arr, obj);
    //convenience method - don't increase refcount
    --obj->refcount;
    return true;
}

nx_result_t nx_arr_push_ptr(nx_obj *arr, void *ptr, int64_t len) {
    nx_obj *obj = nx_ptr_init(ptr, len);
    nx_arr_push(arr, obj);
    --obj->refcount;
    return true;
}

int64_t nx_arr_count(nx_obj *arr) {
    if (nx_is_arr(arr)) {
        return arr->value.arr.count;
    }
    return -1;
}

nx_obj *nx_arr_obj(nx_obj *arr, int64_t index) {
    if (!nx_is_arr(arr)) {
        return NULL;
    }
    if (arr->value.arr.count < (index + 1)) {
        return NULL;
    }
    return ((nx_obj **) arr->value.arr.objs)[index];
}

char *nx_arr_str(nx_obj *arr, int64_t index) {
    nx_obj *obj = nx_arr_obj(arr, index);
    return nx_str(obj);
}

int64_t nx_arr_strlen(nx_obj *arr, int64_t index) {
    nx_obj *obj = nx_arr_obj(arr, index);
    return nx_strlen(obj);
}

void *nx_arr_ptr(nx_obj *arr, int64_t index) {
    nx_obj *obj = nx_arr_obj(arr, index);
    return nx_ptr(obj);
}

nx_obj *nx_str_init(const char *str, int64_t len) {
    nx_obj *obj = nx_obj_init();

    if (len < 0) {
        len = strlen(str);
    }

    obj->type = NX_TYPE_STR;

    obj->value.s.val = malloc(len + 1);
#ifdef NX_CHECK_USAGE
    ++nx_str_alloc_num;
#endif
    bzero(obj->value.s.val, len + 1);
    strncpy(obj->value.s.val, str, len);

    obj->value.s.len = len;

    return obj;
}


void *nx_ptr(nx_obj *obj) {
    if (nx_is_ptr(obj)) {
        return obj->value.ptr;
    }
    return NULL;
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
    if (!obj) {
        return;
    }
    if (obj->refcount > 0) {
        --obj->refcount;
        return;
    }
    if (nx_is_str(obj)) {
        if (nx_str(obj)) {
            free(nx_str(obj));
#ifdef NX_CHECK_USAGE
            ++nx_str_dealloc_num;
#endif
        }
    } else if (nx_is_arr(obj)) {
        int64_t c = nx_arr_count(obj);
        int i;
        nx_obj *tmp_obj;

        for (i = 0; i < c; ++i) {
            tmp_obj = nx_arr_obj(obj, i);
            nx_obj_free(tmp_obj);
        }
    } else if (nx_is_ptr(obj)) {
        if (nx_ptr(obj)) {
            free(nx_ptr(obj));
#ifdef NX_CHECK_USAGE
            ++nx_ptr_dealloc_num;
#endif
        }
    } else if (nx_is_dict(obj)) {
        nx_dict_pair_free(obj->value.dict.pair);
    }
    free(obj);
#ifdef NX_CHECK_USAGE
    ++nx_obj_dealloc_num;
#endif
}


nx_obj *nx_dict_init(void) {
    nx_obj *obj = nx_obj_init();
    obj->type = NX_TYPE_DICT;

    obj->value.dict.count = 0;
    obj->value.dict.pair = NULL;

    return obj;
}

nx_result_t nx_dict_add(nx_obj *dict, const char *key, nx_obj *value) {
    ++value->refcount;
    nx_dict_pair_t *old_pair = nx_dict_pair_get_by_key(dict->value.dict.pair, key);
    if (!old_pair) {
        ++dict->value.dict.count;
    }
    dict->value.dict.pair = nx_dict_pair_add(dict->value.dict.pair, key, value);
    return true;
}

nx_result_t nx_dict_add_str(nx_obj *dict, const char *key, const char *str, int64_t len) {
    nx_obj *value = nx_str_init(str, len);
    //convenience method - don't increase refcount
    --value->refcount;
    return nx_dict_add(dict, key, value);
}

nx_obj *nx_dict(nx_obj *dict, const char *key) {
    if (!dict || !key) {
        return NULL;
    }
    nx_dict_pair_t *pair = nx_dict_pair_get_by_key(dict->value.dict.pair, key);
    if (!pair) {
        return NULL;
    }
    return (nx_obj *) pair->value;
}

char *nx_dict_str(nx_obj *dict, const char *key) {
    nx_dict_pair_t *pair = nx_dict_pair_get_by_key(dict->value.dict.pair, key);
    if (!pair) {
        return NULL;
    }
    return nx_str((nx_obj *) pair->value);
}

int64_t nx_dict_strlen(nx_obj *dict, const char *key) {
    nx_dict_pair_t *pair = nx_dict_pair_get_by_key(dict->value.dict.pair, key);
    return nx_strlen((nx_obj *) pair->value);
}

int64_t nx_dict_count(nx_obj *dict) {
    if (!dict || !nx_is_dict(dict)) {
        return -1;
    }
    return dict->value.dict.count;
}

const char *nx_dict_key_at(nx_obj *dict, int64_t index) {
    long i = 0;
    nx_dict_pair_t *current_dict = dict->value.dict.pair;
    while (current_dict) {
        if (i == index) {
            return current_dict->key;
        }
        ++i;
        current_dict = current_dict->next;
    }
    return NULL;
}

nx_obj *nx_dict_value_at(nx_obj *dict, int64_t index) {
    long i = 0;
    nx_dict_pair_t *current_dict = dict->value.dict.pair;
    nx_obj *tmp_obj;
    while (current_dict) {
        if (i == index) {
            tmp_obj = (nx_obj *) current_dict->value;
            return tmp_obj;
        }
        ++i;
        current_dict = current_dict->next;
    }
    return NULL;
}

nx_dict_pair_t *nx_dict_pair_add(nx_dict_pair_t *dict, const char *key, void *value) {
    nx_dict_pair_t *first_dict = dict, *pre_dict = NULL;
    while (dict) {
        //found existing to replace
        if (!strcmp(dict->key, key)) {
            dict->value = value;
            nx_obj_free((nx_obj *) value);
            return dict->first;
        }
        pre_dict = dict;
        dict = dict->next;
    }

    dict = malloc(sizeof(nx_dict_pair_t));
    dict->key = strdup(key);
    dict->value = value;
#ifdef NX_CHECK_USAGE
    ++nx_dict_alloc_num;
#endif

    dict->next = NULL;
    if (pre_dict) {
        dict->first = first_dict;
        pre_dict->next = dict;
    } else {
        dict->first = dict;
    }
    return dict->first;
}

nx_dict_pair_t *nx_dict_pair_remove(nx_dict_pair_t *dict, const char *key) {
    nx_dict_pair_t *next_one = NULL, *pre_one = NULL, *new_first = NULL, *first = NULL;
    if (dict && dict->first) {
        first = dict->first;
    }
    while (dict) {
        next_one = dict->next;
        if (!strcmp(dict->key, key)) {
            free(dict->key);
            free(dict);
            if (pre_one) {
                if (next_one) {
                    pre_one->next = next_one;
                } else {
                    pre_one->next = NULL;
                }
                /*remove 1st item*/
            } else {
                first = NULL;
                new_first = dict->next;
            }
        } else {
            if (new_first) {
                first = new_first;
                dict->first = new_first;
            }
            pre_one = dict;
        }
        dict = next_one;
    }
    return first;
}

nx_dict_pair_t *nx_dict_pair_get_by_key(nx_dict_pair_t *dict, const char *key) {
    nx_dict_pair_t *ret_dict = NULL;
    while (dict) {
        if (!strcmp(dict->key, key)) {
            return dict;
        }
        dict = dict->next;
    }
    return ret_dict;
}

nx_result_t nx_dict_pair_free(nx_dict_pair_t *dict) {
    if (!dict) {
        return true;
    }
    nx_dict_pair_t *current_dict;
    dict = dict->first;
    while (dict) {
        current_dict = dict;
        dict = dict->next;
        free(current_dict->key);
        nx_obj_free(current_dict->value);
        free(current_dict);
#ifdef NX_CHECK_USAGE
        ++nx_dict_dealloc_num;
#endif
    }
    return true;
}

void nx_dump_with_depth(nx_obj *obj, int depth) {
    int i;
    if (obj == NULL) {
        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "NULL");
    } else if (nx_is_str(obj)) {
        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "STR(%lld): \"", nx_strlen(obj));
        fwrite(nx_str(obj), nx_strlen(obj), 1, stdout);
        fprintf(stdout, "\"");
    } else if (nx_is_arr(obj)) {
        int64_t c = nx_arr_count(obj);
        int i;
        nx_obj *tmp_obj;

        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "ARR(%llu)[", c);
        for (i = 0; i < c; ++i) {
            tmp_obj = nx_arr_obj(obj, i);
            nx_dump_with_depth(tmp_obj, depth + 1);
        }
        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "]");
    } else if (nx_is_ptr(obj)) {
        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "PTR");
    } else if (nx_is_dict(obj)) {
        nx_obj *tmp_obj;

        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "{");
        nx_dict_pair_t *current_dict = obj->value.dict.pair;
        while (current_dict) {
            fprintf(stdout, "\n");
            for (i = 0; i < depth; ++i) {
                fprintf(stdout, " ");
            }
            fprintf(stdout, "\"%s\"=>", current_dict->key);
            tmp_obj = (nx_obj *) current_dict->value;
            nx_dump_with_depth(tmp_obj, depth + 1);
            current_dict = current_dict->next;
        }
        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "}");

    } else {
        fprintf(stdout, "\n");
        for (i = 0; i < depth; ++i) {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "Unknown");
    }
}


nx_obj *nx_ptr_init(void *ptr, int64_t len) {
    nx_obj *obj = nx_obj_init();

    obj->type = NX_TYPE_PTR;

    obj->value.ptr = malloc(len);
    memcpy(obj->value.ptr, ptr, len);
#ifdef NX_CHECK_USAGE
    ++nx_ptr_alloc_num;
#endif

    return obj;
}

char *nx_join(nx_obj *obj, const char *kv_delimiter, const char *obj_delimiter) {
    char *join_string = NULL;
    uint16_t pd_len = strlen(obj_delimiter);
    uint16_t kvd_len = strlen(kv_delimiter);
    uint16_t str_len = 0;
    uint16_t str_pre_len = 0;
    long i, c;

    if (!nx_is_dict(obj)) {
        join_string = malloc(1);
        bzero(join_string, 1);
    } else {
        c = nx_dict_count(obj);
        const char *key;
        const char *value;
        for (i = 0; i < c; ++i) {
            key = nx_dict_key_at(obj, i);
            value = nx_str(nx_dict_value_at(obj, i));

            if (join_string) {
                str_pre_len = str_len;
                str_len += strlen(key) + strlen(value) + pd_len + kvd_len + 1;
                join_string = realloc(join_string, str_len);
                memset(join_string + str_pre_len, 0, str_len - str_pre_len);
                strcat(join_string, obj_delimiter);
            } else {
                str_len += strlen(key) + strlen(value) + kvd_len + 1;
                join_string = malloc(str_len);
                memset(join_string, 0, str_len);
            }
            strcat(join_string, key);
            strcat(join_string, kv_delimiter);
            strcat(join_string, value);
        }

    }

    return join_string;
}


void nx_dump(nx_obj *obj) {
    nx_dump_with_depth(obj, 0);
}

#ifdef NX_CHECK_USAGE

void nx_print_usage(void) {
    fprintf(stdout, "obj alloc: %lld, dealloc:%lld, left:%lld\n", nx_obj_alloc_num, nx_obj_dealloc_num,
            nx_obj_alloc_num - nx_obj_dealloc_num);
    fprintf(stdout, "str alloc: %lld, dealloc:%lld, left:%lld\n", nx_str_alloc_num, nx_str_dealloc_num,
            nx_str_alloc_num - nx_str_dealloc_num);
    fprintf(stdout, "ptr alloc: %lld, dealloc:%lld, left:%lld\n", nx_ptr_alloc_num, nx_ptr_dealloc_num,
            nx_ptr_alloc_num - nx_ptr_dealloc_num);
    fprintf(stdout, "dict alloc: %lld, dealloc:%lld, left:%lld\n", nx_dict_alloc_num, nx_dict_dealloc_num,
            nx_dict_alloc_num - nx_dict_dealloc_num);
}

#endif
