#include "dynamicarray.h"
#include "hashmap.h"
#include "strstream.h"

#ifndef JSON_H
#define JSON_H

#define TAB "    "
#define ENDL "\n"

typedef char bool;
#define false 0
#define true !0
#define JSON_TRUE "true"
#define JSON_FALSE "false"

#define JSON_NULL_S "null"

typedef char jsontype;
#define JSON_NULL (jsontype)0
#define JSON_STR (jsontype)1
#define JSON_FLOAT (jsontype)2
#define JSON_INT (jsontype)3
#define JSON_BOOL (jsontype)4
#define JSON_LIST (jsontype)5
#define JSON_OBJ (jsontype)6

union jsonval
{
    void *n;
    char *s;
    float f;
    int i;
    bool b;
    dynamicarray l;
    hashmap o;
};

typedef struct json
{
    jsontype type;
    union jsonval val;
} json;

/*
    constructors
*/
json json_typeSize(jsontype type, unsigned int n);
json json_type(jsontype type);
json json_null();
json json_string(char *val);
json json_float(float val);
json json_int(int val);
json json_bool(bool val);
json json_list(dynamicarray val);
json json_arr(json *first, unsigned int n);
json json_obj(hashmap val);

/*
    list modifiers
*/
void json_list_add(json *list, json *val);
void json_list_add_null(json *list);
void json_list_add_string(json *list, char *val);
void json_list_add_float(json *list, float val);
void json_list_add_int(json *list, int val);
void json_list_add_bool(json *list, bool val);

/*
    object modifiers
*/
void json_obj_put(json *obj, char *key, json *val);
void json_obj_put_null(json *obj, char *key);
void json_obj_put_string(json *obj, char *key, char *val);
void json_obj_put_float(json *obj, char *key, float val);
void json_obj_put_int(json *obj, char *key, int val);
void json_obj_put_bool(json *obj, char *key, bool val);

/*
    output accessors
*/
strstream json_stringify(json j, bool prettify, int initTabPos, bool isDictVal);
char *json_dump(json j, bool prettify, int *n);

/*
    destructors
*/
void json_free(json *j);
void json_freeDeep(json *j);

#endif