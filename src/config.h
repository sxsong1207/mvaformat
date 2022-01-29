#ifndef MVAFORMAT_DLLMACRO_H
#define MVAFORMAT_DLLMACRO_H

#define _USE_EIGEN
#define _USE_GZSTREAM
#define _USE_ZSTDSTREAM

#if defined(_WIN32) && !defined(MVAFORMAT_STATIC)
#ifdef MVAFORMAT_API_EXPORTS
#define MVAFORMAT_API __declspec(dllexport)
#else
#define MVAFORMAT_API __declspec(dllimport)
#endif
#else
#define MVAFORMAT_API
#endif

#endif // MVAFORMAT_DLLMACRO_H
