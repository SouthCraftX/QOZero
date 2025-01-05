# QOZero
The most basic library for QuickOK libraries

## What it provides?
- Basic types including atomic types (C11 may be required) like `qo_uint32_t`, `qo_ccstring_t` and `qo_fp32_t`
- Limit Macros for basic types 
- Attribute macros like `QO_NODISCARD` and `QO_FORCE_INLINE`
- Macros that identify the trait of the target platform like `QO_PLATFORM(x)`, `QO_SYSTEM_BIT(x)` and `QO_ENDIAN(x)`
- Simple calulators like `QO_MAX(x , y)` and `QO_ABS(x)`
- Macros for debug like `QO_ASSERT(x)` 
- Common error codes like `QO_OK` and `QO_INVALID_ARG`

## Usage
The library is designed to develop QuickOK series of libraries, and if you wants, use it like this:
```c
// Simply including the library is enough
#include "QOZero/qozero.h"

int main()
{
    // Do something
    qo_size_t foo , bar;
    qo_size_t max = QO_MAX(foo , bar);
}
```

## Applying Git Subtree
QuickOK series of libraries use QOZero via git subtree. To use QOZero in this way:
First time adding:
```bash
cd your_repo
git subtree add --prefix=dependencies/QOZero https://github.com/QuickOK/QOZero.git master --squash
```
Pulling lastest changes:
```sh
git subtree pull --prefix=dependencies/QOZero https://github.com/QuickOK/QOZero.git master --squash
```
Replace `dependencies` to a suitable path.

## Capability
Most of the code is capable of C99 or higher standard, but there are some excceptions:
- Atomic types on compilers that do not support GNU C extensions requires C11 or higher (To use C11's atomic types).
C++ capability is experimental. I'm working on it. (Define `QO_ENABLE_EXPERIMENTAL_CXX` to enable it)

## Contributing
Pull requests are welcome. You should:
- Communicate in English any time
- Follow the coding style of the project
- Ensure your code licensed under the MIT License
- Test before submitting
- Discuss first before submitting breaking changes
- Contact me privately if it concerns about security

## License
This library is licensed under the MIT License.
See the LICENSE file for more information.

