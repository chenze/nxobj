#ifndef NXOBJ
#define NXOBJ 1

#include <inttypes.h>
#include <stdbool.h>

#define NX_TYPE_STR 1
#define NX_TYPE_INT 2
#define NX_TYPE_ARR 3

typedef struct _nx_arr {
    int64_t count;
    void **objs;
} nx_arr;

typedef union _nx_value {
    struct {
        char *val;
        int64_t len;
    } s;
    int64_t i;
    nx_arr arr;
} nx_value;

typedef struct _nx_obj {
    int8_t type;
    int32_t refcount;
    nx_value value;
} nx_obj;

nx_obj *nx_obj_init();
nx_obj *nx_arr_init();
int64_t nx_arr_count(nx_obj *arr);
nx_obj *nx_arr_obj(nx_obj *arr, int64_t index);
char *nx_arr_str(nx_obj *arr, int64_t index);
int64_t nx_arr_strlen(nx_obj *arr, int64_t index);

bool nx_arr_push(nx_obj *arr, nx_obj *obj);
bool nx_arr_push_str(nx_obj *arr, char *str, int64_t len);

void nx_obj_free(nx_obj *obj);


bool nx_is_str(nx_obj *obj);
bool nx_is_int(nx_obj *obj);

nx_obj *nx_str_init(char *str, int64_t len);
char *nx_str(nx_obj *obj);
int64_t nx_strlen(nx_obj *obj);

nx_obj *nx_int_init(int64_t i);
int64_t nx_int(nx_obj *obj);

#define NX_CHECK_USAGE 1

#ifdef NX_CHECK_USAGE
void nx_print_usage();
#endif

#endif
