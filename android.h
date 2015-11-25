
#ifndef _ANDROID_H_
#define _ANDROID_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PROPERTY_VALUE_MAX 92

int property_get(const char *key, char *val, const char *def);

#ifdef __cplusplus
}
#endif

#endif

