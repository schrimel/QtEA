#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#if defined(WIN32) && !defined(__linux__)
#define WINDOWS
#elif defined(__linux__) && !defined(WIN32)
#define POSIX
#endif

#endif // PLATFORM_HPP
