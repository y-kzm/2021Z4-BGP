/* 
 * param.c
 * 2021/08/24 ~
 * Yokoo Kazuma
 * Read configures from json file.
*/

#include <stdio.h>
#include "param.h"

struct config
*parse_json(const char *buf, size_t buflen)
{
    // Variable to hold JSON objects.
    json_error_t error;
    json_t *obj;
    json_t *root;
    const char *str;
    int value;

    struct config *cfg = malloc(sizeof(*cfg));

    // Read the entire JSON buffer.
    root = json_loadb(buf, buflen, 0, &error);
    if(root == NULL) {
        fprintf(stderr, "Failed to read the json file.\n");
        exit(EXIT_FAILURE);
    }

    // Read the value.
    obj = json_object_get(root, "router bgp");
    if(json_is_integer(obj)) {
        value = json_integer_value(obj);
        cfg->my_as = value; 
        // printf("router bgp: %d\n", value);
    }

    obj = json_object_get(root, "router-id");
    if(json_is_string(obj)) {
        str = json_string_value(obj);
        inet_aton(str, &cfg->router_id);
        // printf("router-id: %s\n", str);
    }


    // Neighbor from here.
    obj = json_object_get(root, "neighbor");
    if(json_is_object(obj)) {
        json_t *obj_ne;

        obj_ne = json_object_get(obj, "address");
        if(json_is_string(obj_ne)) {
            str = json_string_value(obj_ne);
            inet_aton(str, &cfg->ne.addr);
            // printf("neighbor address: %s\n", str); 
        }

        obj_ne = json_object_get(obj, "remote-as");
        if(json_is_integer(obj_ne)) {
            value = json_integer_value(obj_ne);
            cfg->ne.remote_as = value;
            // printf("remote-as: %d\n", value); 
        }
    }

    return cfg;
}