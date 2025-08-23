// definition of the loggers
#define LOG_ERROR(msg) std::cerr << "\033[01;31m[ERROR]\033[0m " << msg << std::endl
#define LOG_WARN(msg) std::cerr << "\033[01;33m[WARNING]\033[0m " << msg << std::endl
#define LOG_INFO(msg) std::cout << "\033[01;32m[INFO]\033[0m " << msg << std::endl
#ifdef NDEBUG
#define LOG_DEBUG(msg) ((void)0)
#define PRINT_LINE_DEBUG() ((void)0)
#else
#define LOG_DEBUG(msg) std::cout << "\033[01;36m[DEBUG]\033[0m " << msg << std::endl
#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define PRINT_LINE_DEBUG() LOG_DEBUG(__FILENAME__ << ":" << __LINE__ << " in " << __FUNCTION__);
#endif
