
#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef WIN32
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

#define CLAMPU1(x) ((x) <= 0 ? 0 : 1)
#define CLAMPU8(x) ((uint8_t)((x) < 0 ? 0 : (((x) > 255) ? 255 : (x))))
#define CLAMPS16(x) ((int16_t)((x) < -32768 ? -32768 : ((x) > 32767 ? 32767 : (x))))

#endif
