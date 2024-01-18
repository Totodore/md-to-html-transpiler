/**
 * @file simple_strings.h
 * @author Yohann MARTIN (Astropilot@Github)
 *
 * MIT License
 *
 * Copyright (c) 2022 Yohann MARTIN
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef SIMPLE_STRINGS_H
#define SIMPLE_STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define SIMPLE_STRINGS_VERSION_MAJOR 0
#define SIMPLE_STRINGS_VERSION_MINOR 1
#define SIMPLE_STRINGS_VERSION_PATCH 0

#ifndef SIMPLE_STRINGS_API
    #ifdef SIMPLE_STRINGS_INLINE
        #define SIMPLE_STRINGS_API extern inline
    #else
        #define SIMPLE_STRINGS_API extern
    #endif
#endif

#if !defined(_SIMPLE_STRINGS_NODEFAULT_ALLOCATORS)
	#define _SIMPLE_STRINGS_MALLOC(_size)        malloc(_size)
    #define _SIMPLE_STRINGS_REALLOC(_ptr, _size) realloc(_ptr, _size)
	#define _SIMPLE_STRINGS_FREE(_ptr)           free(_ptr)
#endif

/* String buffer
##################################################################
# string length (uint64_t) # buffer capacity (uint64_t) # buffer #
##################################################################
*/

struct string_header {
    uint64_t length;
    uint64_t capacity;
};

typedef struct string_header string_header_t;
typedef char* string;

/* Function declarations */

/**
 * @brief Allocate a new string from a char array.
 *
 * @param str The char array to copy into the new string
 * @return string
 */
SIMPLE_STRINGS_API
string spls_new_string(const char *str);

/**
 * @brief Free a previously allocated string. Should be done to prevent memory leaking.
 *
 * @param str The string to free
 */
SIMPLE_STRINGS_API
void spls_free_string(string str);

/**
 * @brief
 *
 * @param str
 * @return SIMPLE_STRINGS_API
 */
SIMPLE_STRINGS_API
string spls_clone(const string str);

SIMPLE_STRINGS_API
size_t spls_len(const string str);

/* All string manipulations */

SIMPLE_STRINGS_API
string spls_append_string(string str, string to_append);

SIMPLE_STRINGS_API
string spls_append_array(string str, const char *to_append);

SIMPLE_STRINGS_API
string spls_capitalize(const string str);

SIMPLE_STRINGS_API
string spls_tolower(const string str);

SIMPLE_STRINGS_API
string spls_toupper(const string str);

SIMPLE_STRINGS_API
unsigned int spls_count(const string str, const string search, unsigned int start, unsigned int end);

SIMPLE_STRINGS_API
bool spls_startswith(const string str, const string value);

SIMPLE_STRINGS_API
bool spls_endswith(const string str, const string value);

SIMPLE_STRINGS_API
int spls_find(const string str, const string search, unsigned int start, unsigned int end);

SIMPLE_STRINGS_API
bool spls_isalnum(const string str);

SIMPLE_STRINGS_API
bool spls_isalpha(const string str);

SIMPLE_STRINGS_API
bool spls_isdigit(const string str);

SIMPLE_STRINGS_API
bool spls_islower(const string str);

SIMPLE_STRINGS_API
bool spls_isupper(const string str);

SIMPLE_STRINGS_API
bool spls_isprintable(const string str);

SIMPLE_STRINGS_API
bool spls_isspace(const string str);

SIMPLE_STRINGS_API
string spls_strip(const string str, const string characters);

SIMPLE_STRINGS_API
string spls_lstrip(const string str, const string characters);

SIMPLE_STRINGS_API
string spls_rstrip(const string str, const string characters);

SIMPLE_STRINGS_API
string spls_replace(const string str, const string old_value, const string new_value, unsigned int count);

SIMPLE_STRINGS_API
string* spls_split(const string str, const string separator, unsigned int maxsplit);

SIMPLE_STRINGS_API
string* spls_splitlines(const string str, bool keeplinebreaks);

SIMPLE_STRINGS_API
string spls_substring(const string str, unsigned int start_index, unsigned int end_index);

/* Public utils */
#define STR(str) spls_new_string(str)
#define CLONE(str) spls_clone(str)
#define APPEND_STR(str, to_append) (str) = spls_append_string(str, to_append)
#define APPEND_ARR(str, to_append) (str) = spls_append_array(str, to_append)

#ifdef __cplusplus
}
#endif

#ifdef SIMPLE_STRINGS_IMPL

/* Internal utils */
#define STR_BUFFER_PTR(str_ptr) (str_ptr - sizeof(string_header_t))
#define BUFFER_STR_PTR(buf_ptr) (buf_ptr + sizeof(string_header_t))
#define BUFFER_LENGTH(str_len) (sizeof(string_header_t) + (sizeof(char) * (str_len + 1)))
#define BUFFER_LENGTH_CAPACITY(str_capacity) (sizeof(string_header_t) + (sizeof(char) * str_capacity))

#define NULL_GUARD(ptr, return_val) if (ptr == NULL) return (return_val);
#define NULL_GUARD_VOID(ptr) if (ptr == NULL) return;
#define SAFE_FREE(ptr) if (ptr != NULL) _SIMPLE_STRINGS_FREE(ptr);

string spls_new_string(const char *str) {
    NULL_GUARD(str, NULL)

    size_t str_len = strlen(str);
    string m_str = (string) _SIMPLE_STRINGS_MALLOC(BUFFER_LENGTH(str_len));
    string_header_t *str_header = (string_header_t*)m_str;

    str_header->length = str_len;
    str_header->capacity = str_len + 1;

    strcpy(BUFFER_STR_PTR(m_str), str);

    return BUFFER_STR_PTR(m_str);
}

string spls_clone(const string str) {
    NULL_GUARD(str, NULL)

    string_header_t *str_header = (string_header_t*) STR_BUFFER_PTR(str);
    string m_str = (string) _SIMPLE_STRINGS_MALLOC(BUFFER_LENGTH_CAPACITY(str_header->capacity));

    memcpy(m_str, str_header, BUFFER_LENGTH_CAPACITY(str_header->capacity));

    return BUFFER_STR_PTR(m_str);
}

void spls_free_string(string str) {
    NULL_GUARD_VOID(str)

    string buffer = STR_BUFFER_PTR(str);

    SAFE_FREE(buffer)
}

string spls_append_string(string str, string to_append) {
    NULL_GUARD(str, NULL)
    NULL_GUARD(to_append, NULL)

    string_header_t *str_header = (string_header_t*) STR_BUFFER_PTR(str);
    string_header_t *to_append_header = (string_header_t*) STR_BUFFER_PTR(to_append);
	size_t total_len = str_header->length + to_append_header->length + 1;
    string str_buffer = (string) str_header;

	if (total_len > str_header->capacity) {
		size_t new_capacity = total_len * 1.5;

		str_buffer = (string) _SIMPLE_STRINGS_REALLOC(str_header, BUFFER_LENGTH_CAPACITY(new_capacity));
        str_header = (string_header_t*) str_buffer;
		str_header->capacity = new_capacity;
	}
    strcpy(BUFFER_STR_PTR(str_buffer) + str_header->length, to_append);
	str_header->length = total_len - 1;

    return BUFFER_STR_PTR(str_buffer);
}

string spls_append_array(string str, const char *to_append) {
    NULL_GUARD(str, NULL)
    NULL_GUARD(to_append, NULL)

    string_header_t *str_header = (string_header_t*) STR_BUFFER_PTR(str);
	size_t to_append_len = strlen(to_append);
	size_t total_len = str_header->length + to_append_len + 1;
    string str_buffer = (string) str_header;

	if (total_len > str_header->capacity) {
		size_t new_capacity = total_len * 1.5;

		str_buffer = (string) _SIMPLE_STRINGS_REALLOC(str_header, BUFFER_LENGTH_CAPACITY(new_capacity));
        str_header = (string_header_t*) str_buffer;
		str_header->capacity = new_capacity;
	}
    strcpy(BUFFER_STR_PTR(str_buffer) + str_header->length, to_append);
    str_header->length = total_len - 1;

    return BUFFER_STR_PTR(str_buffer);
}

size_t spls_len(const string str) {
    NULL_GUARD(str, 0)

    string_header_t *str_header = (string_header_t*) STR_BUFFER_PTR(str);

    return str_header->length;
}

string spls_capitalize(const string str) {
    NULL_GUARD(str, NULL)

    if (spls_len(str) == 0) {
        return spls_clone(str);
    }

    string copy = spls_clone(str);

    copy[0] = toupper(copy[0]);

    return copy;
}

string spls_tolower(const string str) {
    NULL_GUARD(str, NULL)

    if (spls_len(str) == 0) {
        return spls_clone(str);
    }

    string copy = spls_clone(str);
    size_t i;

    for (i = 0; i < spls_len(copy); i++) {
        copy[i] = tolower(copy[i]);
    }

    return copy;
}

string spls_toupper(const string str) {
    NULL_GUARD(str, NULL)

    if (spls_len(str) == 0) {
        return spls_clone(str);
    }

    string copy = spls_clone(str);
    size_t i;

    for (i = 0; i < spls_len(copy); i++) {
        copy[i] = toupper(copy[i]);
    }

    return copy;
}

unsigned int spls_count(const string str, const string search, unsigned int start, unsigned int end) {
    NULL_GUARD(str, 0)
    NULL_GUARD(search, 0)

    unsigned int count = 0;
    size_t str_len = spls_len(str);

    if (spls_len(search) > str_len) {
        return 0;
    }

    if (start > str_len - 1) {
        start = str_len - 1;
    }
    if (end > str_len - 1) {
        end = str_len - 1;
    }

    while(start < end) {
        int pos = spls_find(str, search, start, end);

        if (pos >= 0) {
            start = pos + 1;
            count++;
        } else {
            start = end;
        }
    }

    return count;
}

bool spls_startswith(const string str, const string value) {
    NULL_GUARD(str, 0)
    NULL_GUARD(value, 0)

    size_t str_len = spls_len(str);
    size_t value_len = spls_len(value);
    unsigned int i;

    if (value_len > str_len) {
        return false;
    }

    for (i = 0; i < value_len; i++) {
        if (str[i] != value[i]) {
            return false;
        }
    }

    return true;
}

bool spls_endswith(const string str, const string value) {
    NULL_GUARD(str, 0)
    NULL_GUARD(value, 0)

    size_t str_len = spls_len(str);
    size_t value_len = spls_len(value);
    unsigned int i;

    if (value_len > str_len) {
        return false;
    }

    for (i = 0; i < value_len; i++) {
        if (str[str_len - 1 - i] != value[value_len - 1 - i]) {
            return false;
        }
    }

    return true;
}

int spls_find(const string str, const string search, unsigned int start, unsigned int end) {
    NULL_GUARD(str, -1)
    NULL_GUARD(search, -1)

    size_t str_len = spls_len(str);

    if (start > str_len - 1 || end > str_len - 1 || start > end) {
        return -1;
    }
    if (str_len < spls_len(search)) {
        return -1;
    }

    string sub = spls_substring(str, start, end);

    char *find = strstr(sub, search);

    spls_free_string(sub);

    if (find == NULL) {
        return -1;
    }

    return (find - sub) + start;
}

bool spls_isalnum(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (isalnum(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool spls_isalpha(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (isalpha(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool spls_isdigit(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (isdigit(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool spls_islower(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (islower(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool spls_isupper(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (isupper(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool spls_isprintable(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (isprint(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

bool spls_isspace(const string str) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    unsigned int i;

    if (str_len == 0) {
        return false;
    }

    for (i = 0; i < str_len; i++) {
        if (isspace(str[i]) == 0) {
            return false;
        }
    }

    return true;
}

string spls_strip(const string str, const string characters) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    char *tmp = str;
    unsigned int removed_chars = 0;

    if (str_len == 0) {
        return spls_clone(str);
    }

    if (characters != NULL) {
        while (*tmp && strchr(characters, *tmp) != NULL) {
            tmp++;
        }
    } else {
        while (*tmp && isspace(*tmp)) {
            tmp++;
        }
    }

    string copy = spls_new_string(tmp);
    string_header_t *copy_header = (string_header_t*) STR_BUFFER_PTR(copy);

    if (copy_header->length == 0) {
        return copy;
    }

    tmp = copy + copy_header->length - 1;

    if (characters != NULL) {
        while (tmp >= copy && strchr(characters, *tmp) != NULL) {
            tmp--;
            removed_chars++;
        }
    } else {
        while (tmp >= copy && isspace(*tmp)) {
            tmp--;
            removed_chars++;
        }
    }

    *(tmp + 1) = '\0';

    copy_header->length = copy_header->length - removed_chars;

    return copy;
}

string spls_lstrip(const string str, const string characters) {
    NULL_GUARD(str, NULL)

    char *tmp = str;

    if (spls_len(str) == 0) {
        return spls_clone(str);
    }

    if (characters != NULL) {
        while (*tmp && strchr(characters, *tmp) != NULL) {
            tmp++;
        }
    } else {
        while (*tmp && isspace(*tmp)) {
            tmp++;
        }
    }

    return spls_new_string(tmp);
}

string spls_rstrip(const string str, const string characters) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);
    string copy = spls_clone(str);
    string_header_t *copy_header = (string_header_t*) STR_BUFFER_PTR(copy);
    char *end;
    unsigned int removed_chars = 0;

    if (str_len == 0) {
        return copy;
    }

    end = copy + str_len - 1;

    if (characters != NULL) {
        while (end >= copy && strchr(characters, *end) != NULL) {
            end--;
            removed_chars++;
        }
    } else {
        while (end >= copy && isspace(*end)) {
            end--;
            removed_chars++;
        }
    }

    *(end + 1) = '\0';

    copy_header->length = str_len - removed_chars;

    return copy;
}

string spls_replace(const string str, const string old_value, const string new_value, unsigned int count) {
    NULL_GUARD(str, NULL)

    if (old_value == NULL || new_value == NULL || count == 0) {
        return spls_clone(str);
    }

    unsigned int replaced = 0;
    unsigned int start = 0;
    unsigned int end = spls_len(str) - 1;
    int last_index_found = -1;
    string new_string = spls_new_string("");

    do {
        last_index_found = spls_find(str, old_value, start, end);

        if (last_index_found >= 0) {
            string sub = spls_substring(str, start, last_index_found);

            new_string = spls_append_string(new_string, sub);
            new_string = spls_append_string(new_string, new_value);
            spls_free_string(sub);

            start = last_index_found + spls_len(old_value);

            replaced += 1;
        }

    } while (replaced < count && last_index_found >= 0);

    if (start < end) {
        new_string = spls_append_array(new_string, (str + start));
    }

    return new_string;
}

string* spls_split(const string str, const string separator, unsigned int maxsplit) {
    //TODO: Implement
    (void)str;
    (void)separator;
    (void)maxsplit;
    return NULL;
}

string* spls_splitlines(const string str, bool keeplinebreaks) {
    //TODO: Implement
    (void)str;
    (void)keeplinebreaks;
    return NULL;
}

string spls_substring(const string str, unsigned int start_index, unsigned int end_index) {
    NULL_GUARD(str, NULL)

    size_t str_len = spls_len(str);

    if (start_index > str_len - 1 || end_index > str_len || start_index > end_index)
    {
        return NULL;
    }

    string copy = spls_clone(str);

    copy[end_index] = '\0';

    string sub = spls_new_string(copy + start_index);

    spls_free_string(copy);

    return sub;
}

/* We clear all internals directives for preventing public use */

#undef STR_BUFFER_PTR
#undef BUFFER_STR_PTR
#undef BUFFER_LENGTH

#undef NULL_GUARD
#undef NULL_GUARD_VOID
#undef SAFE_FREE

#endif /* SIMPLE_STRINGS_IMPL */
#endif /* SIMPLE_STRINGS_H */
