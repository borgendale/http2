
/*
 * Define string to h2huf mapping
 */
typedef struct h2huf_t {
    uint8_t  onelen;
    uint8_t  vallen;
    uint8_t  val;
    uint8_t  code;
} h2huf_t;


/*
 * Define h2huf to string mapping
 */
typedef struct onetab_t {
    uint8_t  vallen;
    uint8_t  valmax;
    uint16_t valoffset;
} onetab_t;

/*
 * Map used for string to h2huf
 */
h2huf_t h2huftab [256] = {
    {  10,   3,   0,  0x00 },   /*   0     */
    {  17,   6,  24,  0x01 },   /*   1     */
    {  23,   5,   2,  0x02 },   /*   2     */
    {  23,   5,   3,  0x03 },   /*   3     */
    {  23,   5,   4,  0x04 },   /*   4     */
    {  23,   5,   5,  0x05 },   /*   5     */
    {  23,   5,   6,  0x06 },   /*   6     */
    {  23,   5,   7,  0x07 },   /*   7     */
    {  23,   5,   8,  0x08 },   /*   8     */
    {  19,   5,  10,  0x09 },   /*   9     */
    {  28,   2,   0,  0x0a },   /*  10     */
    {  23,   5,   9,  0x0b },   /*  11     */
    {  23,   5,  10,  0x0c },   /*  12     */
    {  28,   2,   1,  0x0d },   /*  13     */
    {  23,   5,  11,  0x0e },   /*  14     */
    {  23,   5,  12,  0x0f },   /*  15     */
    {  23,   5,  13,  0x10 },   /*  16     */
    {  23,   5,  14,  0x11 },   /*  17     */
    {  23,   5,  15,  0x12 },   /*  18     */
    {  24,   4,   0,  0x13 },   /*  19     */
    {  24,   4,   1,  0x14 },   /*  20     */
    {  24,   4,   2,  0x15 },   /*  21     */
    {  29,   1,   0,  0x16 },   /*  22     */
    {  24,   4,   3,  0x17 },   /*  23     */
    {  24,   4,   4,  0x18 },   /*  24     */
    {  24,   4,   5,  0x19 },   /*  25     */
    {  24,   4,   6,  0x1a },   /*  26     */
    {  24,   4,   7,  0x1b },   /*  27     */
    {  25,   3,   0,  0x1c },   /*  28     */
    {  25,   3,   1,  0x1d },   /*  29     */
    {  25,   3,   2,  0x1e },   /*  30     */
    {  25,   3,   3,  0x1f },   /*  31     */
    {   0,   6,  20,  0x20 },   /*  32 ' ' */
    {   7,   3,   0,  0x21 },   /*  33 '!' */
    {   7,   3,   1,  0x22 },   /*  34 '"' */
    {   9,   3,   2,  0x23 },   /*  35 '#' */
    {  10,   3,   1,  0x24 },   /*  36 '$' */
    {   0,   6,  21,  0x25 },   /*  37 '%' */
    {   5,   3,   0,  0x26 },   /*  38 '&' */
    {   8,   3,   2,  0x27 },   /*  39 ''' */
    {   7,   3,   2,  0x28 },   /*  40 '(' */
    {   7,   3,   3,  0x29 },   /*  41 ')' */
    {   5,   3,   1,  0x2a },   /*  42 '*' */
    {   8,   3,   3,  0x2b },   /*  43 '+' */
    {   5,   3,   2,  0x2c },   /*  44 ',' */
    {   0,   6,  22,  0x2d },   /*  45 '-' */
    {   0,   6,  23,  0x2e },   /*  46 '.' */
    {   0,   6,  24,  0x2f },   /*  47 '/' */
    {   0,   5,   0,  0x30 },   /*  48 '0' */
    {   0,   5,   1,  0x31 },   /*  49 '1' */
    {   0,   5,   2,  0x32 },   /*  50 '2' */
    {   0,   6,  25,  0x33 },   /*  51 '3' */
    {   0,   6,  26,  0x34 },   /*  52 '4' */
    {   0,   6,  27,  0x35 },   /*  53 '5' */
    {   0,   6,  28,  0x36 },   /*  54 '6' */
    {   0,   6,  29,  0x37 },   /*  55 '7' */
    {   0,   6,  30,  0x38 },   /*  56 '8' */
    {   0,   6,  31,  0x39 },   /*  57 '9' */
    {   1,   6,  28,  0x3a },   /*  58 ':' */
    {   5,   3,   3,  0x3b },   /*  59 ';' */
    {  13,   2,   0,  0x3c },   /*  60 '<' */
    {   1,   5,   0,  0x3d },   /*  61 '=' */
    {   9,   3,   3,  0x3e },   /*  62 '>' */
    {   8,   2,   0,  0x3f },   /*  63 '?' */
    {  10,   3,   2,  0x40 },   /*  64 '@' */
    {   1,   5,   1,  0x41 },   /*  65 'A' */
    {   1,   6,  29,  0x42 },   /*  66 'B' */
    {   1,   6,  30,  0x43 },   /*  67 'C' */
    {   1,   6,  31,  0x44 },   /*  68 'D' */
    {   2,   5,   0,  0x45 },   /*  69 'E' */
    {   2,   5,   1,  0x46 },   /*  70 'F' */
    {   2,   5,   2,  0x47 },   /*  71 'G' */
    {   2,   5,   3,  0x48 },   /*  72 'H' */
    {   2,   5,   4,  0x49 },   /*  73 'I' */
    {   2,   5,   5,  0x4a },   /*  74 'J' */
    {   2,   5,   6,  0x4b },   /*  75 'K' */
    {   2,   5,   7,  0x4c },   /*  76 'L' */
    {   2,   5,   8,  0x4d },   /*  77 'M' */
    {   2,   5,   9,  0x4e },   /*  78 'N' */
    {   2,   5,  10,  0x4f },   /*  79 'O' */
    {   2,   5,  11,  0x50 },   /*  80 'P' */
    {   2,   5,  12,  0x51 },   /*  81 'Q' */
    {   2,   5,  13,  0x52 },   /*  82 'R' */
    {   2,   5,  14,  0x53 },   /*  83 'S' */
    {   2,   5,  15,  0x54 },   /*  84 'T' */
    {   3,   4,   0,  0x55 },   /*  85 'U' */
    {   3,   4,   1,  0x56 },   /*  86 'V' */
    {   3,   4,   2,  0x57 },   /*  87 'W' */
    {   6,   2,   0,  0x58 },   /*  88 'X' */
    {   3,   4,   3,  0x59 },   /*  89 'Y' */
    {   6,   2,   1,  0x5a },   /*  90 'Z' */
    {  10,   3,   3,  0x5b },   /*  91 '[' */
    {  15,   4,   0,  0x5c },   /*  92 '\' */
    {  11,   2,   0,  0x5d },   /*  93 ']' */
    {  12,   2,   0,  0x5e },   /*  94 '^' */
    {   1,   5,   2,  0x5f },   /*  95 '_' */
    {  13,   2,   1,  0x60 },   /*  96 '`' */
    {   0,   5,   3,  0x61 },   /*  97 'a' */
    {   1,   5,   3,  0x62 },   /*  98 'b' */
    {   0,   5,   4,  0x63 },   /*  99 'c' */
    {   1,   5,   4,  0x64 },   /* 100 'd' */
    {   0,   5,   5,  0x65 },   /* 101 'e' */
    {   1,   5,   5,  0x66 },   /* 102 'f' */
    {   1,   5,   6,  0x67 },   /* 103 'g' */
    {   1,   5,   7,  0x68 },   /* 104 'h' */
    {   0,   5,   6,  0x69 },   /* 105 'i' */
    {   3,   4,   4,  0x6a },   /* 106 'j' */
    {   3,   4,   5,  0x6b },   /* 107 'k' */
    {   1,   5,   8,  0x6c },   /* 108 'l' */
    {   1,   5,   9,  0x6d },   /* 109 'm' */
    {   1,   5,  10,  0x6e },   /* 110 'n' */
    {   0,   5,   7,  0x6f },   /* 111 'o' */
    {   1,   5,  11,  0x70 },   /* 112 'p' */
    {   3,   4,   6,  0x71 },   /* 113 'q' */
    {   1,   5,  12,  0x72 },   /* 114 'r' */
    {   0,   5,   8,  0x73 },   /* 115 's' */
    {   0,   5,   9,  0x74 },   /* 116 't' */
    {   1,   5,  13,  0x75 },   /* 117 'u' */
    {   3,   4,   7,  0x76 },   /* 118 'v' */
    {   4,   3,   0,  0x77 },   /* 119 'w' */
    {   4,   3,   1,  0x78 },   /* 120 'x' */
    {   4,   3,   2,  0x79 },   /* 121 'y' */
    {   4,   3,   3,  0x7a },   /* 122 'z' */
    {  14,   1,   0,  0x7b },   /* 123 '{' */
    {   9,   2,   0,  0x7c },   /* 124 '|' */
    {  12,   2,   1,  0x7d },   /* 125 '}' */
    {  11,   2,   1,  0x7e },   /* 126 '~' */
    {  26,   2,   0,  0x7f },   /* 127     */
    {  15,   5,   6,  0x80 },   /* 128     */
    {  16,   6,  18,  0x81 },   /* 129     */
    {  15,   5,   7,  0x82 },   /* 130     */
    {  15,   5,   8,  0x83 },   /* 131     */
    {  16,   6,  19,  0x84 },   /* 132     */
    {  16,   6,  20,  0x85 },   /* 133     */
    {  16,   6,  21,  0x86 },   /* 134     */
    {  17,   6,  25,  0x87 },   /* 135     */
    {  16,   6,  22,  0x88 },   /* 136     */
    {  17,   6,  26,  0x89 },   /* 137     */
    {  17,   6,  27,  0x8a },   /* 138     */
    {  17,   6,  28,  0x8b },   /* 139     */
    {  17,   6,  29,  0x8c },   /* 140     */
    {  17,   6,  30,  0x8d },   /* 141     */
    {  19,   5,  11,  0x8e },   /* 142     */
    {  17,   6,  31,  0x8f },   /* 143     */
    {  19,   5,  12,  0x90 },   /* 144     */
    {  19,   5,  13,  0x91 },   /* 145     */
    {  16,   6,  23,  0x92 },   /* 146     */
    {  18,   5,   0,  0x93 },   /* 147     */
    {  19,   5,  14,  0x94 },   /* 148     */
    {  18,   5,   1,  0x95 },   /* 149     */
    {  18,   5,   2,  0x96 },   /* 150     */
    {  18,   5,   3,  0x97 },   /* 151     */
    {  18,   5,   4,  0x98 },   /* 152     */
    {  15,   6,  28,  0x99 },   /* 153     */
    {  16,   6,  24,  0x9a },   /* 154     */
    {  18,   5,   5,  0x9b },   /* 155     */
    {  16,   6,  25,  0x9c },   /* 156     */
    {  18,   5,   6,  0x9d },   /* 157     */
    {  18,   5,   7,  0x9e },   /* 158     */
    {  19,   5,  15,  0x9f },   /* 159     */
    {  16,   6,  26,  0xa0 },   /* 160     */
    {  15,   6,  29,  0xa1 },   /* 161     */
    {  15,   5,   9,  0xa2 },   /* 162     */
    {  16,   6,  27,  0xa3 },   /* 163     */
    {  16,   6,  28,  0xa4 },   /* 164     */
    {  18,   5,   8,  0xa5 },   /* 165     */
    {  18,   5,   9,  0xa6 },   /* 166     */
    {  15,   6,  30,  0xa7 },   /* 167     */
    {  18,   5,  10,  0xa8 },   /* 168     */
    {  16,   6,  29,  0xa9 },   /* 169     */
    {  16,   6,  30,  0xaa },   /* 170     */
    {  20,   4,   0,  0xab },   /* 171     */
    {  15,   6,  31,  0xac },   /* 172     */
    {  16,   6,  31,  0xad },   /* 173     */
    {  18,   5,  11,  0xae },   /* 174     */
    {  18,   5,  12,  0xaf },   /* 175     */
    {  16,   5,   0,  0xb0 },   /* 176     */
    {  16,   5,   1,  0xb1 },   /* 177     */
    {  17,   5,   0,  0xb2 },   /* 178     */
    {  16,   5,   2,  0xb3 },   /* 179     */
    {  18,   5,  13,  0xb4 },   /* 180     */
    {  17,   5,   1,  0xb5 },   /* 181     */
    {  18,   5,  14,  0xb6 },   /* 182     */
    {  18,   5,  15,  0xb7 },   /* 183     */
    {  15,   5,  10,  0xb8 },   /* 184     */
    {  17,   5,   2,  0xb9 },   /* 185     */
    {  17,   5,   3,  0xba },   /* 186     */
    {  17,   5,   4,  0xbb },   /* 187     */
    {  19,   4,   0,  0xbc },   /* 188     */
    {  17,   5,   5,  0xbd },   /* 189     */
    {  17,   5,   6,  0xbe },   /* 190     */
    {  19,   4,   1,  0xbf },   /* 191     */
    {  21,   5,   0,  0xc0 },   /* 192     */
    {  21,   5,   1,  0xc1 },   /* 193     */
    {  15,   5,  11,  0xc2 },   /* 194     */
    {  15,   4,   1,  0xc3 },   /* 195     */
    {  17,   5,   7,  0xc4 },   /* 196     */
    {  19,   4,   2,  0xc5 },   /* 197     */
    {  17,   5,   8,  0xc6 },   /* 198     */
    {  20,   5,  12,  0xc7 },   /* 199     */
    {  21,   5,   2,  0xc8 },   /* 200     */
    {  21,   5,   3,  0xc9 },   /* 201     */
    {  21,   5,   4,  0xca },   /* 202     */
    {  21,   6,  30,  0xcb },   /* 203     */
    {  21,   6,  31,  0xcc },   /* 204     */
    {  21,   5,   5,  0xcd },   /* 205     */
    {  20,   4,   1,  0xce },   /* 206     */
    {  20,   5,  13,  0xcf },   /* 207     */
    {  15,   4,   2,  0xd0 },   /* 208     */
    {  16,   5,   3,  0xd1 },   /* 209     */
    {  21,   5,   6,  0xd2 },   /* 210     */
    {  22,   5,   0,  0xd3 },   /* 211     */
    {  22,   5,   1,  0xd4 },   /* 212     */
    {  21,   5,   7,  0xd5 },   /* 213     */
    {  22,   5,   2,  0xd6 },   /* 214     */
    {  20,   4,   2,  0xd7 },   /* 215     */
    {  16,   5,   4,  0xd8 },   /* 216     */
    {  16,   5,   5,  0xd9 },   /* 217     */
    {  21,   5,   8,  0xda },   /* 218     */
    {  21,   5,   9,  0xdb },   /* 219     */
    {  26,   2,   1,  0xdc },   /* 220     */
    {  22,   5,   3,  0xdd },   /* 221     */
    {  22,   5,   4,  0xde },   /* 222     */
    {  22,   5,   5,  0xdf },   /* 223     */
    {  15,   5,  12,  0xe0 },   /* 224     */
    {  20,   4,   3,  0xe1 },   /* 225     */
    {  15,   5,  13,  0xe2 },   /* 226     */
    {  16,   5,   6,  0xe3 },   /* 227     */
    {  17,   5,   9,  0xe4 },   /* 228     */
    {  16,   5,   7,  0xe5 },   /* 229     */
    {  16,   5,   8,  0xe6 },   /* 230     */
    {  19,   4,   3,  0xe7 },   /* 231     */
    {  17,   5,  10,  0xe8 },   /* 232     */
    {  17,   5,  11,  0xe9 },   /* 233     */
    {  20,   5,  14,  0xea },   /* 234     */
    {  20,   5,  15,  0xeb },   /* 235     */
    {  20,   4,   4,  0xec },   /* 236     */
    {  20,   4,   5,  0xed },   /* 237     */
    {  21,   5,  10,  0xee },   /* 238     */
    {  19,   4,   4,  0xef },   /* 239     */
    {  21,   5,  11,  0xf0 },   /* 240     */
    {  22,   5,   6,  0xf1 },   /* 241     */
    {  21,   5,  12,  0xf2 },   /* 242     */
    {  21,   5,  13,  0xf3 },   /* 243     */
    {  22,   5,   7,  0xf4 },   /* 244     */
    {  22,   5,   8,  0xf5 },   /* 245     */
    {  22,   5,   9,  0xf6 },   /* 246     */
    {  22,   5,  10,  0xf7 },   /* 247     */
    {  22,   5,  11,  0xf8 },   /* 248     */
    {  27,   1,   0,  0xf9 },   /* 249     */
    {  22,   5,  12,  0xfa },   /* 250     */
    {  22,   5,  13,  0xfb },   /* 251     */
    {  22,   5,  14,  0xfc },   /* 252     */
    {  22,   5,  15,  0xfd },   /* 253     */
    {  23,   4,   0,  0xfe },   /* 254     */
    {  21,   5,  14,  0xff },   /* 255     */
};

/*
 * Map used for string to h2huf
 */
onetab_t h2h_onetab[] = {
    {  5,   9,  0x0000 }, /* ones=0  count=32 */
    {  5,  13,  0x0020 }, /* ones=1  count=32 */
    {  5,  15,  0x0040 }, /* ones=2  count=16 */
    {  4,   7,  0x0050 }, /* ones=3  count=8  */
    {  3,   3,  0x0058 }, /* ones=4  count=4  */
    {  3,   3,  0x005c }, /* ones=5  count=4  */
    {  2,   1,  0x0060 }, /* ones=6  count=2  */
    {  3,   3,  0x0062 }, /* ones=7  count=4  */
    {  2,   0,  0x0066 }, /* ones=8  count=4  */
    {  2,   0,  0x006a }, /* ones=9  count=4  */
    {  3,   3,  0x006e }, /* ones=10 count=4  */
    {  2,   1,  0x0072 }, /* ones=11 count=2  */
    {  2,   1,  0x0074 }, /* ones=12 count=2  */
    {  2,   1,  0x0076 }, /* ones=13 count=2  */
    {  1,   0,  0x0078 }, /* ones=14 count=1  */
    {  4,   2,  0x0079 }, /* ones=15 count=32 special third range at 13 */
    {  5,   8,  0x0099 }, /* ones=16 count=32 */
    {  5,  11,  0x00b9 }, /* ones=17 count=32 */
    {  5,  15,  0x00d9 }, /* ones=18 count=16 */
    {  4,   4,  0x00e9 }, /* ones=19 count=16 */
    {  4,   5,  0x00f9 }, /* ones=20 count=16 */
    {  5,  14,  0x0109 }, /* ones=21 count=32 */
    {  5,  15,  0x0129 }, /* ones=22 count=16 */
    {  4,   0,  0x0139 }, /* ones=23 count=16 */
    {  4,   7,  0x0149 }, /* ones=24 count=8  */
    {  3,   3,  0x0151 }, /* ones=25 count=4  */
    {  2,   1,  0x0155 }, /* ones=26 count=2  */
    {  1,   0,  0x0157 }, /* ones=27 count=1  */
    {  2,   1,  0x0158 }, /* ones=28 count=2  */
    {  1,   0,  0x015a }, /* ones=29 count=1  */
};

/*
 * Value table for h2huf to string mapping
 */
uint8_t h2h_vals [347] = {
    /* 000 */  0x30, 0x31, 0x32, 0x61, 0x63, 0x65, 0x69, 0x6f, 0x73, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 010 */  0x00, 0x00, 0x00, 0x00, 0x20, 0x25, 0x2d, 0x2e, 0x2f, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    /* 020 */  0x3d, 0x41, 0x5f, 0x62, 0x64, 0x66, 0x67, 0x68, 0x6c, 0x6d, 0x6e, 0x70, 0x72, 0x75, 0x00, 0x00,
    /* 030 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x42, 0x43, 0x44,
    /* 040 */  0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54,
    /* 050 */  0x55, 0x56, 0x57, 0x59, 0x6a, 0x6b, 0x71, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x26, 0x2a, 0x2c, 0x3b,
    /* 060 */  0x58, 0x5a, 0x21, 0x22, 0x28, 0x29, 0x3f, 0x00, 0x27, 0x2b, 0x7c, 0x00, 0x23, 0x3e, 0x00, 0x24,
    /* 070 */  0x40, 0x5b, 0x5d, 0x7e, 0x5e, 0x7d, 0x3c, 0x60, 0x7b, 0x5c, 0xc3, 0xd0, 0x00, 0x00, 0x00, 0x80,
    /* 080 */  0x82, 0x83, 0xa2, 0xb8, 0xc2, 0xe0, 0xe2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 090 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0xa1, 0xa7, 0xac, 0xb0, 0xb1, 0xb3, 0xd1, 0xd8, 0xd9, 0xe3,
    /* 0a0 */  0xe5, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x84, 0x85, 0x86, 0x88,
    /* 0b0 */  0x92, 0x9a, 0x9c, 0xa0, 0xa3, 0xa4, 0xa9, 0xaa, 0xad, 0xb2, 0xb5, 0xb9, 0xba, 0xbb, 0xbd, 0xbe,
    /* 0c0 */  0xc4, 0xc6, 0xe4, 0xe8, 0xe9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 0d0 */  0x00, 0x01, 0x87, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8f, 0x93, 0x95, 0x96, 0x97, 0x98, 0x9b, 0x9d,
    /* 0e0 */  0x9e, 0xa5, 0xa6, 0xa8, 0xae, 0xaf, 0xb4, 0xb6, 0xb7, 0xbc, 0xbf, 0xc5, 0xe7, 0xef, 0x00, 0x00,
    /* 0f0 */  0x00, 0x00, 0x00, 0x09, 0x8e, 0x90, 0x91, 0x94, 0x9f, 0xab, 0xce, 0xd7, 0xe1, 0xec, 0xed, 0x00,
    /* 100 */  0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xcf, 0xea, 0xeb, 0xc0, 0xc1, 0xc8, 0xc9, 0xca, 0xcd, 0xd2,
    /* 110 */  0xd5, 0xda, 0xdb, 0xee, 0xf0, 0xf2, 0xf3, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 120 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcb, 0xcc, 0xd3, 0xd4, 0xd6, 0xdd, 0xde, 0xdf, 0xf1,
    /* 130 */  0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0x00, 0x02, 0x03, 0x04, 0x05, 0x06,
    /* 140 */  0x07, 0x08, 0x0b, 0x0c, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x17, 0x18, 0x19, 0x1a,
    /* 150 */  0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x7f, 0xdc, 0xf9, 0x0a, 0x0d, 0x16,
};
