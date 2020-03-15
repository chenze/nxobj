#import <Foundation/Foundation.h>

@interface NSDictionary (NSDictionary_nxobj)
+ (NSDictionary *)dictionaryWithNXObj:(nx_obj *)obj;

- (nx_obj *)copyNXObj;
@end;

@interface NSArray (NSArray_nxobj)
+ (NSArray *)arrayWithNXObj:(nx_obj *)obj;

- (nx_obj *)copyNXObj;
@end;