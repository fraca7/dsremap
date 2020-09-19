
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifdef WIN32
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

#endif
