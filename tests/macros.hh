#ifndef MACRO_TESTS
#define MACRO_TESTS

int nfailed = 0;
int npassed = 0;
int ntest = 0;

#define BEGIN_TEST() {                                                                  \
    std::cout << "### TEST " << ++ntest << " .......... " << __FUNCTION__ << std::endl; \
    nfailed = npassed = 0;                                                              \
}

#define END_TEST() {                                                                      \
    if (nfailed) std::cout << "-->\033[1;31m FAILED TEST "<< ntest << "\033[0m"           \
                           << " (" << nfailed << " failures / " << npassed+nfailed << ")" \
                           << std::endl;                                                  \
    else std::cout << "-->\033[1;32m PASSED TEST " << ntest << "\033[0m"                  \
                           << " (" << npassed << " units)"                                \
                           << std::endl;                                                  \
}

#define SIMPLE_TEST(x) {                                                                            \
    if (!(x)){                                                                                      \
        std::cout << "Error: test " << __FUNCTION__ << " failed on line " << __LINE__ << std::endl; \
        nfailed++;                                                                                  \
    }                                                                                               \
    else npassed++;                                                                                 \
}
        
#define COMPARE_TSTRING(str,ref) {                                                     \
    if (str.CompareTo(ref) != 0) {                                                     \
        std::cout << "Error: test " << __FUNCTION__ << " failed on line " << __LINE__  \
                  << " - expected " << ref << " but got " << str << std::endl;         \
        nfailed++;                                                                     \
    }                                                                                  \
    else npassed++;                                                                    \
}

#define EXPECTED_EXCEPTION(expr, ExceptionType){                                                                           \
    try{                                                                                                                   \
        expr;                                                                                                              \
        std::cout << "Error: test " << __FUNCTION__ << " failed on line " << __LINE__ << " (no exception)" << std::endl;   \
        nfailed++;                                                                                                         \
    }                                                                                                                      \
    catch(const ExceptionType &){ npassed++;}                                                                              \
    catch (...){                                                                                                           \
        std::cout << "Error: test " << __FUNCTION__ << " failed on line " << __LINE__ << " (wrong exception)" << std::endl;\
        nfailed++;                                                                                                         \
    }                                                                                                                      \
}

#endif