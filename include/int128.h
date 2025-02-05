#pragma once
#include "internal/gnuc_spec/types.h"
#define __QO_INT128_H__

#include "types.h"
#include "attribute.h"

#if defined(__SIZEOF_INT128__) || _MSV_VER >= 1928
#   define QO_HAS_NATIVE_INT128 1
typedef     __int128_t          qo_int128_t;
typedef     __uint128_t         qo_uint128_t;

#   define QO_INT128_C(high , low)  (qo_int128_t)((high) << 64 | (low))
#   define QO_UINT128_C(high , low) (qo_uint128_t)((high) << 64 | (low))

#else
struct _QO_Int128 
{
    qo_uint64_t low;
    qo_int64_t  high;
};
typedef struct _QO_Int128 qo_int128_t;
struct _QO_Uint128
{
    qo_uint64_t low;
    qo_uint64_t high;
};
typedef struct _QO_Uint128 qo_uint128_t;
#   define QO_HAS_NATIVE_INT128 0
#endif