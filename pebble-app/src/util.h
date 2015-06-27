#ifndef _UTIL
#define _UTIL

#define copy_string(dst, src, len) if (snprintf((dst), (len), "%s", (src)) >= (len)) snprintf((dst), (len), "%s", "Overflow Error")

#endif
