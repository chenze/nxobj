void test_regex() {
    char *pattern = "html([0-9]+)";
    char *string  = "jjjjuuuhtml12345pjhtml43";
    nx_obj *matches = regex_mach_all(pattern, string, 2);
    
    int i;
    int c = nx_arr_count(matches);
    nx_obj *match;
    char *matched_str;
    for (i=0;i<c;++i) {
        match=nx_arr_obj(matches, i);
        matched_str = nx_arr_str(match, 1);
        fprintf(stdout, "matched:%s\n", matched_str);
    }
    nx_obj_free(matches);
    nx_print_usage();
}
