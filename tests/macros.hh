#ifndef MACRO_TESTS
#define MACRO_TESTS

#include "Log.hh"

extern int nfailed;
extern int npassed;
extern int ntest;
extern bool pass_all_tests;

// #include "TSystem.h"
// #define __FILENAME__ gSystem->BaseName(__FILE__)

#define PRINT_LINE() std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]" << std::endl

#define BEGIN_TEST()                                                                    \
    std::cout << "### TEST " << ++ntest << " .......... " << __FUNCTION__ << std::endl; \
    nfailed = npassed = 0;

#define END_TEST()                                                                 \
    if (nfailed)                                                                   \
        std::cout << "-->\033[1;31m FAILED TEST " << ntest << "\033[0m"            \
                  << " (" << nfailed << " failures / " << npassed + nfailed << ")" \
                  << std::endl;                                                    \
    else                                                                           \
        std::cout << "-->\033[1;32m PASSED TEST " << ntest << "\033[0m"            \
                  << " (" << npassed << " units)"                                  \
                  << std::endl;                                                    \
    pass_all_tests = pass_all_tests && (nfailed == 0);

#define SIMPLE_TEST(x)                                   \
    if (!(x)) {                                          \
        LOG_ERROR("Test " << __FUNCTION__ << " failed"); \
        PRINT_LINE();                                    \
        nfailed++;                                       \
    }                                                    \
    else                                                 \
        npassed++;

#define COMPARE_TSTRING(str, ref)                                                        \
    if (str.CompareTo(ref) != 0) {                                                       \
        LOG_ERROR("Test " << __FUNCTION__ << " failed"                                   \
                          << " - expected \'" << ref << "\' but got \'" << str << "\'"); \
        PRINT_LINE();                                                                    \
        nfailed++;                                                                       \
    }                                                                                    \
    else                                                                                 \
        npassed++;

#define EXPECTED_EXCEPTION(expr, ExceptionType)                                    \
    try {                                                                          \
        expr;                                                                      \
        LOG_ERROR("Test " << __FUNCTION__ << " failed (no exception was thrown)"); \
        PRINT_LINE();                                                              \
        nfailed++;                                                                 \
    }                                                                              \
    catch (const ExceptionType&) {                                                 \
        npassed++;                                                                 \
    }                                                                              \
    catch (...) {                                                                  \
        LOG_ERROR("Test " << __FUNCTION__ << " failed (wrong exception)");         \
        PRINT_LINE();                                                              \
        nfailed++;                                                                 \
    }

#define EXCEPTION_CAUGHT(e)                            \
    LOG_ERROR("Exception caught in " << __FUNCTION__); \
    std::cout << e.what() << std::endl;                \
    PRINT_LINE();                                      \
    nfailed++;

#endif
