// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVUtils/Internal/API.hpp"
#include "SSVUtils/Encoding/Inc/Internal/MD5.hpp"

#include <string>
#include <climits>
#include <fstream>

namespace ssvu
{
namespace Encoding
{
namespace Impl
{
using POINTER = unsigned char*;
using CONST_POINTER = const unsigned char*;

// 16 bits integer
#if USHRT_MAX == 0xFFFF
using UINT2 = unsigned short;
#elif UINT_MAX == 0xFFFF
using UINT2 = unsigned int;
#elif ULONG_MAX == 0xFFFF
using UINT2 = unsigned long;
#endif

// 32 bits integer
#if USHRT_MAX == 0xFFFFFFFF
using UINT4 = unsigned short;
#elif UINT_MAX == 0xFFFFFFFF
using UINT4 = unsigned int;
#elif ULONG_MAX == 0xFFFFFFFF
using UINT4 = unsigned long;
#endif

struct MD5_CTX
{
    UINT4 state[4];
    UINT4 count[2];
    unsigned char buffer[64];
};

void MD5Init(MD5_CTX*);
void MD5Update(MD5_CTX*, const unsigned char*, unsigned int);
void MD5Final(unsigned char[16], MD5_CTX*);

SSVU_INLINE std::string MD5::Calculate(std::ifstream& file)
{
    if(!file) return "";
    file.seekg(0, std::ios::end);
    int length(file.tellg());
    file.seekg(0, std::ios::beg);

    char* buffer{new char[length]};

    file.read(buffer, length);
    Calculate((unsigned char*)buffer, length);
    delete[] buffer;
    return m_sHash;
}
SSVU_INLINE std::string MD5::Calculate(
    const unsigned char* source, uint32_t len)
{
    MD5_CTX context;

    MD5Init(&context);
    MD5Update(&context, source, len);
    MD5Final(m_rawHash, &context);

    m_sHash.clear();
    m_sHash.reserve(32);
    char buffer[3];
    buffer[2] = '\0';
    for(int i{0}; i < 16; ++i)
    {
        sprintf(buffer, "%02x", m_rawHash[i]);
        m_sHash += buffer;
    }
    return m_sHash;
}

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

void MD5Transform(UINT4[4], const unsigned char[64]);
void Encode(unsigned char*, const UINT4*, unsigned int);
void Decode(UINT4*, const unsigned char*, unsigned int);
void MD5_memcpy(POINTER, CONST_POINTER, unsigned int);
void MD5_memset(POINTER, int, unsigned int);

static unsigned char PADDING[64]{0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
        15-April-2003 Sony: use MSVC intrinsic to save some cycles
 */
#ifdef _MSC_VER
#pragma intrinsic(_rotl)
#define ROTATE_LEFT(x, n) _rotl((x), (n))
#else
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#endif

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac)                     \
    {                                                \
        (a) += F((b), (c), (d)) + (x) + (UINT4)(ac); \
        (a) = ROTATE_LEFT((a), (s));                 \
        (a) += (b);                                  \
    }
#define GG(a, b, c, d, x, s, ac)                     \
    {                                                \
        (a) += G((b), (c), (d)) + (x) + (UINT4)(ac); \
        (a) = ROTATE_LEFT((a), (s));                 \
        (a) += (b);                                  \
    }
#define HH(a, b, c, d, x, s, ac)                     \
    {                                                \
        (a) += H((b), (c), (d)) + (x) + (UINT4)(ac); \
        (a) = ROTATE_LEFT((a), (s));                 \
        (a) += (b);                                  \
    }
#define II(a, b, c, d, x, s, ac)                     \
    {                                                \
        (a) += I((b), (c), (d)) + (x) + (UINT4)(ac); \
        (a) = ROTATE_LEFT((a), (s));                 \
        (a) += (b);                                  \
    }

/* MD5 initialization. Begins an MD5 operation, writing a new
 * context.
 */
SSVU_INLINE void MD5Init(MD5_CTX* context)
{
    context->count[0] = context->count[1] = 0;
    /* Load magic initialization constants.
     */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
SSVU_INLINE void MD5Update(
    MD5_CTX* context, const unsigned char* input, unsigned int inputLen)
{
    unsigned int i, index, partLen;

    /* Compute number of bytes mod 64 */
    index = (unsigned int)((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if((context->count[0] += ((UINT4)inputLen << 3)) < ((UINT4)inputLen << 3))
        context->count[1]++;
    context->count[1] += ((UINT4)inputLen >> 29);

    partLen = 64 - index;

    /* Transform as many times as possible.
     */
    if(inputLen >= partLen)
    {
        MD5_memcpy(
            (POINTER)&context->buffer[index], (CONST_POINTER)input, partLen);
        MD5Transform(context->state, context->buffer);

        for(i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform(context->state, &input[i]);

        index = 0;
    }
    else
        i = 0;

    /* Buffer remaining input */
    MD5_memcpy((POINTER)&context->buffer[index], (CONST_POINTER)&input[i],
        inputLen - i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing
  the
  the message digest and zeroizing the context.
 */
SSVU_INLINE void MD5Final(unsigned char digest[16], MD5_CTX* context)
{
    unsigned char bits[8];
    unsigned int index, padLen;

    /* Save number of bits */
    Encode(bits, context->count, 8);

    /* Pad out to 56 mod 64.
     */
    index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, PADDING, padLen);

    /* Append length (before padding) */
    MD5Update(context, bits, 8);
    /* Store state in digest */
    Encode(digest, context->state, 16);

    /* Zeroize sensitive information.
     */
    MD5_memset((POINTER)context, 0, sizeof(*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
SSVU_INLINE void MD5Transform(UINT4 state[4], const unsigned char block[64])
{
    UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    Decode(x, block, 64);

    /* Round 1 */
    FF(a, b, c, d, x[0], S11, 0xd76aa478)  /* 1 */
    FF(d, a, b, c, x[1], S12, 0xe8c7b756)  /* 2 */
    FF(c, d, a, b, x[2], S13, 0x242070db)  /* 3 */
    FF(b, c, d, a, x[3], S14, 0xc1bdceee)  /* 4 */
    FF(a, b, c, d, x[4], S11, 0xf57c0faf)  /* 5 */
    FF(d, a, b, c, x[5], S12, 0x4787c62a)  /* 6 */
    FF(c, d, a, b, x[6], S13, 0xa8304613)  /* 7 */
    FF(b, c, d, a, x[7], S14, 0xfd469501)  /* 8 */
    FF(a, b, c, d, x[8], S11, 0x698098d8)  /* 9 */
    FF(d, a, b, c, x[9], S12, 0x8b44f7af)  /* 10 */
    FF(c, d, a, b, x[10], S13, 0xffff5bb1) /* 11 */
    FF(b, c, d, a, x[11], S14, 0x895cd7be) /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6b901122) /* 13 */
    FF(d, a, b, c, x[13], S12, 0xfd987193) /* 14 */
    FF(c, d, a, b, x[14], S13, 0xa679438e) /* 15 */
    FF(b, c, d, a, x[15], S14, 0x49b40821) /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[1], S21, 0xf61e2562)  /* 17 */
    GG(d, a, b, c, x[6], S22, 0xc040b340)  /* 18 */
    GG(c, d, a, b, x[11], S23, 0x265e5a51) /* 19 */
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa)  /* 20 */
    GG(a, b, c, d, x[5], S21, 0xd62f105d)  /* 21 */
    GG(d, a, b, c, x[10], S22, 0x2441453)  /* 22 */
    GG(c, d, a, b, x[15], S23, 0xd8a1e681) /* 23 */
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8)  /* 24 */
    GG(a, b, c, d, x[9], S21, 0x21e1cde6)  /* 25 */
    GG(d, a, b, c, x[14], S22, 0xc33707d6) /* 26 */
    GG(c, d, a, b, x[3], S23, 0xf4d50d87)  /* 27 */
    GG(b, c, d, a, x[8], S24, 0x455a14ed)  /* 28 */
    GG(a, b, c, d, x[13], S21, 0xa9e3e905) /* 29 */
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8)  /* 30 */
    GG(c, d, a, b, x[7], S23, 0x676f02d9)  /* 31 */
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a) /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[5], S31, 0xfffa3942)  /* 33 */
    HH(d, a, b, c, x[8], S32, 0x8771f681)  /* 34 */
    HH(c, d, a, b, x[11], S33, 0x6d9d6122) /* 35 */
    HH(b, c, d, a, x[14], S34, 0xfde5380c) /* 36 */
    HH(a, b, c, d, x[1], S31, 0xa4beea44)  /* 37 */
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9)  /* 38 */
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60)  /* 39 */
    HH(b, c, d, a, x[10], S34, 0xbebfbc70) /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289b7ec6) /* 41 */
    HH(d, a, b, c, x[0], S32, 0xeaa127fa)  /* 42 */
    HH(c, d, a, b, x[3], S33, 0xd4ef3085)  /* 43 */
    HH(b, c, d, a, x[6], S34, 0x4881d05)   /* 44 */
    HH(a, b, c, d, x[9], S31, 0xd9d4d039)  /* 45 */
    HH(d, a, b, c, x[12], S32, 0xe6db99e5) /* 46 */
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8) /* 47 */
    HH(b, c, d, a, x[2], S34, 0xc4ac5665)  /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[0], S41, 0xf4292244)  /* 49 */
    II(d, a, b, c, x[7], S42, 0x432aff97)  /* 50 */
    II(c, d, a, b, x[14], S43, 0xab9423a7) /* 51 */
    II(b, c, d, a, x[5], S44, 0xfc93a039)  /* 52 */
    II(a, b, c, d, x[12], S41, 0x655b59c3) /* 53 */
    II(d, a, b, c, x[3], S42, 0x8f0ccc92)  /* 54 */
    II(c, d, a, b, x[10], S43, 0xffeff47d) /* 55 */
    II(b, c, d, a, x[1], S44, 0x85845dd1)  /* 56 */
    II(a, b, c, d, x[8], S41, 0x6fa87e4f)  /* 57 */
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0) /* 58 */
    II(c, d, a, b, x[6], S43, 0xa3014314)  /* 59 */
    II(b, c, d, a, x[13], S44, 0x4e0811a1) /* 60 */
    II(a, b, c, d, x[4], S41, 0xf7537e82)  /* 61 */
    II(d, a, b, c, x[11], S42, 0xbd3af235) /* 62 */
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb)  /* 63 */
    II(b, c, d, a, x[9], S44, 0xeb86d391)  /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /* Zeroize sensitive information.
     */
    MD5_memset((POINTER)x, 0, sizeof(x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
SSVU_INLINE void Encode(
    unsigned char* output, const UINT4* input, unsigned int len)
{
    unsigned int i, j;
    for(i = 0, j = 0; j < len; ++i, j += 4)
    {
        output[j] = (unsigned char)(input[i] & 0xff);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
  a multiple of 4.
 */
SSVU_INLINE void Decode(
    UINT4* output, const unsigned char* input, unsigned int len)
{
    unsigned int i, j;
    for(i = 0, j = 0; j < len; ++i, j += 4)
        output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
                    (((UINT4)input[j + 2]) << 16) |
                    (((UINT4)input[j + 3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
 */

SSVU_INLINE void MD5_memcpy(
    POINTER output, CONST_POINTER input, unsigned int len)
{
    unsigned int i;
    for(i = 0; i < len; ++i) output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
 */
SSVU_INLINE void MD5_memset(POINTER output, int value, unsigned int len)
{
    unsigned int i;
    for(i = 0; i < len; ++i) ((char*)output)[i] = (char)value;
}
} // namespace Impl
} // namespace Encoding
} // namespace ssvu

#undef S11
#undef S12
#undef S13
#undef S14
#undef S21
#undef S22
#undef S23
#undef S24
#undef S31
#undef S32
#undef S33
#undef S34
#undef S41
#undef S42
#undef S43
#undef S44
#undef F
#undef G
#undef H
#undef I
#undef ROTATE_LEFT
#undef FF
#undef GG
#undef HH
#undef II
