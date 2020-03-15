## What is nxobj
`nxobj` is a C library to provide JSON-like structure and operations.

## Examples
```c
nx_obj *obj = nx_dict_init();

nx_obj *sub_dict = nx_dict_init();
nx_dict_add_str(sub_dict, "sub_dict", "123456789", 9);

nx_dict_add(obj, "test_dict", sub_dict);
nx_dict_add_str(obj, "test_str", "12345", 5);
```
[More Examples](./examples)