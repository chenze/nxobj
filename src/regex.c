#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include "nxobj.h"

nx_obj *regex_match_all(char *pattern, char *str, size_t nmatch) {
    int r;
    regex_t preg;
    if ((r = regcomp(&preg, pattern, REG_EXTENDED))) {
         char errbuf[1024];
         regerror(r, &preg, errbuf, sizeof(errbuf));
         //printf("error: %s\n", errbuf);
         return NULL;
    }

    regmatch_t *match;
    match = malloc(sizeof(regmatch_t)*nmatch);

    nx_obj *all_matches=nx_arr_init();
    nx_obj *current_match;
    int n;

    while (1) {
        //fprintf(stdout, "Go\n");
        //fprintf(stdout, "Go%d\n",strlen(str));
        int r2=regexec(&preg, str, nmatch , match, 0);
        //fprintf(stdout, "Gone\n");
        if (r2==REG_NOMATCH) {
            break;
        }
        current_match=nx_arr_init();
        for (n=0;n<nmatch;++n) {
            nx_arr_push_str(current_match,
                            str+match[n].rm_so,
                            match[n].rm_eo-match[n].rm_so);
            //printf("n:%d,%d,%d,%lld,%lld\n",n,r2,REG_NOMATCH,match[n].rm_so,match[n].rm_eo);
        }
        nx_arr_push(all_matches, current_match);
        nx_obj_free(current_match);
        str+=match[0].rm_eo;
    }
    free(match);
    return all_matches;
}
