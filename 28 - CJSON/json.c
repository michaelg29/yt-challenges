#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "json.h"
#include "strstream.h"

/*
    constructors
*/
json json_typeSize(jsontype type, unsigned int n)
{
    json ret;
    ret.type = type;

    switch (type)
    {
    case JSON_NULL:
        ret.val.n = NULL;
        break;
    case JSON_STR:
        ret.val.s = "";
        break;
    case JSON_FLOAT:
        ret.val.f = 0.0f;
        break;
    case JSON_INT:
        ret.val.i = 0;
        break;
    case JSON_BOOL:
        ret.val.b = false;
        break;
    case JSON_LIST:
        ret.val.l = dynarr_allocate(n);
        break;
    case JSON_OBJ:
        ret.val.o = hmap_allocateStrAsKeyWithNum(n);
        break;
    }

    return ret;
}

json json_type(jsontype type)
{
    return json_typeSize(type, 0);
}

json json_null()
{
    return json_type(JSON_NULL);
}

json json_string(char *val)
{
    json ret = json_type(JSON_STR);

    // copy string
    unsigned int n = strlen(val);
    ret.val.s = malloc((n + 1) * sizeof(char));
    strcpy(ret.val.s, val);

    return ret;
}

json json_float(float val)
{
    json ret = json_type(JSON_FLOAT);
    ret.val.f = val;
    return ret;
}

json json_int(int val)
{
    json ret = json_type(JSON_INT);
    ret.val.i = val;
    return ret;
}

json json_bool(bool val)
{
    json ret = json_type(JSON_INT);
    ret.val.b = val;
    return ret;
}

json json_list(dynamicarray val)
{
    json ret;
    ret.type = JSON_LIST;

    ret.val.l = val;
    return ret;
}

json json_arr(json *first, unsigned int n)
{
    json ret = json_typeSize(JSON_LIST, n);

    // insert values
    for (unsigned int i = 0; i < n; i++)
    {
        dynarr_addLast(&ret.val.l, (void *)(first + i));
    }

    return ret;
}

json json_obj(hashmap val)
{
    json ret;
    ret.type = JSON_OBJ;

    ret.val.o = val;
    return ret;
}

/*
    list modifiers
*/
void json_list_add(json *list, json *val)
{
    dynarr_addLast(&list->val.l, (void *)val);
}

void json_list_add_null(json *list)
{
    json *jval = malloc(sizeof(json));
    *jval = json_null();

    json_list_add(list, jval);
}

void json_list_add_string(json *list, char *val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_string(val);

    json_list_add(list, jval);
}

void json_list_add_float(json *list, float val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_float(val);

    json_list_add(list, jval);
}

void json_list_add_int(json *list, int val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_int(val);

    json_list_add(list, jval);
}

void json_list_add_bool(json *list, bool val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_bool(val);

    json_list_add(list, jval);
}

/*
    object modifiers
*/
void json_obj_put(json *obj, char *key, json *val)
{
    hmap_put(&obj->val.o, (void *)key, (void *)val);
}

void json_obj_put_null(json *obj, char *key)
{
    json *jval = malloc(sizeof(json));
    *jval = json_null();

    json_obj_put(obj, key, jval);
}

void json_obj_put_string(json *obj, char *key, char *val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_string(val);

    json_obj_put(obj, key, jval);
}

void json_obj_put_float(json *obj, char *key, float val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_float(val);

    json_obj_put(obj, key, jval);
}

void json_obj_put_int(json *obj, char *key, int val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_int(val);

    json_obj_put(obj, key, jval);
}

void json_obj_put_bool(json *obj, char *key, bool val)
{
    json *jval = malloc(sizeof(json));
    *jval = json_bool(val);

    json_obj_put(obj, key, jval);
}

/*
    output accessors
*/
strstream json_stringify(json j, bool prettify, int initTabPos, bool isDictVal)
{
    strstream ret;

    // initial indentation
    if (prettify && !isDictVal)
    {
        ret = strstream_alloc(4 * initTabPos);
        for (int i = 0; i < initTabPos; i++)
        {
            strstream_concat(&ret, TAB);
        }
    }
    else
    {
        ret = strstream_allocDefault();
    }

    // placeholder for end line
    char *_endl = prettify ? ENDL : "";

    // write element
    switch (j.type)
    {
    case JSON_NULL:
        strstream_concat(&ret, JSON_NULL_S);
        break;
    case JSON_STR:
        strstream_concat(&ret, "\"%s\"", j.val.s);
        break;
    case JSON_FLOAT:
        strstream_concat(&ret, "%f", j.val.f);
        break;
    case JSON_INT:
        strstream_concat(&ret, "%d", j.val.i);
        break;
    case JSON_BOOL:
        strstream_concat(&ret, "%s", j.val.b ? JSON_TRUE : JSON_FALSE);
        break;
    case JSON_LIST:
        if (!j.val.l.size)
        {
            // no elements, insert empty square brackets
            strstream_concat(&ret, "[]");
            break;
        }

        strstream_concat(&ret, "[%s", _endl);

        void *cur = NULL;
        dynarr_iterator list_it = dynarr_iterator_new(&j.val.l);

        while ((cur = dynarr_iterator_next(&list_it)))
        {
            // recursively get dumped string of the json element
            strstream el_stream = json_stringify(*((json *)cur), prettify, initTabPos + 1, false);
            // concatenate to return stream
            strstream_concat(&ret, "%s,%s", el_stream.str, _endl);
            strstream_clear(&el_stream);
        }

        // has an extra comma at the end
        // if prettify, have an extra ENDL at the end
        strstream_retreat(&ret, prettify ? 2 : 1);

        // add end characters
        if (prettify)
        {
            strstream_concat(&ret, ENDL);
            for (int i = 0; i < initTabPos; i++)
            {
                strstream_concat(&ret, TAB);
            }
        }

        strstream_concat(&ret, "]");

        break;

    case JSON_OBJ:
        if (!j.val.o.numEntries)
        {
            // no entries, concatenate opening and closing brackets
            strstream_concat(&ret, "{}");
            break;
        }

        strstream_concat(&ret, "{%s", _endl);

        mapentry *curEntry = NULL;
        hashmap_iterator obj_it = hmap_iterator_new(&j.val.o);

        while ((curEntry = hmap_iterator_next(&obj_it)))
        {
            // tab spaces
            if (prettify)
            {
                for (int i = 0; i < initTabPos + 1; i++)
                {
                    strstream_concat(&ret, TAB);
                }
            }

            // key
            strstream_concat(&ret, "\"%s\":%s", (char *)curEntry->key, prettify ? " " : "");

            // get dumped string from the value recursively
            strstream el_stream = json_stringify(*((json *)curEntry->val), prettify, initTabPos + 1, true);
            // concatenate to return stream
            strstream_concat(&ret, "%s,%s", el_stream.str, _endl);
            strstream_clear(&el_stream);
        }

        // has an extra comma at the end
        // if prettify, have an extra ENDL at the end
        strstream_retreat(&ret, prettify ? 2 : 1);
        if (prettify)
        {
            strstream_concat(&ret, ENDL);
            for (int i = 0; i < initTabPos; i++)
            {
                strstream_concat(&ret, TAB);
            }
        }

        strstream_concat(&ret, "}");

        break;
    };

    return ret;
}

char *json_dump(json j, bool prettify, int *n)
{
    // get output as a string
    strstream s = json_stringify(j, prettify, 0, false);
    // set the size
    *n = s.size;

    // allocate the return string
    char *ret = malloc((s.size + 1) * sizeof(char));
    memcpy(ret, s.str, *n + 1);
    strstream_clear(&s);
    return ret;
}

/*
    destructors
*/
void json_free(json *j)
{
    if (j->type == JSON_STR)
    {
        free(&j->val.s);
    }
    else if (j->type == JSON_LIST)
    {
        void *cur = NULL;
        dynarr_iterator list_it = dynarr_iterator_new(&j->val.l);

        while ((cur = dynarr_iterator_next(&list_it)))
        {
            json_free((json *)cur);
        }

        dynarr_free(&j->val.l);
    }
    else if (j->type == JSON_OBJ)
    {
        mapentry *cur = NULL;
        hashmap_iterator hmap_it = hmap_iterator_new(&j->val.o);

        while ((cur = hmap_iterator_next(&hmap_it)))
        {
            json_free((json *)cur->val);
        }

        hmap_free(&j->val.o);
    }
}

void json_freeDeep(json *j)
{
    if (j->type == JSON_STR)
    {
        free(&j->val.s);
    }
    else if (j->type == JSON_LIST)
    {
        void *cur = NULL;
        dynarr_iterator list_it = dynarr_iterator_new(&j->val.l);

        while ((cur = dynarr_iterator_next(&list_it)))
        {
            json_freeDeep((json *)cur);
        }

        dynarr_freeDeep(&j->val.l);
    }
    else if (j->type == JSON_OBJ)
    {
        mapentry *cur = NULL;
        hashmap_iterator hmap_it = hmap_iterator_new(&j->val.o);

        while ((cur = hmap_iterator_next(&hmap_it)))
        {
            json_freeDeep((json *)cur->val);
        }

        hmap_freeDeep(&j->val.o);
    }
}