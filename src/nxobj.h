#ifndef NXOBJ
#define NXOBJ 1

#include <inttypes.h>
#include <stdbool.h>
#include <regex.h>

#define NX_TYPE_STR 1
#define NX_TYPE_INT 2
#define NX_TYPE_ARR 3
#define NX_TYPE_DICT 4
#define NX_TYPE_PTR 9 
#define nx_result_t bool

typedef struct _nx_arr {
    int64_t count;
    void **objs;//nx_obj **
} nx_arr;

typedef struct _nx_dict_pair_t {
    char *key;
    void *value;//nx_obj *
    struct _nx_dict_pair_t *first;
    struct _nx_dict_pair_t *next;
} nx_dict_pair_t;

typedef struct _nx_dict_t {
    int64_t count;
    nx_dict_pair_t *pair;
} nx_dict_t;

typedef union _nx_value {
    struct {
        char *val;
        int64_t len;
    } s;
    int64_t i;
    void *ptr;
    nx_arr arr;
    nx_dict_t dict;
} nx_value;

typedef struct _nx_obj {
    int8_t type;
    int32_t refcount;
    nx_value value;
} nx_obj;


nx_obj *nx_obj_init(void);
nx_obj *nx_arr_init(void);
nx_obj *nx_str_init(const char *str, int64_t len);
nx_obj *nx_int_init(int64_t i);
nx_obj *nx_ptr_init(void *ptr, int64_t len);
nx_obj *nx_dict_init(void);

void nx_obj_retain(nx_obj *obj);


bool nx_is_arr(nx_obj *obj);
bool nx_is_str(nx_obj *obj);
bool nx_is_int(nx_obj *obj);
bool nx_is_ptr(nx_obj *obj);
bool nx_is_dict(nx_obj *obj);


int64_t  nx_arr_count(nx_obj *arr);
nx_obj  *nx_arr_obj(nx_obj *arr, int64_t index);
char    *nx_arr_str(nx_obj *arr, int64_t index);
int64_t  nx_arr_strlen(nx_obj *arr, int64_t index);
void    *nx_arr_ptr(nx_obj *arr, int64_t index);

nx_result_t nx_arr_push(nx_obj *arr, nx_obj *obj);
nx_result_t nx_arr_push_str(nx_obj *arr, const char *str, int64_t len);
nx_result_t nx_arr_push_ptr(nx_obj *arr, void *str, int64_t len);

void nx_obj_free(nx_obj *obj);


char *nx_str(nx_obj *obj);
int64_t nx_strlen(nx_obj *obj);
int64_t nx_int(nx_obj *obj);
void *nx_ptr(nx_obj *obj);

nx_result_t nx_dict_add(nx_obj *dict, const char *key, nx_obj *value);
nx_result_t nx_dict_add_str(nx_obj *dict, const char *key, const char *str, int64_t len);
nx_obj *nx_dict(nx_obj *dict, const char *key);
char *nx_dict_str(nx_obj *dict, const char *key);
int64_t nx_dict_strlen(nx_obj *dict, const char *key);
const char *nx_dict_key_at(nx_obj *dict, int64_t index);
nx_obj *nx_dict_value_at(nx_obj *dict, int64_t index);
int64_t nx_dict_count(nx_obj *dict);

nx_dict_pair_t *nx_dict_pair_add(nx_dict_pair_t *dict,const char *key, void *value);
nx_dict_pair_t *nx_dict_pair_remove(nx_dict_pair_t *dict,const char *key);
nx_dict_pair_t *nx_dict_pair_get_by_key(nx_dict_pair_t *dict,const char *key);
nx_result_t nx_dict_pair_free(nx_dict_pair_t *dict);
char *nx_join(nx_obj *obj,const char *obj_delimiter,const char *pair_delimiter);

void nx_dump_with_depth(nx_obj *obj, int depth);
void nx_dump(nx_obj *obj);

#define NX_CHECK_USAGE 1

#ifdef NX_CHECK_USAGE
void nx_print_usage(void);
#endif

/*Regex*/
nx_obj *regex_match_all(char *pattern, char *str, size_t nmatch);
#endif
