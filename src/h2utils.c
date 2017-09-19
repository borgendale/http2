
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define TRACE_COMP Util
#include "h2utils.h"


/*
 * Free an allocation buffer
 */
void h2_buffer_free(h2_buffer_t * buf) {
    if (buf->inheap) {
        if (buf->buf)
            free(buf->buf);
        buf->buf = NULL;
        buf->len = 0;
        buf->inheap = 0;
    }
}


/*
 * Ensure the buffer capacity
 *
 * @param buf   The buffer
 * @param len   The number of bytes needed
 *
 */
void h2_buffer_ensure(h2_buffer_t * buf, int len) {
    if (buf->used + len  > buf->len) {
        int newsize = 64*1024;
        while (newsize < buf->used + len)
            newsize *= 2;
        if (buf->inheap) {
            char * tmp = realloc(buf->buf, newsize);
            if (tmp)
                buf->buf = tmp;
            else
                return;
        } else {
            char * tmpbuf = malloc(newsize);
            if (tmpbuf && buf->used)
                memcpy(tmpbuf, buf->buf, buf->used);
            buf->buf = tmpbuf;
        }
        if (!buf->buf)
            return;
        buf->inheap = 1;
        buf->len = newsize;
    }
}


/*
 * Allocate space in an allocation buffer
 */
char * h2_buffer_alloc(h2_buffer_t * buf, int len, int aligned) {
    char * ret;
    if (buf->used + len + 7 > buf->len) {
        h2_buffer_ensure(buf, len+7);
    }
    ret = buf->buf + buf->used;
    if (aligned) {
        ret = (char *)(((uintptr_t)(ret+7))&~7);
    }
    buf->used += len;
    return ret;
}

/*
 * Put a byte array into a buffer
 */
void h2_buffer_putBytes(h2_buffer_t * buf, const char * newbuf, int len) {
    if (buf->used + len  > buf->len) {
        h2_buffer_ensure(buf, len);
    }
    memcpy(buf->buf + buf->used, newbuf, len);
    buf->used += len;
}


/*
 * Put a string into a buffer
 */
void h2_buffer_putString(h2_buffer_t * buf, const char * newbuf) {
    int len = (int)strlen(newbuf);
    if (buf->used + len  > buf->len) {
        h2_buffer_ensure(buf, len);
    }
    memcpy(buf->buf + buf->used, newbuf, len);
    buf->used += len;
}


/*
 * Bitmask for max value based on N
 */
static int  maxvals[9] = {0, 1, 3,  7, 15, 31, 63, 127, 255};


/*
 * Put an HPACK integer into a buffer
 */
int h2_hpack_putInt(h2_buffer_t * buf, uint32_t value, int bits, int upper) {
    int maxval;

    if (bits<1 || bits > 8)
        return -1;
    maxval = maxvals[bits];
    upper &= (255-maxval);

    if (buf->used + 6 > buf->len) {    /* 6 is the max size value we can encode */
        h2_buffer_ensure(buf, 6);
    }

    if (value < maxval) {
        buf->buf[buf->used++] = (char)(upper | value);
    } else {
        h2_buffer_put(buf, (char)(upper | maxval));
        while (value > 128) {
            buf->buf[buf->used++] = (char)(0x80 | (value&0x7f));
            value >>= 7;
        }
        buf->buf[buf->used++] = (char)value;
    }
    return 0;
}


/*
 * Get an HPACK integer
 */
int h2_hpack_getInt(h2_buffer_t * buf, uint32_t * value, int bits, int * upper) {
    int      maxval;
    uint32_t val;
    int      startpos;
    int      shift;
    char     ch;

    if (bits < 0 || bits > 8)
        return -2;
    if (buf->pos < buf->used) {
        ch = buf->buf[buf->pos++];
        if (bits == 0) {
            if (ch & 0x80)
                bits = 7;
            else if (ch & 0x40)
                bits = 6;
            else if (ch & 0x20)
                bits = 5;
            else
                bits = 4;
        }
        maxval = maxvals[bits];
        if (upper) {
            *upper = (ch & (255-maxval));
        }
        if ((ch&maxval) != maxval) {
            *value = ch&maxval;
            return 1;
        }
        val = 0;
        shift = 0;
        startpos = buf->pos-1;
        while (buf->pos < buf->used) {
            ch = buf->buf[buf->pos++];
            val += (ch&0x7f) << shift;
            if (ch&0x80) {
                shift += 7;
            } else {
                *value = val;
                return buf->pos - startpos;
            }
        }
    }
    return -1;
}


/*
 * Put a string to a buffer
 * @param buf   The buffer
 * @param str   The string to put
 * @param len   The length (or -1 to use a null terminated string)
 * @param huff  Set to use huffman encoding of the string
 */
int h2_hpack_putString(h2_buffer_t * buf, const char * str, int len, int huff) {
    char * abuf = NULL;
    char * hbuf;
    if (len < 0)
        len = (int)strlen(str);
    if (huff) {
        if (len < 4096) {
            hbuf = alloca(len*4);
        } else {
            abuf = hbuf = malloc(len*4);
        }
        len = h2_str2huf(str, len, hbuf, len*4);
        if (len < 0)
            return len;
        str = hbuf;
    }
    h2_hpack_putInt(buf, len, 7, huff ? 0x80 : 0);
    h2_buffer_putBytes(buf, str, len);
    if (abuf)
        free(abuf);
    return 0;
}


/*
 * Get a string from a buffer with truncation
 * The string is copied to a user supplied buffer.  If the returned length is
 * greater or equal to the supplied buffer, then the string is truncated.
 */
int h2_hpack_getStringBuf(h2_buffer_t * buf, char * retbuf, int retlen) {
    int rc;
    uint32_t slen;
    int upper;

    rc = h2_hpack_getInt(buf, &slen, 7, &upper);
    if (rc >= 0) {
        if (slen > buf->used+buf->pos) {
            return -3;
        }
        if (upper&0x80) {
            rc = h2_huf2str(buf->buf + buf->pos, slen, retbuf, retlen);
            if (rc >= 0)
                buf->pos += slen;
            return rc;
        } else {
            if (slen < retlen) {
                memcpy(retbuf, buf->buf + buf->pos, slen);
                retbuf[slen] = 0;
            } else {
                if (retlen > 1) {
                    memcpy(retbuf, buf->buf + buf->pos, retlen-1);
                    retbuf[retlen-1] = 0;
                }
            }
            buf->pos += slen;
            return slen;
        }
    }
    return rc;
}


/*
 * Get a string from a buffer with allocation.
 * The string is copied to a user supplied buffer.  If the length is greater
 * or equal to the supplied buffer, then return an allocated value.
 */
const char * h2_hpack_getString(h2_buffer_t * buf, char * retbuf, int retlen) {
    int rc;
    uint32_t slen;
    int upper;

    rc = h2_hpack_getInt(buf, &slen, 7, &upper);
    if (rc >= 0) {
        if (slen > buf->used+buf->pos) {
            return NULL;
        }
        if (upper&0x80) {
            int savepos = buf->pos;
            rc = h2_huf2str(buf->buf + buf->pos, slen, retbuf, retlen);
            if (rc < 0)
                return NULL;
            if (rc >= retlen) {
                buf->pos = savepos;
                retlen = rc+1;
                retbuf = malloc(retlen);
                rc = h2_huf2str(buf->buf + buf->pos, slen, retbuf, retlen);
            }
            buf->pos += slen;
            return retbuf;
        } else {
            if (slen >= retlen)
                retbuf = malloc(slen+1);
            memcpy(retbuf, buf->buf + buf->pos, slen);
            retbuf[slen] = 0;
            buf->pos += slen;
            return retbuf;
        }
    }
    return NULL;
}




