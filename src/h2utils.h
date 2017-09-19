
#ifndef __H2UTILS_DEFINED
#define __H2UTILS_DEFINED


/* These interfaces are defined in C */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/* Windows */
#ifdef _WIN32
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned __int64 uint64_t;
#define inline __inline
#define endian_int16(x)  _byteswap_ushort((uint16_t)(x))
#define endian_int32(x)  _byteswap_ulong((uint32_t)(x))
#define endian_int64(x)  _byteswap_uint64((uint64_t)(x))

/* Linux */
#else
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <byteswap.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
   #define F_LITTLE_ENDIAN
   #define endian_int16(x)  bswap_16(x)
   #define endian_int32(x)  bswap_32(x)
   #define endian_int64(x)  bswap_64(x)
#else
   #define F_BIG_ENDIAN
#endif
#endif

#ifdef F_BIG_ENDIAN
  #define endian_int16(x)  x
  #define endian_int32(x)  x
  #define endian_int64(x)  x
#endif

#ifndef XAPI
    #define XAPI extern
#endif
#ifndef xUNUSED
    #if defined(__GNUC__)
        #define xUNUSED __attribute__ (( unused ))
    #else
        #define xUNUSED
    #endif
#endif

#define XINLINE __inline__

#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <time.h>

/*
 * Structure for allocated memory for a result buffer.
 * This is designed so that the buffer can be put on the stack, but if it overflows
 * it will be reallocated on the heap.
 */
typedef struct h2_buffer_t {
    char * buf;                /**< The buffer which can be reallocated    */
    int    len;                /**< The allocated length                   */
    int    used;               /**< The used length of the buffer          */
    int    pos;                /**< Buffering options                      */
    char   inheap;             /**< buf is in the heap and must be freed   */
    char   compact;            /**< json compact: 0=normal, 1=compact, 3=very compact */
    char   resv[2];
} h2_buffer_t;


/*
 * Free any heap memory associated with a buffer.
 *
 * This should be called if there is any chance the buffer has been moved to the heap.
 * This can be checked by looking at the inheap flag.
 *
 * @param buf  The buffer
 */
void h2_buffer_free(h2_buffer_t * buf);


/*
 * Ensure the buffer capacity
 *
 * @param buf   The buffer
 * @param len   The number of bytes needed
 *
 */
void h2_buffer_ensure(h2_buffer_t * buf, int len);

/*
 * Allocate bytes in a buffer.
 *
 * Bytes are allocated sequentially in the buffer.  If aligned is specified then
 * pad bytes are added to get to a boundary.  The allocated memory is freed when
 * the buffer is freed.
 *
 * @param buf  The buffer
 * @param len  The number of bytes
 * @param aligned Alignment 0=byte, 1=8byte
 * @param
char * h2_buffer_alloc(h2_buffer_t * buf, int len, int aligned);

/*
 * Put bytes into a buffer.
 *
 * @param buf   The buffer
 * @param bytes The location of the data to put
 * @param len   The length of the data to put
 *
 */
void h2_buffer_putBytes(h2_buffer_t * buf, const char * bytes, int len);


/*
 * Put a string into a buffer
 *
 * @param buf    The buffer
 * @param str    A null terminated string
 */
void h2_buffer_putString(h2_buffer_t * buf, const char * str);


/*
 * Put one character into a buffer
 *
 * @param buf    The buffer
 * @param ch     The character
 */
#define h2_buffer_put(buf, ch) \
    if ((buf)->used >= (buf)->len) \
        h2_buffer_ensure((buf), 1); \
    (buf)->buf[(buf)->used++] = (char)(ch);


/*
 * Get a string from a buffer with allocation.
 * The string is copied to a user supplied buffer.  If the length is greater
 * or equal to the supplied buffer, then return an allocated value.
 * @param buf     The buffer containing the hpack
 * @param retbuf  The buffer to return the string
 * @param retlen  The length of the return buffer
 */
const char * h2_hpack_getString(h2_buffer_t * buf, char * retbuf, int retlen);


/*
 * Get a string from a buffer with truncation
 * The string is copied to a user supplied buffer.  If the returned length is
 * greater or equal to the supplied buffer, then the string is truncated.
 * @param buf     The buffer containing the hpack
 * @param retbuf  The buffer to return the string
 * @param retlen  The length of the return buffer
 */
int h2_hpack_getStringBuf(h2_buffer_t * buf, char * retbuf, int retlen);


/*
 * Put a string to a buffer
 * @param buf   The buffer
 * @param str   The string to put
 * @param len   The length (or -1 to use a null terminated string)
 * @param huff  Set to use huffman encoding of the string
 */
int h2_hpack_putString(h2_buffer_t * buf, const char * str, int len, int huff);

/*
 * Put an HPACK integer into a buffer
 */
int h2_hpack_putInt(h2_buffer_t * buf, uint32_t value, int bits, int upper);


/*
 * Get an HPACK integer
 */
int h2_hpack_getInt(h2_buffer_t * buf, uint32_t * value, int bits, int * upper);

/*
 *
 */
int h2_huf2str(const char * huf, int huflen, char * str, int slen);


/*
 *
 */
int h2_str2huf(const char * str, int slen, char * buf, int buflen) ;


#ifdef __cplusplus
}
#endif

#endif
