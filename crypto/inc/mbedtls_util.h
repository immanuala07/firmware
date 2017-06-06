/**
 ******************************************************************************
 Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation, either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#ifndef MBEDTLS_UTIL_H
#define MBEDTLS_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include "mbedtls/md.h"

// Random number generator callback
int mbedtls_default_rng(void*, unsigned char* data, size_t size);

typedef struct {
    uint16_t version;
    uint16_t size;
    const int* (*mbedtls_md_list)(void);
    const mbedtls_md_info_t* (*mbedtls_md_info_from_string)(const char *md_name);
    const mbedtls_md_info_t* (*mbedtls_md_info_from_type)(mbedtls_md_type_t md_type);
} mbedtls_callbacks_t;

#ifdef  __cplusplus
extern "C" {
#endif

int mbedtls_set_callbacks(mbedtls_callbacks_t* callbacks, void* reserved);

#ifdef  __cplusplus
}
#endif

#endif // MBEDTLS_UTIL_H

