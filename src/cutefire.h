#ifndef CUTEFIRE_H
#define CUTEFIRE_H

#include <QtGlobal>

#ifdef Q_OS_WIN32

#ifdef CUTEFIRE_EXPORTS
#define CUTEFIRE_API __declspec(dllexport)
#else
#define CUTEFIRE_API __declspec(dllimport)
#endif

#else

#define CUTEFIRE_API __attribute__((visibility("default")))

#endif

#endif // CUTEFIRE_H
