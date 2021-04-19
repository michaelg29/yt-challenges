#include <stdio.h>
#include <stdlib.h>

#include "json.h"

int main()
{
    printf("Hello, world!\n");

    // json OUT = json_type(JSON_OBJ);

    // json list = json_type(JSON_LIST);
    // json_list_add_string(&list, "str1");
    // json_list_add_float(&list, 1.29f);
    // json_list_add_null(&list);
    // json_obj_put(&OUT, "list1", &list);

    // json list2 = json_type(JSON_LIST);
    // json_list_add_null(&list2);
    // json_obj_put(&OUT, "list2", &list2);

    // json list3 = json_type(JSON_LIST);
    // json_obj_put(&OUT, "list3", &list3);

    // json_obj_put_string(&OUT, "str123", "asdfasdgf");
    // json_obj_put_float(&OUT, "float", 15325.3f);

    json OUT = json_readFile("test.json");

    json_list_get(json_obj_get(&OUT, "list1"), 1)->val.f -= 0.54f;

    int n;
    char *str = json_dump(OUT, true, &n);
    printf("%s\n", str);
    free(str);

    json_freeDeep(&OUT);

    return 0;
}