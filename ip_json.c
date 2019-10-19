
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
 *  along with ip_json parser.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Mustafa CAMURLI - mustafacamurli@gmail.com
 *  Sun July 14, 2013
 */

/**
 * Author: Mustafa CAMURLI
 * Date: Sun July 14, 2013
 *
 * +) Initil version of in place json parser library
 *
 * Example feature abc
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ip_json.h"

static int ip_json_strncmp(const char* s1, const char* s2, size_t len)
{
    size_t i;

    if (!s1 && !s2)
        return (0);
    else if (!s1)
        return (1);
    else if (!s2)
        return (-1);


    for (i = 0; i < len; ++i)
    {
        if (!s1[i] || tolower((s1[i]) < tolower(s2[i])))
            return (-1);
        else if (!s2[i] || tolower((s1[i]) > tolower(s2[i])))
            return (1);
    }
    return 0;
}

static size_t ip_json_pass_ws(const char* str)
{
    size_t i = 0;
    if (!str)
        return (0);

    while (str[i] && isspace(str[i]))
        ++i;
    return (i);
}

static char* ip_json_last_nonws_char(const char* str)
{
    int i;
    int len;

    if (!str)
        return (NULL);

    len = strlen(str);

    if (0 == len)
        return (NULL);

    for (i = (len - 1); i >= 0; --i)
        if (!isspace(str[i]))
            return (str + i);

    return (NULL);
}

static int ip_json_detect_type(const char* str)
{
    if (!str)
        return (0);

    switch (*str)
    {
        case '{': return (IP_JSON_VAL_TYPE_OBJ);
        case '[': return (IP_JSON_VAL_TYPE_ARR);
        case '\"': return (IP_JSON_VAL_TYPE_STR);
        case '0' ... '9':
        case '-': return (IP_JSON_VAL_TYPE_NUM);
    }

    if (!ip_json_strncmp(str, "true", strlen("true")))
        return (IP_JSON_VAL_TYPE_TRUE);
    else if (!ip_json_strncmp(str, "false", strlen("false")))
        return (IP_JSON_VAL_TYPE_FALSE);
    else if (!ip_json_strncmp(str, "null", strlen("null")))
        return (IP_JSON_VAL_TYPE_NULL);

    return (0); /* this is the error case */
}

static int ip_json_get_obj_or_arr_len(const char* str, const char opening_key, const char closing_key)
{
    size_t i = 1;
    int b_lvl = 1;

    if (!str)
        return (0);

    /* self check, first character must be opening brace */
    if (opening_key != str[0])
        return (0);

    while (str[i])
    {
        if (opening_key == str[i])
        {
            ++b_lvl;
        }
        else if (closing_key == str[i])
        {
            --b_lvl;

            if (0 == b_lvl) /* main closing brace found */
            {
                return (i + 1);
            }
        }
        ++i;
    }
    return (0);
}

static int ip_json_get_obj_len(const char* str)
{
    return (ip_json_get_obj_or_arr_len(str, '{', '}'));
}

static int ip_json_get_arr_len(const char* str)
{
    return (ip_json_get_obj_or_arr_len(str, '[', ']'));
}

static int ip_json_get_str_len(const char* str)
{
    size_t i = 1;
    uint8_t escaped;
    uint8_t hex_digits;

    if (!str)
        return (0);

    /* self check, string must be started with a quotation mark */
    if ('\"' != str[0])
        return (0);

    while (str[i])
    {
        if (hex_digits)
        {
            switch (str[i])
            {
                case '0' ... '9':
                case 'a' ... 'f':
                case 'A' ... 'F':
                    break;
                default:
                    return (0); /* invalid hex character is encountered */
            }
            --hex_digits;
        }
        else if (escaped)
        {
            switch (str[i])
            {
                case '\"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    break;
                case 'u':
                    hex_digits = 4;
                    break;
                default:
                    return (0); /* invalid escape character is encountered */
            }
            escaped = 0;
        }
        else if ('\\' == str[i])
        {
            escaped = 1;
        }
        else if ('\"' == str[i])
        {
            return (i + 1);
        }

        ++i;
    }

    return (0);
}

static int ip_json_get_num_len(const char* str)
{
    size_t i = 1;
    uint8_t digit_seen = 0;

    if (!str)
        return (0);

    /* self check, number must be started with a negate sign or digit */
    switch (str[0])
    {
        case '0' ... '9':
            digit_seen = 1;
            break;
        case '-':
            break;
        default:
            return (0);
    }

    while (str[i])
    {
        if (isdigit(str[i])) /* 0 ... 9 */
        {
            switch (str[i - 1])
            {
                case '0':
                    if (1 == i)
                        return (0); /* if first number is 0, next chararcter cannot be digit */
                    break;
                case '1' ... '9':
                case '+':
                case '-':
                case '.':
                case 'e':
                case 'E':
                    break;
                default:
                    return (0);
            }
        }
        else if ('.' == str[i] || ('e' == str[i]) || ('E' == str[i]))
        {
            if (!isdigit(str[i - 1]))
            {
                return (0);
            }
        }
        else if (('-' == str[i]) || ('+' == str[i]))
        {
            if (!(('e' == str[i - 1]) || ('E' == str[i - 1])))
            {
                return (0);
            }
        }
        else if (isspace(str[i]) || (',' == str[i]) || (']' == str[i]) || ('}' == str[i]))
        {
            if (!isdigit(str[i - 1])) /* we're done. number only can end with digit */
            {
                return (0);
            }
            return (i);
        }
        else /* other characters are not allowed in number */
        {
            return (0); /* malformed json file */
        }

        digit_seen = 0;
        ++i;
    }

    if (digit_seen)
    {
        return (1);
    }

    return (0);
}

static int ip_json_get_len_by_type(ip_json_val_t* vp)
{
    int len = 0;

    if (!vp)
        return (0);

    switch (vp->type)
    {
        case IP_JSON_VAL_TYPE_OBJ:
            len = ip_json_get_obj_len(vp->start);
            break;
        case IP_JSON_VAL_TYPE_ARR:
            len = ip_json_get_arr_len(vp->start);
            break;
        case IP_JSON_VAL_TYPE_STR:
            len = ip_json_get_str_len(vp->start);
            break;
        case IP_JSON_VAL_TYPE_NUM:
            len = ip_json_get_num_len(vp->start);
            break;
        case IP_JSON_VAL_TYPE_TRUE:
            len = strlen("true");
            break;
        case IP_JSON_VAL_TYPE_FALSE:
            len = strlen("false");
            break;
        case IP_JSON_VAL_TYPE_NULL:
            len = strlen("null");
            break;
        default:
            break;
    }

    return (len);
}

static int ip_json_detect_and_parse_next_val(const char* str, ip_json_val_t* vp)
{
    size_t i;

    if (!vp)
        return (1);

    i = ip_json_pass_ws(str);

    vp->start = str + i;
    vp->type  = ip_json_detect_type(str + i);
    vp->len   = ip_json_get_len_by_type(vp);

    if (!(vp->len))
        return (1);

    return (0);
}

static int ip_json_parse_name_n_val(const char* str, ip_json_val_t* np, ip_json_val_t* vp)
{
    size_t i;
    char* cur;

    if (!np || !vp)
        return (1);

    if(0 != ip_json_detect_and_parse_next_val(str, np))
        return (1);

    if (IP_JSON_VAL_TYPE_STR != np->type)
        return (1);

    cur = np->start + np->len;

    i = ip_json_pass_ws(cur);

    if (':' != *(cur + i))
        return (1);

    cur += i + 1;

    if (0 != ip_json_detect_and_parse_next_val(cur, vp))
        return (1);

    return (0);
}

void ip_json_printn(const char* s, size_t len)
{
    size_t i;

    for (i = 0; i < len; ++i)
    {
        printf("%c", s[i]);
    }
}

int ip_json_get_value_by_name(ip_json_val_t* op, const char* name, ip_json_val_t* vp)
{
    size_t i;
    char* cur;
    ip_json_val_t str;

    if (!op || !vp)
        return (1);

    if (IP_JSON_VAL_TYPE_OBJ != op->type)
        return (1);

    cur = op->start + 1;

    while (!ip_json_parse_name_n_val(cur, &str, vp))
    {
        if (0 == ip_json_strncmp(name, str.start + 1, str.len - 2))
            return (0);

        cur = vp->start + vp->len;

        i = ip_json_pass_ws(cur);

        if (',' == *(cur + i))
            cur += i + 1; /* find comma, just fine move on */
        else
            return (1); /* invalid character */
    }

    i = ip_json_pass_ws(cur);

    if ('}' == *(cur + i))
    {
        /* end of string and cannot find the value */
        return (1);
    }

    /* invalid character is encountered invalid json */
    return (1);
}

int ip_json_get_value_by_index(ip_json_val_t* ap, size_t index, ip_json_val_t* vp)
{
    size_t i;
    char* cur;
    ip_json_val_t str;

    if (!ap || !vp)
        return (1);

    if (IP_JSON_VAL_TYPE_ARR != ap->type)
        return (1);

    cur = ap->start + 1;

    while (!ip_json_detect_and_parse_next_val(cur, vp))
    {
        if (!index)
            return (0);

        cur = vp->start + vp->len;

        i = ip_json_pass_ws(cur);

        if (',' == *(cur + i))
            cur += i + 1; /* find comma, just fine move on */
        else
            return (1); /* invalid character */

        --index;
    }

    i = ip_json_pass_ws(cur);

    if ('}' == *(cur + i))
    {
        /* end of string and cannot find the value */
        return (1);
    }

    /* invalid character is encountered invalid json */
    return (1);
}

int ip_json_init_str(const char* str, ip_json_val_t* vp)
{
    return (ip_json_detect_and_parse_next_val(str, vp));
}


