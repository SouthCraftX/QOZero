#pragma once
#define __QOZERO_H__

#if defined(__cplusplus) && !defined(QO_ENABLE_EXPERIMENTAL_CXX)
#   error QOZero is currently not supported in C++
#endif 

#include "attribute.h"
#include "platform.h"
#include "types.h"
#include "stat.h"
#include "debug.h"
#include "basic_class.h"
#include "fastclac.h"
#include "limit.h"

#if QO_PLATFORM(WINDOWS)
// #include "base/win32_common.h"
#endif // QO_PLATFORM(WINDOWS)