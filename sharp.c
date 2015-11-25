
#include "sharp.h"
#include "android.h"
#include <string.h>

bool is_sharp_dev() {
    char val[PROPERTY_VALUE_MAX];

    memset(val, 0, sizeof(val));
    property_get("ro.product.manufacturer", val, "");
    return !strcmp(val, "SHARP") ? true : false;
}

