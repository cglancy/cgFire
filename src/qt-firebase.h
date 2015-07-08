#ifndef QTFIREBASE_H
#define QTFIREBASE_H

#include <QtGlobal>

#ifdef Q_OS_WIN32

#ifdef QTFIREBASE_EXPORTS
#define QTFIREBASE_API __declspec(dllexport)
#else
#define QTFIREBASE_API __declspec(dllimport)
#endif

#else

#define QTFIREBASE_API __attribute__((visibility("default")))

#endif

#endif // QTFIREBASE_H
