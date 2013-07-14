

#ifndef _IP_JSON_H_
#define _IP_JSON_H_

#include <stdint.h>

typedef struct ip_json_val_s ip_json_val_t;

enum ip_json_val_types_e {
    IP_JSON_VAL_TYPE_OBJ = 1,
    IP_JSON_VAL_TYPE_ARR,
    IP_JSON_VAL_TYPE_STR,
    IP_JSON_VAL_TYPE_NUM,
    IP_JSON_VAL_TYPE_TRUE,
    IP_JSON_VAL_TYPE_FALSE,
    IP_JSON_VAL_TYPE_NULL,
};

struct ip_json_val_s {
    char* start;
    size_t len;
    uint8_t type;
};

int ip_json_init_str(const char* str, ip_json_val_t* vp);
int ip_json_get_value_by_name(ip_json_val_t* op, const char* name, ip_json_val_t* vp);
int ip_json_get_value_by_index(ip_json_val_t* ap, size_t index, ip_json_val_t* vp);
void ip_json_printn(const char* s, size_t len);

#endif /* _IP_JSON_H_ */
