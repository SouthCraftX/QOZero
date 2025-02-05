#pragma once

#include <assert.h>
#include "attribute.h"
#include "debug.h"
#include "platform.h"

#if defined(__GNUC__)
#   include "internal/gnuc_spec/types.h"
#elif defined(_MSC_VER)
#   include "internal/msvc_spec/types.h"
#else
#   include "internal/stdc_spec/types.h"
#endif 

#if QO_SYSTEM_BIT(64)
typedef     qo_uint64_t qo_uintmax_t;
typedef     qo_int64_t  qo_intmax_t;
typedef     qo_uint64_t qo_size_t;
typedef     qo_int64_t  qo_ssize_t;
#else
typedef     qo_int32_t          qo_intmax_t;
typedef     qo_uint32_t         qo_size_t;
typedef     qo_int32_t          qo_ssize_t;
#endif // QO_SYSTEM_BIT(64)

typedef     qo_uint8_t  qo_byte_t;

typedef     void*        qo_pointer_t;
typedef     qo_int8_t   qo_bool_t;
typedef     qo_uint32_t qo_ref_count_t;

#define     qo_true     ((qo_bool_t)1)
#define     qo_false    ((qo_bool_t)0)

#if defined(QO_WANT_HALF_LEN_STAT_T)
typedef     qo_int16_t  qo_stat_t;
#else
typedef     qo_int32_t  qo_stat_t;
#endif 

typedef     qo_ssize_t  qo_offset_t;

typedef     qo_int8_t   qo_flag8_t;
typedef     qo_int16_t  qo_flag16_t;
typedef     qo_int32_t  qo_flag32_t;
typedef     qo_int64_t  qo_flag64_t;

typedef     char*               qo_cstring_t;
typedef     char const  *       qo_ccstring_t;

#define QO_INVALID_ADDRESS ((qo_pointer_t)(-1))

#include "int128.h"

// --------- VLA --------
struct _QO_VLA
{
    qo_size_t  size;
    qo_byte_t  data[];
};
typedef struct _QO_VLA QO_VLA;

QO_GLOBAL_UNIQUE QO_FORCE_INLINE QO_NO_SIDE_EFFECTS
qo_pointer_t
qo_vla_at(
    QO_VLA *    vla ,
    qo_size_t   element_size ,
    qo_size_t   index
) {
    QO_ASSERT(index * element_size < vla->size);
    return (qo_pointer_t)(vla->data + index * element_size);
}

QO_GLOBAL_UNIQUE QO_FORCE_INLINE QO_NO_SIDE_EFFECTS
qo_pointer_t
qo_vla_get_data(
    QO_VLA *    vla
) {
    return (qo_pointer_t)(vla->data);
}