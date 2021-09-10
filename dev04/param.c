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
    // my_as.
    obj = json_object_get(root, "router_bgp");
    if(json_is_integer(obj)) {
        value = json_integer_value(obj);
        cfg->my_as = value; 
        // printf("router bgp: %d\n", value);
    }

    // router_id.
    obj = json_object_get(root, "router_id");
    if(json_is_string(obj)) {
        str = json_string_value(obj);
        inet_aton(str, &cfg->router_id);
        // printf("router-id: %s\n", str);
    }


    // Neighbor from here.
    obj = json_object_get(root, "neighbors");
    if(json_is_array(obj)) {
        int i;
        int array_size = json_array_size(obj);
        // printf("Location array size: %d\n", array_size);

        // Neighbor array loop.
        for(i = 0; i < array_size; i ++) {
            json_t *obj_ne = json_array_get(obj, i);
            if(json_is_object(obj_ne)) {
                json_t *obj_ne_items;

                // Address.
                obj_ne_items = json_object_get(obj_ne, "address");
                if(json_is_string(obj_ne_items)) {
                    str = json_string_value(obj_ne_items);
                    inet_aton(str, &cfg->ne.addr);
                    // printf("neighbor address: %s\n", str); 
                }
                // Remote_as.
                obj_ne_items = json_object_get(obj_ne, "remote_as");
                if(json_is_integer(obj_ne_items)) {
                    value = json_integer_value(obj_ne_items);
                    cfg->ne.remote_as = value;
                    // printf("remote-as: %d\n", value); 
                }
            }
        }

    }

    return cfg;
}