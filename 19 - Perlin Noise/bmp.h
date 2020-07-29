/*
    bmp.h

    Adapted from Harvard CS50 Problem Set 4

    BMP-related data types based on Microsoft's own.
*/

#ifndef BMP_H
#define BMP_H

#include <stdint.h>

/*
    Defining a pack function unique to each platform

    Taken from https://stackoverflow.com/a/3312896
*/
#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

/*
    Common Data Types

    The data types in this section are essentially aliases for C/C++
    primitive data types.

    Adapted from http://msdn.microsoft.com/en-us/library/cc230309.aspx.
    See http://en.wikipedia.org/wiki/Stdint.h for more on stdint.h.
*/
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

/*
    BITMAPFILEHEADER

    The BITMAPFILEHEADER structure contains information about the type, size,
    and layout of a file that contains a DIB [device-independent bitmap].

    Adapted from http://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx.
*/
PACK(struct BITMAPFILEHEADER
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
});

/*
    BITMAPINFOHEADER

    The BITMAPINFOHEADER structure contains information about the
    dimensions and color format of a DIB [device-independent bitmap].

    Adapted from http://msdn.microsoft.com/en-us/library/dd183376(VS.85).aspx.
*/
PACK(struct BITMAPINFOHEADER
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
});

/*
    RGBTRIPLE

    This structure describes a color consisting of relative intensities of
    red, green, and blue.

    Adapted from http://msdn.microsoft.com/en-us/library/aa922590.aspx.
*/
PACK(struct RGBTRIPLE
{
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
});

#endif