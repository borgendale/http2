/*
 * Encode and decode http2 huffman strings
 */
#include "h2utils.h"
#include "h2huf.h"

uint32_t onevals [32] = {
    0x00000000,   /*  0 */
    0x00000001,   /*  1 */
    0x00000003,   /*  2 */
    0x00000007,   /*  3 */
    0x0000000f,   /*  4 */
    0x0000001f,   /*  5 */
    0x0000003f,   /*  6 */
    0x0000007f,   /*  7 */
    0x000000ff,   /*  8 */
    0x000001ff,   /*  9 */
    0x000003ff,   /* 10 */
    0x000007ff,   /* 11 */
    0x00000fff,   /* 12 */
    0x00001fff,   /* 13 */
    0x00003fff,   /* 14 */
    0x00007fff,   /* 15 */
    0x0000ffff,   /* 16 */
    0x0001ffff,   /* 17 */
    0x0003ffff,   /* 18 */
    0x0007ffff,   /* 19 */
    0x000fffff,   /* 20 */
    0x001fffff,   /* 21 */
    0x003fffff,   /* 22 */
    0x007fffff,   /* 23 */
    0x00ffffff,   /* 24 */
    0x01ffffff,   /* 25 */
    0x03ffffff,   /* 26 */
    0x07ffffff,   /* 27 */
    0x0fffffff,   /* 28 */
    0x1fffffff,   /* 29 */
    0x3fffffff,   /* 30 */
    0x7fffffff,   /* 31 */
};


static inline void h2_write4(uint32_t val, char * pos, int align) {
    if (align) {
        *(uint32_t *)pos = endian_int32(val);
    } else {
        uint32_t tint = val;
        memcpy(pos, &tint, 4);
    }
    pos += 4;
}

static inline void h2_writen(uint64_t val, char * pos, int n) {
    int i;
    int shift = (n-1)*8;
    for (i=0; i<n; i++) {
        *pos++ = (uint8_t)(val>>shift);
        shift -= 8;
    }

}

static inline uint32_t h2_read4(const char * pos, int align) {
    if (align) {
        return endian_int32(*(uint32_t *)pos);
    } else {
        uint32_t ret;
        memcpy(&ret, pos, 4);
        return endian_int32(ret);
    }
}

static inline uint32_t h2_readn(const char * pos, int n) {
    int i;
    uint32_t ret = 0;
    for (i=0; i<n; i++) {
        ret <<= 8;
        ret |= (uint8_t)*pos++;
    }
    return ret;
}



/*
 * Convert a string to huffman encoding
 * @return the number of bytes in the encoding or negative to indicate an error
 */
int h2_str2huf(const char * str, int slen, char * buf, int buflen) {
    char * inbuf = buf;
    int out_align = !((uintptr_t)buf & 3);  /* Check 4 byte alignment */
    uint64_t a = 0;
    int bits = 0;
    int left;

    if (slen < 0) {
        slen = (int)strlen(str);
    }

    while (slen--) {
        h2huf_t * tab = h2huftab + (uint8_t)*str++;
        if (tab->onelen) {
            a <<= tab->onelen;
            a |= onevals[tab->onelen];
            bits += tab->onelen;
        }
        a <<= tab->vallen;
        a |= tab->val;
        bits += tab->vallen;
        if (bits >= 32) {
            if (buflen >= 4) {
                h2_write4(a>>(bits-32), buf, out_align);
            }
            buf += 4;
            buflen -= 4;
            bits -= 32;
        }
    }
    if (bits&7) {
        int pad = 8-(bits&7);
        a <<= pad;
        a |= onevals[pad];
        bits += pad;
    }
    left = bits>>3;
    if (left <= buflen) {
        h2_writen(a, buf, left);
    }
    buf += left;
    return buf-inbuf;
}


/*
 * Convert a huffman encoding to a string
 * @return the number of bytes in the string or negative to indicate an error
 */
int h2_huf2str(const char * huf, int huflen, char * str, int slen) {
    char * instr = str;
    int in_align = !((uintptr_t)huf & 3);  /* Check 4 byte alignment */
    onetab_t * tab;
    uint64_t a = 0;
    uint64_t mask;
    int bits = 0;
    int onebits;
    int val;
    uint8_t code;

    while (huflen || bits) {
        if (bits < 32) {
            if (huflen >= 4) {
                bits += 32;
                a <<= 32;
                a |= h2_read4(huf, in_align);
                huf += 4;
                huflen -= 4;
            } else {
                if (huflen) {
                    bits += 8*huflen;
                    a <<= 8*huflen;
                    a |= h2_readn(huf, huflen);
                    huflen = 0;
                }
            }
        }
        mask = 1;
        mask <<= (bits-1);
        onebits = 0;
        while (a&mask) {
            onebits++;
            mask = mask >> 1;
        }
        if (onebits > 29 || bits == onebits) {
            if (onebits > 7)
                return -1;
        }
        bits -= onebits;
        if (!bits)
            break;
        tab = h2h_onetab + onebits;
        if (bits < tab->vallen) {
            return -2;
        }
        val = a>>(bits-tab->vallen) & onevals[tab->vallen];
        bits -= tab->vallen;
        if (val > tab->valmax) {
            if (bits < 1) {
                return -2;
            }
            val <<= 1;
            if ((a>>(bits-1))&1) {
                val |= 1;
            }
            bits--;
            if (onebits == 15 && val>13) {
                if (bits < 1) {
                    return -2;
                }
                val <<= 1;
                if ((a>>(bits-1))&1) {
                    val |= 1;
                }
                bits--;
            }
        }
        code = h2h_vals[tab->valoffset + val];
        if (slen) {
            *str = (char)code;
        }
        str++;
        slen--;
    }
    if (slen) {
        *str = 0;
    }
    return str-instr;
}


