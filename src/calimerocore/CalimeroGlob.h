#ifndef CALIMEROGLOBALS_H
#define CALIMEROGLOBALS_H

#ifdef _MSC_VER
#pragma warning( disable: 4251 )
#endif
#ifdef _WIN32
#define CALIMERO_PUBLIC __declspec(dllexport)
#else
#define CALIMERO_PUBLIC
#endif

#endif // CALIMEROGLOBALS_H
