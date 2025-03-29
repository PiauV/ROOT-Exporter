#ifndef MACRO_TESTS
#define MACRO_TESTS

#include "TSystem.h"

extern int nfailed;
extern int npassed;
extern int ntest;
extern bool pass_all_tests;

#define __FILENAME__ gSystem->BaseName(__FILE__)

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

#define SIMPLE_TEST(x)                                                           \
    if (!(x)) {                                                                  \
        std::cout << "Error: test " << __FUNCTION__ << " failed" << std::endl;   \
        std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]" << std::endl; \
        nfailed++;                                                               \
    }                                                                            \
    else                                                                         \
        npassed++;

#define COMPARE_TSTRING(str, ref)                                                \
    if (str.CompareTo(ref) != 0) {                                               \
        std::cout << "Error: test " << __FUNCTION__ << " failed"                 \
                  << " - expected \'" << ref << "\' but got \'" << str << "\'"   \
                  << std::endl;                                                  \
        std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]" << std::endl; \
        nfailed++;                                                               \
    }                                                                            \
    else                                                                         \
        npassed++;

#define EXPECTED_EXCEPTION(expr, ExceptionType)                                                          \
    try {                                                                                                \
        expr;                                                                                            \
        std::cout << "Error: test " << __FUNCTION__ << " failed (no exception was thrown)" << std::endl; \
        std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]" << std::endl;                         \
        nfailed++;                                                                                       \
    }                                                                                                    \
    catch (const ExceptionType&) {                                                                       \
        npassed++;                                                                                       \
    }                                                                                                    \
    catch (...) {                                                                                        \
        std::cout << "Error: test " << __FUNCTION__ << " failed (wrong exception)" << std::endl;         \
        std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]" << std::endl;                         \
        nfailed++;                                                                                       \
    }

#define EXCEPTION_CAUGHT(e)                                                  \
    std::cout << "Exception caught in " << __FUNCTION__ << std::endl;        \
    std::cout << e.what() << std::endl;                                      \
    std::cout << "[" << __FILENAME__ << ":" << __LINE__ << "]" << std::endl; \
    nfailed++;

#endif
