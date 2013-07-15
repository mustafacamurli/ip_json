
/**
 * This file is part of ip_json parser.
 *
 *  TmpltLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TmpltLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with TmpltLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Mustafa CAMURLI - mustafacamurli@gmail.com
 *  Sun July 14, 2013
 */

/**
 * Author: Mustafa CAMURLI
 * Date: Sun July 14, 2013
 *
 * +) Initil version of in place json parser library
*/

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
