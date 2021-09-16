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

    int i, array_size;

    struct config *cfg = (struct config *)malloc(sizeof(struct config));

    // Prepare an array for the number of neighbors.
    // cfg = (struct config *)malloc(
    //     sizeof(struct config) + sizeof(struct neighbor) * 1);

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
        array_size = json_array_size(obj);
        cfg->neighbors_num = array_size;
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
                    inet_aton(str, &cfg->neighbors[i].addr);                  
                    // printf("neighbor address: %s\n", str); 
                }
                // Remote_as.
                obj_ne_items = json_object_get(obj_ne, "remote_as");
                if(json_is_integer(obj_ne_items)) {
                    value = json_integer_value(obj_ne_items);
                    cfg->neighbors[i].remote_as = value;
                    // printf("remote-as: %d\n", value); 
                }
            }
        }
    }

    // address_familly.
    obj = json_object_get(root, "address_familly");
    if(json_is_object(obj)) {
        json_t *obj_af;

        // ipv4_unicast.
        obj_af = json_object_get(obj, "ipv4_unicast");
        if(json_is_object(obj_af)) {
            json_t *obj_v4uni;

            // networks.
            obj_v4uni = json_object_get(obj_af, "networks");
            if(json_is_array(obj_v4uni)) {
                array_size = json_array_size(obj_v4uni);
                cfg->networks_num = array_size;

                // Networks array loop.
                for(i = 0; i < array_size; i ++) {
                    json_t *obj_net = json_array_get(obj_v4uni, i);
                    if(json_is_object(obj_net)){
                        json_t *obj_net_items;

                        // adress.
                        obj_net_items = json_object_get(obj_net, "address");
                        if(json_is_string(obj_net_items)) {
                            str = json_string_value(obj_net_items);
                            inet_aton(str, &cfg->networks[i].prefix.addr);
                            // printf("networks: %s\n", str); 
                        }
                        // prefix_len.
                        obj_net_items = json_object_get(obj_net, "prefix_len");
                        if(json_is_integer(obj_net_items)) {
                            value = json_integer_value(obj_net_items);
                            cfg->networks[i].prefix.len = value;
                            // printf("networks_len: %d\n", value); 
                        }
                    }
                }
            }
        }
    }

    return cfg;
}