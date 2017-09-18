/*
 * Create http2 huffman encodeing tables from table in the hpack spec
 *
 * Ken Borgendale 2017-06-30
 */
#include <stdio.h>
#include <string.h>

typedef unsigned char uint8_t;

typedef struct onetab_t {
    uint8_t vallen;
    uint8_t valmax;
    uint8_t vals [32];
} onetab_t;

int main (int argc, char * * argv) {
    int i = 0;
    int j;
    int which;
    int onelen;
    int val;
    int vallen;
    char line [256];
    char * lp;
    uint8_t onelens [256] = {0};
    uint8_t vallens [256] = {0};
    uint8_t vals [256] = {0};
    onetab_t onetab [32] = {0};
    uint8_t valtab [1024];
    int     valpos;


    FILE * f = fopen("h2huff.txt", "r");
    if (!f) {
        printf("h2huff.txt not found\n");
        return 1;
    }

    printf("\n/*\n * Define string to h2huf mapping\n */\n");
    printf("typedef struct h2huf_t {\n");
    printf("    uint8_t  onelen;\n");
    printf("    uint8_t  vallen;\n");
    printf("    uint8_t  val;\n");
    printf("    uint8_t  code;\n");
    printf("} h2huf_t;\n\n");
    printf("\n/*\n * Define h2huf to string mapping\n */\n");
    printf("typedef struct onetab_t {\n");
    printf("    uint8_t  vallen;\n");
    printf("    uint8_t  valmax;\n");
    printf("    uint16_t valoffset;\n");
    printf("} onetab_t;\n\n");

    printf("/*\n * Map used for string to h2huf\n */\n");
    printf("h2huf_t h2huftab [256] = {\n");
    lp = fgets(line, sizeof line, f);
    while (lp) {
        if (!strchr(line, '|') || strlen(line)<6)
            continue;
        lp = strchr(line+3, '(');
        if (!lp)
            continue;
        which = atoi(lp+1);
        if (which != i) {
            printf("error at index %d which=%d line=%s\n", i, which, line);
            return 2;
        }
        lp = strchr(line+3, '|');
        onelen = 0;
        val = 0;
        vallen = 0;

        while (*lp=='|' || *lp=='1') {
            if (*lp == '1') {
                onelen++;
            }
            lp++;
        }
        while (*lp=='0' || *lp=='1' || *lp=='|') {
            if (*lp!='|') {
                val <<= 1;
                if (*lp=='1')
                    val |= 1;
                vallen++;
            }
            lp++;
        }
        lp = strchr(line+3, '[');
        if (!lp) {
            printf("len error at index %d\n", i);
            return 3;
        } else {
            int xlen = atoi(lp+1);
            if (xlen != (onelen+vallen)) {
                printf("len not matched at index %d (computed=%d, file=%d)\n", i, (onelen+vallen), xlen);
            }
        }
        line[3] = 0;
        if (i<256) {
            onelens[i] = onelen;
            vallens[i] = vallen;
            vals[i] = val;
            printf("    { %3d, %3d, %3d,  0x%02x },   /* %3d %3s */\n", onelen, vallen, val, i, i, line);

        }
        i++;
        lp = fgets(line, sizeof line, f);
    }
    printf("};\n\n");
    printf("/*\n * Map used for string to h2huf\n */\n");

    for (i=0; i<256; i++) {
        onetab_t * tab = onetab + onelens[i];
        if (!tab->vallen)
            tab->vallen = vallens[i];
        if (tab->vallen > vallens[i]) {
            tab->vallen = vallens[i];
        }
        if (tab->vallen == vallens[i]) {
            tab->valmax = vals[i];
        }
        tab->vals[vals[i]] = (uint8_t)i;
    }

    printf("onetab_t h2h_onetab[] = {\n");
    valpos = 0;
    for (i=0; i<30; i++) {
        onetab_t * tab = onetab + i;
        int valcount;
        for (valcount=32; valcount>0; valcount--) {
            if (tab->vals[valcount-1])
                break;
        }
        memcpy(valtab+valpos, tab->vals, valcount);
        printf("    {%3d, %3d,  0x%04x }, /* ones=%-2d count=%-2d */\n", tab->vallen, tab->valmax, valpos, i, valcount);
        valpos += valcount;
    }
    printf("};\n");

    printf("\n/*\n * Value table for h2huf to string mapping\n */\n");
    printf("uint8_t h2h_vals [%d] = {", valpos);
    for (j=0; j<valpos; j++) {
        if (j%16 == 0) {
            printf("\n    /* %03x */  ", j);
        }
        printf("0x%02x, ", valtab[j]);
    }
    printf("\n};\n");

}
