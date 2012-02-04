#include "nxobj.h"
#import "NS_nxobj.h"

@implementation  NSDictionary (NSDictionary_nxobj)

+ (NSDictionary *)dictionaryWithNXObj:(nx_obj *)obj {
    if (!obj || !nx_is_dict(obj)) {
        return NULL;
    }
    NSMutableDictionary *dict=[NSMutableDictionary dictionary];
    
    
    nx_dict_pair_t *current_dict=obj->value.dict.pair;
    nx_obj *tmp_obj;
    NSString *tmp_key;
    while (current_dict) {
        tmp_obj = (nx_obj *)current_dict->value;
        tmp_key = [NSString stringWithCString:current_dict->key encoding:NSUTF8StringEncoding];
        if (nx_is_str(tmp_obj)) {
            [dict setValue:[NSString stringWithCString:nx_str(tmp_obj) encoding:NSUTF8StringEncoding] forKey:tmp_key];
        } else if (nx_is_arr(tmp_obj)) {
            [dict setValue:[NSArray arrayWithNXObj:tmp_obj] forKey:tmp_key];
        } else if (nx_is_dict(tmp_obj)) {
            [dict setValue:[NSDictionary dictionaryWithNXObj:tmp_obj] forKey:tmp_key];
        } else {
            //
        }
        current_dict=current_dict->next;
    }
    return dict;
}
- (nx_obj *)copyNXObj {
    nx_obj *obj=nx_dict_init();
    NSEnumerator *enumerator = [self keyEnumerator];
    
    id key;
    id value;
    //nx_obj *tmp_obj;
    const char *tmp_str;
    
    while ((key = [enumerator nextObject])) {
        value = [self valueForKey:key];
        if ([value isKindOfClass:[NSString class]]) {
            tmp_str=[value UTF8String];
            nx_dict_add_str(obj, [key UTF8String], tmp_str, strlen(tmp_str));
        } else if ([value isKindOfClass:[NSArray class]]) {
            nx_dict_add(obj, [key UTF8String], [value copyNXObj]);
        } else if ([value isKindOfClass:[NSDictionary class]]) {
            nx_dict_add(obj, [key UTF8String], [value copyNXObj]);
        }
    }
    return obj;
}
@end

@implementation NSArray (NSArray_nxobj)
+ (NSArray *)arrayWithNXObj:(nx_obj *)obj {
    if (!obj || !nx_is_arr(obj)) {
        return NULL;
    }
    NSMutableArray *arr=[NSMutableArray array];
    long c,i;
    c=nx_arr_count(obj);
    nx_obj *tmp_obj;
    for (i=0; i<c; ++i) {
        tmp_obj=nx_arr_obj(obj, i);
        if (nx_is_str(tmp_obj)) {
            [arr addObject:[NSString stringWithCString:nx_str(tmp_obj) encoding:NSUTF8StringEncoding]];
        } else if (nx_is_dict(tmp_obj)) {
            [arr addObject:[NSDictionary dictionaryWithNXObj:tmp_obj]];
        } else if (nx_is_arr(tmp_obj)) {
            [arr addObject:[NSArray arrayWithNXObj:tmp_obj]];
        }
    }
    return arr;
}
- (nx_obj *)copyNXObj {
    nx_obj *obj=nx_arr_init();
    long i,c;
    c=[self count];
    id value;
    const char *tmp_str;
    for (i=0; i<c; ++i) {
        value=[self objectAtIndex:i];
        if ([value isKindOfClass:[NSString class]]) {
            tmp_str=[value UTF8String];
            nx_arr_push_str(obj, tmp_str, strlen(tmp_str));
        } else if ([value isKindOfClass:[NSDictionary class]]) {
            nx_arr_push(obj, [value copyNXObj]);
        } else if ([value isKindOfClass:[NSArray class]]) {
            nx_arr_push(obj, [value copyNXObj]);
        }
    }
    return obj;
}
@end