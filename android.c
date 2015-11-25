
#include "android.h"
#include <string.h>

extern int __system_property_get(const char *, char *);

int property_get(const char *key, char *val, const char *def) {
    int len;

    len = __system_property_get(key, val);
    if(len > 0) {
        return len;
    }
    if(def) {
        len = strlen(def);
        if (len >= PROPERTY_VALUE_MAX) {
            len = PROPERTY_VALUE_MAX - 1;
        }
        memcpy(val, def, len);
        val[len] = '\0';
    }
    return len;
}

