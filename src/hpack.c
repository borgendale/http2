#include "h2utils.h"
#include "hpack.h"

/*
 * HTTP/2 defines the entry to have 32 bytes of overhead.  We use this a two pointer,
 * two short length fields, two null bytes, and pad of up to 7 bytes as we keep the entries
 * 8 byte aligned.
 */
typedef struct h2_entry_t {
    struct h2_entry_t * next;
    struct h2_entry_t * prev;
    uint16_t hdrlen;
    uint16_t valuelen;
    uint8_t  inheap;
    char     hdr[11];
} h2_entry_t;


/*
 * The HTTP/2 hpack context.
 * This describes the dynamic table and the options for encoding or decoding.
 * A particular context can only be used for either encoding or decoding but not both.
 * The encoder defines what goes into the dynamic table, and the decoder must match this
 * table exactly.
 */
struct h2_context_t {
    char *   dyntab;                      /* The location of the base table*/
    h2_entry_t * head;                    /* The head (62) entry*/
    h2_entry_t * tail;
    uint32_t alloc_size;                  /* The number of bytes allocated */
    uint32_t declare_size;                /* The declared size of the table */
    uint32_t used_size;                   /* The used size of the table */
    uint32_t current_size;                /* The crrent size (can be reduced by encoder) */
    uint32_t max_entry_size;              /* The max entry size for encode */
    uint32_t entries;                     /* The number of entries */
    uint8_t  encode_opt;                  /* Encodeer options */
    uint8_t  decode_opt;                  /* Decode options */
    uint8_t  usehuff;                     /* Huffman options (oonly used in encoder) */
    uint8_t  encode;                      /* 0=decode 1=encode */
};


/*
 * Internal functions
 */
int hpack_reduceDynamic(h2_context_t * h2ctx, int size);
int hpack_changeDynamic(h2_context_t * h2ctx, int size);
int hpack_canonicalize(char * buf, int len);
int hpack_pushDynamic(h2_context_t * h2ctx, const char * hdr, const char * value);
int hpack_lookupDynamic(h2_context_t * h2ctx, const char * hdr, const char * value);
int hpack_lookupStatic(const char * hdr, const char * value);
const char * hpack_getHeader(h2_context_t * h2ctx, int inx);
const char * hpack_getValue(h2_context_t * h2ctx, int inx);


/*
 * HPACK static table header values
 */
const char * h2_static_val[17] = {
    "",                          /*   0   */
    "",                          /*   1   */
    "GET",                       /*   2   */
    "POST",                      /*   3   */
    "/",                         /*   4   */
    "/index.html",               /*   5   */
    "http",                      /*   6   */
    "https",                     /*   7   */
    "200",                       /*   8   */
    "204",                       /*   9   */
    "206",                       /*  10   */
    "304",                       /*  11   */
    "400",                       /*  12   */
    "404",                       /*  13   */
    "500",                       /*  14   */
    "",                          /*  15   */
    "gzip, deflate",             /*  16   */
};


/*
 * HPACK static table header names
 */
const char * h2_static[63] = {
    NULL,
    ":authority",                        /*   1   */
    ":method",                           /*   2   */
    ":method",                           /*   3   */
    ":path",                             /*   4   */
    ":path",                             /*   5   */
    ":scheme",                           /*   6   */
    ":scheme",                           /*   7   */
    ":status",                           /*   8   */
    ":status",                           /*   9   */
    ":status",                           /*  10   */
    ":status",                           /*  11   */
    ":status",                           /*  12   */
    ":status",                           /*  13   */
    ":status",                           /*  14   */
    "accept-charset",                    /*  15   */
    "accept-encoding",                   /*  16   */
    "accept-language",                   /*  17   */
    "accept-ranges",                     /*  18   */
    "accept",                            /*  19   */
    "access-control-allow-origin",       /*  20   */
    "age",                               /*  21   */
    "allow",                             /*  22   */
    "authorization",                     /*  23   */
    "cache-control",                     /*  24   */
    "content-disposition",               /*  25   */
    "content-encoding",                  /*  26   */
    "content-language",                  /*  27   */
    "content-length",                    /*  28   */
    "content-location",                  /*  29   */
    "content-range",                     /*  30   */
    "content-type",                      /*  31   */
    "cookie",                            /*  32   */
    "date",                              /*  33   */
    "etag",                              /*  34   */
    "expect",                            /*  35   */
    "expires",                           /*  36   */
    "from",                              /*  37   */
    "host",                              /*  38   */
    "if-match",                          /*  39   */
    "if-modified-since",                 /*  40   */
    "if-none-match",                     /*  41   */
    "if-range",                          /*  42   */
    "if-unmodified-since",               /*  43   */
    "last-modified",                     /*  44   */
    "link",                              /*  45   */
    "location",                          /*  46   */
    "max-forwards",                      /*  47   */
    "proxy-authenticate",                /*  48   */
    "proxy-authorization",               /*  49   */
    "range",                             /*  50   */
    "referer",                           /*  51   */
    "refresh",                           /*  52   */
    "retry-after",                       /*  53   */
    "server",                            /*  54   */
    "set-cookie",                        /*  55   */
    "strict-transport-security",         /*  56   */
    "transfer-encoding",                 /*  57   */
    "user-agent",                        /*  58   */
    "vary",                              /*  59   */
    "via",                               /*  60   */
    "www-authenticate",                  /*  61   */
    "",                                  /* end of table */
};


/*
 * Create a new h2 context
 * @param  size      The size of the dynamic table
 * @param  encode    Use of context 0=decode, 1=encode
 * @param  maxentry  The size of the largest entry allowed
 * @param  options   The encoder or decoder options
 * @param  huf       Use huffman encoding
 */
h2_context_t * hpack_newContext(int size, int encode, int maxentry, int options, int huff) {
    h2_context_t * h2ctx;
    if (size < 0 || size > 256*1024 || maxentry<0 || maxentry > 32*1024) {
        return NULL;
    }

    h2ctx = malloc(sizeof(h2_context_t) + size + maxentry);
    memset(h2ctx, 0, sizeof(h2_context_t));
    h2ctx->encode = !!encode;
    h2ctx->dyntab = (char *)(h2ctx+1);
    h2ctx->alloc_size = size + maxentry;
    h2ctx->declare_size = size;
    h2ctx->current_size = size;
    h2ctx->max_entry_size = maxentry;
    h2ctx->usehuff = (uint8_t)huff;
    if (h2ctx->encode) {
        h2ctx->encode_opt = (uint8_t)options;
    } else {
        h2ctx->encode_opt = (uint8_t)H2ENCODE_MAX;
        h2ctx->decode_opt = (uint8_t)options;
    }
    return h2ctx;
}

/*
 * Free the hpack context
 * @param h2ctx  The hpack context
 */
void hpack_freeContext(h2_context_t * h2ctx) {
    free(h2ctx);
}


/*
 * Get the sizes of the hpack context
 *
 * If the output paramaters are allowed to be NULL and if so no value is returned.
 *
 * @param h2ctx   The hpack context
 * @param maxsize The max size of the dynamic table (output)
 * @param currentsize The current size of the dynamic table (output)
 * @param usedsize The number of bytes actually used in the dynamic table (output)
 * @param return The number of entries in the dynamic table
 */
int hpack_getContextStats(h2_context_t * h2ctx, int * maxsize, int * currentsize, int * usedsize) {
    if (maxsize)
        *maxsize = h2ctx->declare_size;
    if (currentsize)
        *currentsize = h2ctx->current_size;
    if (usedsize)
        *usedsize = h2ctx->used_size;
    return h2ctx->entries;
}


/*
 * Reduce the size of the dynmic table so enough space is available
 */
int hpack_reduceDynamic(h2_context_t * h2ctx, int size) {
    h2_entry_t * ent;
    while (h2ctx->tail && size > (int)(h2ctx->current_size - h2ctx->used_size)) {
        int entsize;
        ent = h2ctx->tail;
        entsize = 32 + ent->hdrlen + ent->valuelen;
        if (ent->prev == NULL) {
            /* Unlink the last entry */
            h2ctx->head = NULL;
            h2ctx->tail = NULL;
            h2ctx->used_size = 0;
            h2ctx->entries = 0;
        } else {
            /* Unlink one entry */
            h2ctx->tail = ent->prev;
            ent->prev->next = NULL;
            h2ctx->used_size -= entsize;
            h2ctx->entries--;
        }
    }
    return 0;
}


/*
 * Change the size of the dynamic table
 */
int hpack_changeDynamic(h2_context_t * h2ctx, int size) {
    if (size > h2ctx->declare_size || size < 0) {
        return -1;
    }

    h2ctx->current_size = size;
    if (size == 0) {
        h2ctx->head = NULL;
        h2ctx->tail = NULL;
        h2ctx->entries = 0;
        h2ctx->used_size = 0;
    } else {
        if (h2ctx->used_size > size) {
            hpack_reduceDynamic(h2ctx, 0);
        }
    }
    return 0;
}


/*
 * Canonicalize the http/2 headers
 * 1. Remove all optional white space
 * 2. Compress required white space to one space
 * 3. Change CRLF to LF
 * 4. Lowercase header names
 */
int hpack_canonicalize(char * buf, int len) {
    char * in = buf;
    char * out = buf;
    int    inhdr = 1;
    int    lastchar = 0;
    int    wasspace = 0;
    int    i;

    for (i=0; i<len; i++) {
        char ch = *in++;
        /* Handle control characters and space */
        if ((uint8_t)ch <= ' ') {
            if (inhdr) {
                return -3;
            }
            if (ch == ' ' || ch == '\t') {
                ch = ' ';
                if (lastchar == '\n') {
                    if (!wasspace) {
                        *out++ = ' ';
                        wasspace = 1;
                    }
                    lastchar = ch;
                    continue;
                }
                if (!wasspace) {
                    *out++ = ' ';
                    wasspace = 1;
                }
            } else if (ch == '\r') {
                if (lastchar == '\n')
                    *out++ = '\n';
            } else if (ch == '\n') {
            } else {
                return -2;              /* Invalid control char      */
            }
        }
        /* All characters above space */
        else {
            if (ch == ':' && inhdr > 1) {
                *out++ = ch;
                wasspace = 1;
                inhdr = 0;
            } else {
                wasspace = 0;
                if (inhdr)
                    inhdr++;
                if (lastchar == '\n') {
                    *out++ = '\n';
                    inhdr = 1;
                }
                *out++ = inhdr ? tolower(ch) : ch;
            }
        }
        lastchar = ch;
    }
    if (lastchar == '\n')
        *out++ = '\n';
    *out = 0;
    if (!strchr(buf, '\n')) {
        return -4;
    }
    return out-buf;
}


/*
 * Encode an http/2 header
 *
 * @param h2ctx  The http/2 context
 * @param src    The source compressed header.  This will be modified by this method.
 * @param slen   The length of the source
 * @param buf    The output buffer
 * @return A return code, 0=good
 */
int hpack_encode(h2_context_t * h2ctx, char * src, int slen, h2_buffer_t * buf) {
    int  rc;
    char * hdr;
    char * next;
    char * value;
    int    inx;
    int    inx2;
    int    idx;

    rc = hpack_canonicalize(src, slen);
    if (rc < 0)
        return rc;

    slen = rc;
    hdr = src;
    next = strchr(hdr, '\n');
    if (next)
        *next = 0;
    while (*hdr) {
        value = strchr(hdr+1, ':');
        if (!value)
            return -6;
        *value++ = 0;
        inx = 0;
        inx2 = 0;
        if (h2ctx->encode_opt >= H2ENCODE_STATIC) {
            inx = hpack_lookupStatic(hdr, value);
        }
        if ((!inx || !(inx&NOLITERAL)) && (h2ctx->encode_opt >= H2ENCODE_MIN)) {
            inx2 = hpack_lookupDynamic(h2ctx, hdr, value);
            if (!inx || (inx2&NOLITERAL)) {
                inx = inx2;
            }
        }
        if (inx & NOLITERAL) {
            h2_hpack_putInt(buf, inx&0xffffff, 7, 0x80);   /* indexed field */
            //printf("enocde %s=%d\n", hdr, inx&0xffffff);
        } else {
            idx = hpack_pushDynamic(h2ctx, hdr, value);
            if (idx) {
                if (inx == 0) {
                    h2_buffer_put(buf, 0x40);
                    h2_hpack_putString(buf, hdr, -1, h2ctx->usehuff);
                    //printf("encode add0 %s: %s\n", hdr, value);
                } else {
                    h2_hpack_putInt(buf, inx, 6, 0x40);
                    //printf("encode addx %s=%d: %s\n", hdr, inx, value);
                }
            } else {
                if (inx == 0) {
                    h2_buffer_put(buf, 0x00);
                    h2_hpack_putString(buf, hdr, -1, h2ctx->usehuff);
                    //printf("encode 0 %s: %s\n", hdr, value);
                } else {
                    h2_hpack_putInt(buf, inx, 4, 0x00);
                    //printf("encode x %s=%d: %s\n", hdr, inx, value);
                }
            }
            h2_hpack_putString(buf, value, -1, h2ctx->usehuff);
        }


        hdr = next+1;
        next = strchr(hdr, '\n');
        if (next)
            *next = 0;
    }
    return 0;

}

/*
 * Decode an hpack header
 *
 * @param h2ctx  The hpack context
 * @param src    The source compressed header.
 * @param slen   The length of the source
 * @param buf    The output buffer
 * @return A return code, 0=good
 */
int hpack_decode(h2_context_t * h2ctx, const char * src, int slen, h2_buffer_t * buf) {
    char hdrbuf [512];
    char valbuf [4096];
    uint8_t freehdr = 0;
    uint8_t freeval = 0;
    const char * nl = "\n";

    h2_buffer_t sbuf = {(char *)src, slen, slen};
    while (sbuf.pos < slen) {
        int      rc;
        int      upper;
        uint32_t index;
        const char * hdr;
        const char * value;

        rc = h2_hpack_getInt(&sbuf, &index, 0, &upper);
        if (rc < 0) {
            return rc;
        } else {
            if (upper & 0x80) {
                /* Indexed header */
                hdr = hpack_getHeader(h2ctx, index);
                value = hpack_getValue(h2ctx, index);
            } else if (upper & 0x40) {
                /* Dynamic add */
                if (index == 0) {
                    hdr = h2_hpack_getString(&sbuf, hdrbuf, sizeof hdrbuf);
                    freehdr = (char *)hdr != hdrbuf;
                } else {
                    hdr = hpack_getHeader(h2ctx, index);
                }
                value = h2_hpack_getString(&sbuf, valbuf, sizeof valbuf);
                freeval = (char *)value != valbuf;
                rc = hpack_pushDynamic(h2ctx, hdr, value);
                /* TODO: error handling */
            } else if (upper & 0x20) {
                /* Update dynamic table size */
                hpack_changeDynamic(h2ctx, index);
                continue;
            } else {
                /* Literal */
                if (index == 0) {
                    hdr = h2_hpack_getString(&sbuf, hdrbuf, sizeof hdrbuf);
                    freehdr = (char *)hdr != hdrbuf;
                } else {
                    hdr = hpack_getHeader(h2ctx, index);
                }
                value = h2_hpack_getString(&sbuf, valbuf, sizeof valbuf);
                freeval = (char *)value != valbuf;
            }

            /* Write the line  */
            h2_buffer_putString(buf, hdr);
            h2_buffer_putString(buf, (h2ctx->decode_opt==H2DECODE_SPACE) ? ": " : ":");
            h2_buffer_putString(buf, value);
            h2_buffer_putString(buf, nl);
            //printf("decode %s=%s\n", hdr, value);

            /* Free if the hdr or value are in the heap */
            if (freehdr)
                free((char *)hdr);
            if (freeval)
                free((char *)value);
        }
        h2_buffer_put(buf, 0);
        buf->used--;
    }
    return 0;
}



/*
 * Push an entry into the dynmaic table.
 * If the entry is not pushed return 0 to say use the literal
 * If the entry is pushed, it will always be at position 62.
 */

int hpack_pushDynamic(h2_context_t * h2ctx, const char * hdr, const char * value) {
    int where;
    int hdrlen = (int)strlen(hdr);
    int vallen = (int)strlen(value);
    int entlen = 32+hdrlen+vallen;
    h2_entry_t * ent;

    /*
     * If the entry does not fit in the table, or we choose to not use dynamic, return 0
     */
    if (entlen > h2ctx->current_size || entlen > h2ctx->max_entry_size || h2ctx->encode_opt < H2ENCODE_MIN) {
        return 0;
    }
    if (h2ctx->encode_opt == H2ENCODE_MIN && entlen > (h2ctx->current_size - h2ctx->used_size)) {
        return 0;
    }

    /* Throw out enough old entries that we have space */
    hpack_reduceDynamic(h2ctx, entlen);

    /* Insert a new entry */
    if (h2ctx->head) {
        ent = h2ctx->head;
        where = ((char *)h2ctx->head) - h2ctx->dyntab + 30 + ent->hdrlen + ent->valuelen;
        where &= ~7;
        if (where + entlen > h2ctx->alloc_size) {
            where = 0;
        }
    } else {
        where = 0;
    }
    ent = (h2_entry_t *)(h2ctx->dyntab + where);
    ent->next = h2ctx->head;
    ent->prev = NULL;
    if (h2ctx->head) {
        h2ctx->head->prev = ent;
    } else {
        h2ctx->tail = ent;
    }
    ent->hdrlen = hdrlen;
    ent->valuelen = vallen;
    strcpy(ent->hdr, hdr);
    strcpy(ent->hdr+hdrlen+1, value);
    h2ctx->head = ent;
    h2ctx->used_size += entlen;
    h2ctx->entries++;
    return 62;
}


/*
 * Lookup in the dynamic table.
 *
 * If we get an exact match we use that.  If we get only a header name match we keep
 * looking and if there is no exact match we return the name match.
 */
int hpack_lookupDynamic(h2_context_t * h2ctx, const char * hdr, const char * value) {
    int partial = 0;
    h2_entry_t * ent;
    int hdrlen;
    int which;

    if (!h2ctx->head)
        return 0;

    hdrlen = (int)strlen(hdr);
    ent = h2ctx->head;
    which = 62;
    while (ent) {
        if (hdrlen == ent->hdrlen) {
            if (!memcmp(hdr, ent->hdr, hdrlen)) {
                if (!partial) {
                    partial = which;
                }
                if (!strcmp(ent->hdr+hdrlen+1, value)) {
                    return which|NOLITERAL;
                }
            }
        }
        ent = ent->next;
        which++;
    }
    return partial;
}


/*
 * Lookup in the static table
 */
int hpack_lookupStatic(const char * hdr, const char * value) {
    int idx;
    switch (*hdr) {
    case ':':  idx = 1;    break;
    case 'a':  idx = 15;   break;
    case 'c':  idx = 24;   break;
    case 'd':  idx = 33;   break;
    case 'e':  idx = 34;   break;
    case 'f':  idx = 37;   break;
    case 'h':  idx = 38;   break;
    case 'i':  idx = 39;   break;
    case 'l':  idx = 44;   break;
    case 'm':  idx = 47;   break;
    case 'p':  idx = 48;   break;
    case 'r':  idx = 50;   break;
    case 's':  idx = 54;   break;
    case 't':  idx = 57;   break;
    case 'u':  idx = 58;   break;
    case 'v':  idx = 59;   break;
    case 'w':  idx = 61;   break;
    default:   return 0;
    };
    while (*h2_static[idx] == *hdr) {
        if (!strcmp(h2_static[idx], hdr)) {
            if (idx <= 16) {
                switch (idx) {
                case 2:  /* :method */
                    if (!strcmp(value, "GET"))
                        return NOLITERAL + 2;
                    if (!strcmp(value, "POST"))
                        return NOLITERAL + 3;
                    break;
                case 4:  /* :path */
                    if (!strcmp(value, "/"))
                        return NOLITERAL + 4;
                    if (!strcmp(value, "/index.html"))
                        return NOLITERAL + 5;
                    break;
                case 6:  /* :scheme */
                    if (!strcmp(value, "http"))
                        return NOLITERAL + 6;
                    if (!strcmp(value, "https"))
                        return NOLITERAL + 7;
                    break;
                case 8:  /* :status */
                    if (!strcmp(value, "200"))
                        return NOLITERAL + 8;
                    if (!strcmp(value, "204"))
                        return NOLITERAL + 9;
                    if (!strcmp(value, "206"))
                        return NOLITERAL + 10;
                    if (!strcmp(value, "304"))
                        return NOLITERAL + 11;
                    if (!strcmp(value, "400"))
                        return NOLITERAL + 12;
                    if (!strcmp(value, "404"))
                        return NOLITERAL + 13;
                    if (!strcmp(value, "500"))
                        return NOLITERAL + 14;
                    break;
                case 16:
                    if (!strcmp(value, "gzip, deflate"))
                        return NOLITERAL + 16;
                    break;
                default:
                    break;
                }
            }
            return idx;
        }
        idx++;
    }
    return 0;
}


/*
 *
 */
const char * hpack_getHeader(h2_context_t * h2ctx, int inx) {
    if (inx < 62) {
        return h2_static[inx];
    } else {
        int where = 62;
        h2_entry_t * ent = h2ctx->head;
        while (ent && where < inx) {
            ent = ent->next;
        }
        return (const char *)ent->hdr;
    }
    return NULL;
}


/*
 *
 */
const char * hpack_getValue(h2_context_t * h2ctx, int inx) {
    if (inx < 62) {
        if (inx < 17) {
            return h2_static_val[inx];
        } else {
            return "";
        }
    } else {
        int where = 62;
        h2_entry_t * ent = h2ctx->head;
        while (ent && where < inx) {
            ent = ent->next;
        }
        return (const char *)(ent->hdr + ent->hdrlen + 1);
    }
}


