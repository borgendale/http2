
#include <h2utils.h>
#include <hpack.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void testCanonical(void);
void testHuf(void);
void testStaticLookup(void);
void testDynamicLookup(void);
void testHPACK(void);

CU_TestInfo hpack_tests[] = {
    {"canonicalHdr   ..",     testCanonical },
    {"huffmanEncode  ..",      testHuf },
    {"staticLookup   ..",      testStaticLookup },
    {"dynamicLookup  ..",      testDynamicLookup },
    {"HPACK          ..",      testHPACK },
    NULL,
};

/*
 * Array that carries the test suite and other functions to the CUnit framework
 */
CU_SuiteInfo http2_CUnit_suites[] = {
    {"hpack",  NULL,   NULL,  NULL, NULL, hpack_tests},
    NULL,
};

static int verbose = 0;
static int RC = 0;


const char * h2_static_val[17];
const char * h2_static[63];

/*
 * This is the main CUnit routine that starts the CUnit framework.
 * CU_basic_run_tests() Actually runs all the test routines.
 */
static void startup(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    CU_SuiteInfo * runsuite;
    CU_pTestRegistry testregistry;
    CU_pSuite testsuite;
    CU_pTest testcase;
    int testsrun = 0;

    runsuite = http2_CUnit_suites;

    setvbuf(stdout, NULL, _IONBF, 0);
    if (CU_initialize_registry() == CUE_SUCCESS) {
        if (CU_register_suites(runsuite) == CUE_SUCCESS) {
            CU_basic_set_mode(CU_BRM_VERBOSE);
            CU_basic_run_tests();
        }
    }
}


/*
 * This routine closes CUnit environment, and needs to be called only before the exiting of the program.
 */
static void term(void) {
    CU_cleanup_registry();
}

/*
 * This routine displays the statistics for the test run in silent mode.
 * CUnit does not display any data for the test result logged as "success".
 */
static void summary(char * headline) {
    CU_RunSummary *pCU_pRunSummary;

#if 0
    pCU_pRunSummary = CU_get_run_summary();
    if (!verbose) {
        printf("%s", headline);
        printf("\n--Run Summary: Type       Total     Ran  Passed  Failed\n");
        printf("               tests     %5d   %5d   %5d   %5d\n",
                pCU_pRunSummary->nTestsRun + 1, pCU_pRunSummary->nTestsRun + 1,
                pCU_pRunSummary->nTestsRun + 1 - pCU_pRunSummary->nTestsFailed,
                pCU_pRunSummary->nTestsFailed);
        printf("               asserts   %5d   %5d   %5d   %5d\n",
                pCU_pRunSummary->nAsserts, pCU_pRunSummary->nAsserts,
                pCU_pRunSummary->nAsserts - pCU_pRunSummary->nAssertsFailed,
                pCU_pRunSummary->nAssertsFailed);
    }
#endif
}


/*
 * This routine prints out the final test sun status. The final test run status will be scanned by the
 * build process to determine if the build is successful.
 * Please do not change the format of the output.
 */
void print_final_summary(void) {
    CU_RunSummary * CU_pRunSummary_Final;
    CU_pRunSummary_Final = CU_get_run_summary();
    printf("\n\n[cunit] Tests run: %d, Failures: %d, Errors: %d\n\n",
    CU_pRunSummary_Final->nTestsRun,
    CU_pRunSummary_Final->nTestsFailed,
    CU_pRunSummary_Final->nAssertsFailed);
    RC = CU_pRunSummary_Final->nTestsFailed + CU_pRunSummary_Final->nAssertsFailed;
}


/*
 * Main entry point
 */
int main(int argc, char * * argv) {
#ifdef _WIN32
    if (argc>2) {
        __debugbreak();
    }
#endif
    if (argc > 1 && *argv[1]=='v') {
        verbose = 1;
    }
    if (argc > 1 && *argv[1]=='q') {
        verbose = 0;
    }

    /* Run tests */
    startup();

    /* Print results */
    summary("\n\n Test: --- Testing HTTP/2 --- ...\n");
    print_final_summary();

    term();
    return RC;
}



void testCanonical(void) {
    int rc;
    int len;
    int i;
    char * test;
    char xbuf [4096];
    char sbuf [4096];
    char all [256];

    test = ":Status:400\naBc:   def\r\nghi:\n kl    mn\n";
    strcpy(xbuf, test);
    rc = hpack_canonicalize(xbuf, strlen(xbuf));
    if (rc < 0)
        xbuf[0] = 0;
    CU_ASSERT(rc == 30);
    if (verbose || rc != 30)
        printf("rc=%d %s\n", rc, xbuf);
}

void testHuf(void) {
    int rc;
    int len;
    int i;
    char * test;
    char xbuf [4096];
    char sbuf [4096];
    char all [256];

    //n = h2_str2huf("abcXYZ                /* %#", 10, xbuf, sizeof xbuf);*/
    len = h2_str2huf("now is the time for all good men to come to the aid of their parties. #(*)", 74, xbuf, sizeof xbuf);
    CU_ASSERT(len == 54);
    if (verbose || len != 54) {
        printf("str2huf len=%d  ", len);
        for (i=0; i<len; i++) {
            printf("%02x ", (uint8_t)xbuf[i]);
        }
        printf("\n");
    }

    sbuf[0] = 0;
    len = h2_huf2str(xbuf, len, sbuf, sizeof sbuf);
    CU_ASSERT(len == 74);
    if (verbose || len != 74)
        printf("huf2str len=%d, val='%s'\n", len, sbuf);

    for (i=0; i<256; i++) {
        all[i] = (char)i;
    }
    len = h2_str2huf(all, 256, xbuf, sizeof xbuf);
    CU_ASSERT(len ==583);
    if (verbose || len != 583)
        printf("str2huf len=%d\n", len);

    len = h2_huf2str(xbuf, len, sbuf, sizeof sbuf);
    CU_ASSERT(len == 256);
    if (verbose || len != 256)
        printf("huf2str len=%d\n", len);
    CU_ASSERT(!memcmp(all, sbuf, 256));
    if (verbose || memcmp(all, sbuf, 256)) {
        for (i=0; i<len; i++) {
            printf("%02x ", (uint8_t)sbuf[i]);
            if (i%32 == 31)
                printf("\n");
        }
        printf("\n");
    }
}

/*
 * Test static lookup
 */
void testStaticLookup(void) {
    int rc;
    int i;

    rc = hpack_lookupStatic(":authority", "x");
    CU_ASSERT(rc == 1);
    if (rc != 1)
        printf("FAILED static lookup: :authority x rc=%d\n", rc);

    rc = hpack_lookupStatic(":method", "x");
    CU_ASSERT(rc == 2);
    if (rc != 2)
        printf("FAILED static lookup: :method x rc=%d\n", rc);

    rc = hpack_lookupStatic(":method", "GET");
    CU_ASSERT(rc == (2|NOLITERAL));
    if (rc != (2|NOLITERAL))
        printf("FAILED static lookup: :method GET rc=%x\n", rc);

    rc = hpack_lookupStatic(":method", "POST");
    CU_ASSERT(rc == (3|NOLITERAL));
    if (rc != (3|NOLITERAL))
        printf("FAILED static lookup: :method POST rc=%x\n", rc);

    rc = hpack_lookupStatic(":path", "x");
    CU_ASSERT(rc == 4);
    if (rc != 4)
        printf("FAILED static lookup: :path x rc=%d\n", rc);

    rc = hpack_lookupStatic(":path", "/");
    CU_ASSERT(rc == (4|NOLITERAL));
    if (rc != (4|NOLITERAL))
        printf("FAILED static lookup: :path / rc=%x\n", rc);

    rc = hpack_lookupStatic(":path", "/index.html");
    CU_ASSERT(rc == (5|NOLITERAL));
    if (rc != (5|NOLITERAL))
        printf("FAILED static lookup: :path /index.html rc=%x\n", rc);

    rc = hpack_lookupStatic(":scheme", "x");
    CU_ASSERT(rc == 6);
    if (rc != 6)
        printf("FAILED static lookup: :path /index.html rc=%x\n", rc);

    rc = hpack_lookupStatic(":scheme", "http");
    CU_ASSERT(rc == (6|NOLITERAL));
    if (rc != (6|NOLITERAL))
        printf("FAILED static lookup: :path /index.html rc=%x\n", rc);

    rc = hpack_lookupStatic(":scheme", "https");
    CU_ASSERT(rc == (7|NOLITERAL));
    if (rc != (7|NOLITERAL))
        printf("FAILED static lookup: :path /index.html rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "x");
    CU_ASSERT(rc == 8);
    if (rc != 8)
        printf("FAILED static lookup: :status x rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "200");
    CU_ASSERT(rc == (8|NOLITERAL));
    if (rc != (8|NOLITERAL))
        printf("FAILED static lookup: :status 200 rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "204");
    CU_ASSERT(rc == (9|NOLITERAL));
    if (rc != (9|NOLITERAL))
        printf("FAILED static lookup: :status 204 rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "206");
    CU_ASSERT(rc == (10|NOLITERAL));
    if (rc != (10|NOLITERAL))
        printf("FAILED static lookup: :status 206 rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "304");
    CU_ASSERT(rc == (11|NOLITERAL));
    if (rc != (11|NOLITERAL))
        printf("FAILED static lookup: :status 304 rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "400");
    CU_ASSERT(rc == (12|NOLITERAL));
    if (rc != (12|NOLITERAL))
        printf("FAILED static lookup: :status 400 rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "404");
    CU_ASSERT(rc == (13|NOLITERAL));
    if (rc != (13|NOLITERAL))
        printf("FAILED static lookup: :status 404 rc=%x\n", rc);

    rc = hpack_lookupStatic(":status", "500");
    CU_ASSERT(rc == (14|NOLITERAL));
    if (rc != (14|NOLITERAL))
        printf("FAILED static lookup: :status 500 rc=%x\n", rc);

    for (i=15; i<62; i++) {
        rc = hpack_lookupStatic(h2_static[i], "x");
        CU_ASSERT(rc == i);
        if (rc != i)
            printf("FAILED static lookup hdr=%s  %d found=%d\n", h2_static[i], i, rc);
    }
}


void testDynamicLookup(void) {
    int  ctxsize;
    int  cursize;
    int  usedsize;
    int  entries;
    int  rc;

    h2_context_t * h2ctx = hpack_newContext(4096, 1, 1024, H2ENCODE_MIN, 1);

    entries = hpack_getContextStats(h2ctx, &ctxsize, &cursize, &usedsize);
    CU_ASSERT(entries == 0);
    CU_ASSERT(usedsize == 0);
    CU_ASSERT(ctxsize == 4096);
    CU_ASSERT(cursize == 4096);
    if (verbose)
        printf("entries=%d ctxsize=%d currsize=%d used=%d\n", entries, ctxsize, cursize, usedsize);

    rc = hpack_pushDynamic(h2ctx, "testhdr", "val1");
    CU_ASSERT(rc == 62);
    rc = hpack_pushDynamic(h2ctx, "testhdr", "val2");
    CU_ASSERT(rc == 62);
    rc = hpack_pushDynamic(h2ctx, "another", "another value");
    CU_ASSERT(rc == 62);

    entries = hpack_getContextStats(h2ctx, NULL, NULL, &usedsize);
    CU_ASSERT(entries == 3);
    CU_ASSERT(usedsize == 138);
    if (verbose || entries != 3 || usedsize != 138)
        printf("entries=%d used=%d\n", entries, usedsize);

    rc = hpack_lookupDynamic(h2ctx, "another", "fred");
    CU_ASSERT(rc == 62);
    if (rc != 62)
        printf("FAILED dynamic lookup: another fred rc=%x\n", rc);

    rc = hpack_lookupDynamic(h2ctx, "another", "another value");
    CU_ASSERT(rc == (62|NOLITERAL));
    if (rc != (62|NOLITERAL))
        printf("FAILED dynamic lookup: another anothher value rc=%x\n", rc);

    rc = hpack_lookupDynamic(h2ctx, "testhdr", "val1");
    CU_ASSERT(rc == (64|NOLITERAL));
    if (rc != (64|NOLITERAL))
        printf("FAILED dynamic lookup: testhdr val1 rc=%x\n", rc);

    rc = hpack_lookupDynamic(h2ctx, "testhdr", "val2");
    CU_ASSERT(rc == (63|NOLITERAL));
    if (rc != (63|NOLITERAL))
        printf("FAILED dynamic lookup: testhdr val2 rc=%x\n", rc);

    rc = hpack_lookupDynamic(h2ctx, "testhdr", "val3");
    CU_ASSERT(rc == 63);
    if (rc != 63)
        printf("FAILED dynamic lookup: testhdr val3 rc=%x\n", rc);

    rc = hpack_changeDynamic(h2ctx, 64);
    entries = hpack_getContextStats(h2ctx, &ctxsize, &cursize, &usedsize);
    CU_ASSERT(entries == 1);
    CU_ASSERT(cursize == 64);
    if (verbose || entries != 1 || cursize != 64)
        printf("xntries=%d ctxsize=%d currsize=%d used=%d\n", entries, ctxsize, cursize, usedsize);


    rc = hpack_lookupDynamic(h2ctx, "another", "another value");
    CU_ASSERT(rc == (62|NOLITERAL));
    if (rc != (62|NOLITERAL))
        printf("FAILED dynamic lookup: another rc=%x\n", rc);

    rc = hpack_changeDynamic(h2ctx,0);
    entries = hpack_getContextStats(h2ctx, &ctxsize, &cursize, &usedsize);
    CU_ASSERT(rc == 0);
    CU_ASSERT(entries == 0);
    CU_ASSERT(usedsize == 0);
    if (verbose || entries != 0 || usedsize != 0)
        printf("entries=%d ctxsize=%d currsize=%d used=%d\n", entries, ctxsize, cursize, usedsize);

    hpack_freeContext(h2ctx);
}


void testHeader(int tblsize, int encopt, int decopt, int huff) {
    h2_context_t * client_enc;
    h2_context_t * client_dec;
    h2_context_t * server_enc;
    h2_context_t * server_dec;
    char srcbuf [2048];
    char ebufbuf[4096];
    char dbufbuf[4096];
    h2_buffer_t ebuf = {ebufbuf, sizeof ebufbuf};
    h2_buffer_t dbuf = {dbufbuf, sizeof dbufbuf};
    int   rc;
    int   entc;
    int   usedc;
    int   ents;
    int   useds;
    int   srclen;

    char * hdr1 =
    ":method: GET\n"
    ":scheme: https\n"
    ":authority: kwb.borgendale.com\n"
    ":path: /\n"
    "Accept-Encoding: gzip, deflate\n"
    "Accept-Language: en\n"
    "Content-Type: text/plain; charset=utf-8\n"
    "MyHeader: myvalue\n";

    char * hdr2 =
    ":status: 400\n"
    "Date: 2017-07-11T01:23\n"
    "Server: This is who I am\n";

    client_enc = hpack_newContext(tblsize, 1, 256, encopt, huff);
    CU_ASSERT(client_enc != NULL);
    client_dec = hpack_newContext(tblsize, 0, 256, decopt, huff);
    CU_ASSERT(client_dec != NULL);
    server_enc = hpack_newContext(tblsize, 1, 256, encopt, huff);
    CU_ASSERT(server_enc != NULL);
    server_dec = hpack_newContext(tblsize, 0, 256, decopt, huff);
    CU_ASSERT(server_dec != NULL);

    strcpy(srcbuf, hdr1);  /* The source is modified by encode */
    ebuf.used = 0;
    ebuf.pos  = 0;
    dbuf.used = 0;
    dbuf.pos  = 0;
    srclen = (int)strlen(srcbuf);
    rc = hpack_encode(client_enc, srcbuf, srclen, &ebuf);
    CU_ASSERT(rc == 0);
    if (verbose || rc != 0)
        printf("encode rc=%d outlen=%d\n", rc, ebuf.used);

    rc = hpack_decode(server_dec, ebuf.buf, ebuf.used, &dbuf);
    CU_ASSERT(rc == 0);
    if (verbose || rc != 0)
        printf("decode rc=%d\n", rc);

    entc = hpack_getContextStats(client_enc, NULL, NULL, &usedc);
    ents = hpack_getContextStats(server_dec, NULL, NULL, &useds);

    CU_ASSERT(entc == ents);
    CU_ASSERT(usedc == useds);
    if (verbose || entc != ents || usedc != useds)
        printf("context size: client=%d,%d  server=%d,%d\n", entc,usedc, ents,useds);

    if (decopt == H2DECODE_SPACE) {
        CU_ASSERT(srclen == dbuf.used);
        if (verbose || srclen != dbuf.used) {
            printf("inlen=%d outlen=%d\n", srclen, dbuf.used);
            if (srclen != dbuf.used) {
                printf("hdr=%s", dbuf.buf);
            }
        }
    }
}


void testHPACK(void) {
    if (verbose)
        printf("\nsize=4096 MAX\n");
    testHeader(4096, H2ENCODE_MAX, H2DECODE_SPACE, 0);

    if (verbose)
        printf("\nsize=4096 MAX HUFF\n");
    testHeader(4096, H2ENCODE_MAX, H2DECODE_SPACE, 1);

    if (verbose)
        printf("\nsize=64 MAX\n");
    testHeader(64, H2ENCODE_MAX, H2DECODE_SPACE, 0);

    if (verbose)
        printf("\nsize=64 MAX HUFF\n");
    testHeader(64, H2ENCODE_MAX, H2DECODE_SPACE, 1);

    if (verbose)
        printf("\nsize=0 STATIC\n");
    testHeader(0, H2ENCODE_STATIC, H2DECODE_SPACE, 0);

    if (verbose)
        printf("\nsize=0 STATIC HUFF\n");
    testHeader(0, H2ENCODE_STATIC, H2DECODE_SPACE, 1);

    if (verbose)
        printf("\nsize=0 NONE HUFF\n");
    testHeader(0, H2ENCODE_NONE, H2DECODE_SPACE, 1);

    if (verbose)
        printf("\nsize=0 NONE\n");
    testHeader(0, H2ENCODE_NONE, H2DECODE_SPACE, 0);
}


