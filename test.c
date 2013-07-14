


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "ip_json.h"

int file_dump_to_mem(const char* file, void** buf, size_t *len)
{
    int fd;
    struct stat st;

    if (!buf || !len)
        goto err0;

    if (access(file, F_OK))
        goto err0;

    if (stat(file, &st))
        goto err0;

    *buf = malloc(st.st_size);
    if (!(*buf))
        goto err0;

    fd = open(file, O_RDONLY);
    if (-1 == fd)
        goto err1;

    if (st.st_size != read(fd, *buf, st.st_size))
        goto err2;

    *len = st.st_size;
    close(fd);
    return (0);

err2:
    close(fd);
err1:
    free(*buf);
err0:
    return (1);
}

static const char* valid_test_numbers[] = { "0}",
                                            "-0 }",
                                            "41312 }",
                                            "123.123 }",
                                            "0.123 }",
                                            "0e10 }",
                                            "0.3e-10 }",
                                            NULL };

static const char* invalid_test_numbers[] = { "+0 }",
                                              "012 }",
                                              "123-1 }",
                                              "123-e12 }",
                                              "-.123 }",
                                              "0.e1 }",
                                              "1312.23e10r }",
                                              NULL };

int test_number()
{
    size_t i;
    ip_json_val_t val;

    fprintf(stderr, "%s testing ...\n", __FUNCTION__);

    fprintf(stderr, "valid numbers: ");
    for (i = 0; valid_test_numbers[i]; ++i)
    {
        assert(0 == ip_json_init_str(valid_test_numbers[i], &val));
        fprintf(stderr, "|");
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "invalid numbers: ");
    for (i = 0; valid_test_numbers[i]; ++i)
    {
        assert(1 == ip_json_init_str(invalid_test_numbers[i], &val));
        fprintf(stderr, "|");
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "%s [PASSED]\n", __FUNCTION__);
}

int main( int argc, char* argv[])
{
    void* str;
    size_t len;
    ip_json_val_t base_obj;
    ip_json_val_t value;
    ip_json_val_t item;

    if (0 != file_dump_to_mem(argv[1], &str, &len))
    {
        printf("failed to dump file to memory.\n");
        return (1);
    }

    if (0 != ip_json_init_str(str, &base_obj))
    {
        printf("failed to init json string.\n");
        return (1);
    }

    if (0 != ip_json_get_value_by_name(&base_obj, "employees", &value))
    {
        printf("failed to get value by name.\n");
        return (1);
    }

    if (0 != ip_json_get_value_by_index(&value, 0, &item))
    {
        printf("failed to get value by index.\n");
        return (1);
    }

    printf("len: %d\n", item.len);
    printf("typ: %d\n", item.type);
    ip_json_printn(item.start, item.len);

    if (0 != ip_json_get_value_by_index(&value, 1, &item))
    {
        printf("failed to get value by index.\n");
        return (1);
    }

    printf("len: %d\n", item.len);
    printf("typ: %d\n", item.type);
    ip_json_printn(item.start, item.len);

    if (0 != ip_json_get_value_by_index(&value, 2, &item))
    {
        printf("failed to get value by index.\n");
        return (1);
    }

    printf("len: %d\n", item.len);
    printf("typ: %d\n", item.type);
    ip_json_printn(item.start, item.len);

    printf("Alles klar!\n");

    return 0;

}












