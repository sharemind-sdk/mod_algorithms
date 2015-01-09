/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#include "ToString.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sstream>


namespace {

template <typename T>
struct FixByteTypeForFormatting { typedef T type; };

template <>
struct FixByteTypeForFormatting<int8_t> { typedef int16_t type; };

template <>
struct FixByteTypeForFormatting<uint8_t> { typedef uint16_t type; };

template <typename T>
void integral_toString(SharemindCodeBlock * args,
                        SharemindCodeBlock * returnValue,
                        SharemindModuleApi0x1SyscallContext * c)
{
    static_assert(sizeof(T) <= sizeof(args[0u]),
                  "sizeof(T) <= sizeof(args[0u])");
    T * val = new (&args[0u]) T;
    std::ostringstream ss;
    ss << static_cast<typename FixByteTypeForFormatting<T>::type>(*val);
    const std::string str = ss.str();
    const uint64_t mem_size = str.size () + 1u;
    const uint64_t mem_hndl = (* c->publicAlloc) (c, mem_size);
    if (mem_hndl) {
        char * const mem_ptr =
                static_cast<char *>((*c->publicMemPtrData)(c, mem_hndl));
        assert(mem_ptr);
        strncpy(mem_ptr, str.c_str(), mem_size);
    }
    returnValue[0].uint64[0] = mem_hndl;
}

template <typename T, typename UT>
void float_toString(const void * const arg,
                     SharemindCodeBlock * returnValue,
                     SharemindModuleApi0x1SyscallContext * c)
{
    static_assert(sizeof(T) == sizeof(UT), "sizeof(T) == sizeof(UT)");
    T f_val;
    memcpy(&f_val, arg, sizeof(UT));
    std::ostringstream ss;
    ss << std::setprecision(std::numeric_limits<T>::digits10 + 1)
       << f_val;
    const std::string str = ss.str();
    const uint64_t mem_size = str.size() + 1u;
    const uint64_t mem_hndl = (* c->publicAlloc)(c, mem_size);
    if (mem_hndl) {
        char * const mem_ptr =
                static_cast<char *>((*c->publicMemPtrData)(c, mem_hndl));
        assert(mem_ptr);
        strncpy(mem_ptr, str.c_str(), mem_size);
    }
    returnValue[0].uint64[0] = mem_hndl;
}

} // anonymous namespace

SHAREMIND_EXTERN_C_BEGIN

#define TO_STRING_WRAPPER_DEFINE(type,logic) \
    SHAREMIND_MODULE_API_0x1_SYSCALL(type ## _toString, \
                                     args, num_args, refs, crefs, \
                                     returnValue, c) \
    { \
        if (num_args != 1 || refs || crefs || !returnValue) \
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL; \
        assert(c); \
        try { \
            logic \
        } catch (const std::bad_alloc &) { \
            return SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY; \
        } catch (...) { \
            return SHAREMIND_MODULE_API_0x1_SHAREMIND_ERROR; \
        } \
        return SHAREMIND_MODULE_API_0x1_OK; \
    }

#define INTEGRAL_TO_STRING_DEFINE(type) \
    TO_STRING_WRAPPER_DEFINE(\
        type, \
        integral_toString<type ## _t>(args, returnValue, c);)

INTEGRAL_TO_STRING_DEFINE(int8) INTEGRAL_TO_STRING_DEFINE(int16)
INTEGRAL_TO_STRING_DEFINE(int32) INTEGRAL_TO_STRING_DEFINE(int64)
INTEGRAL_TO_STRING_DEFINE(uint8) INTEGRAL_TO_STRING_DEFINE(uint16)
INTEGRAL_TO_STRING_DEFINE(uint32) INTEGRAL_TO_STRING_DEFINE(uint64)

#define FLOAT_TO_STRING_DEFINE(width,ctype) \
    TO_STRING_WRAPPER_DEFINE( \
        float ## width, \
        (float_toString<ctype, uint ## width ## _t>( \
                &args[0u].float ## width [0u], \
                returnValue, \
                c));)

FLOAT_TO_STRING_DEFINE(32, float)
FLOAT_TO_STRING_DEFINE(64, double)

SHAREMIND_EXTERN_C_END
