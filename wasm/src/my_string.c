#include <string.h>
#include "my_math.h"
#include "my_string.h"

string string_new_sized(char *data, ptrdiff_t len, arena *a) {
    string new_str;
    uint8_t *new_data = a_malloc(a, uint8_t, len);
    memcpy(new_data, data, len);
    new_str.data = new_data;
    new_str.len = len;
    return new_str;
}

string string_new(char *data, arena *a) {
    return string_new_sized(data, lengthof(data), a);
}

string string_after(string str, ptrdiff_t idx) {
    return (string){str.data + idx, str.len - idx};
}

string_buffer string_buffer_new(ptrdiff_t cap, arena *a) {
    string_buffer new_str_buf = {0};
    new_str_buf.str.data = a_malloc(a, uint8_t, cap);
    new_str_buf.cap = cap;
    return new_str_buf;
}

void string_buffer_clear(string_buffer *buf) {
    buf->str.len = 0;
}

ptrdiff_t string_buffer_remaining(string_buffer *buf) {
    return buf->cap - buf->str.len;
}

ptrdiff_t string_buffer_add_str(string_buffer *buf, string str) {
    ptrdiff_t buf_rem = string_buffer_remaining(buf);
    ptrdiff_t append_len = buf_rem < str.len ? buf_rem : str.len;
    memcpy(buf->str.data + buf->str.len, str.data, append_len);
    buf->str.len += append_len;
    return append_len;
}

ptrdiff_t string_buffer_add_int(string_buffer *buf, int32_t val) {
    uint8_t num_str[11];
    if (0 == val) {
        num_str[0] = '0';
        return string_buffer_add_str(buf, (string){num_str, 1});
    }

    uint8_t negative = val < 0;
    if (negative) {
        num_str[0] = '-';
        val = -val;
    }
    
    const ptrdiff_t size = digits(val) + negative;
    uint8_t *num_str_ptr = num_str + size - 1;
    int32_t next_val;
    while (val >= 10) {
        next_val = val / 10;
        *(num_str_ptr--) = '0' + val - (next_val * 10);
        val = next_val;
    }
    *num_str_ptr = '0' + val;

    return string_buffer_add_str(buf, (string){num_str, size});
}

bool string_buffer_terminate(string_buffer *buf) {
    if (buf->str.len < buf->cap) {
        buf->str.data[buf->str.len] = '\0';
        buf->str.len += 1;
        return true;
    }
    return false;
}
