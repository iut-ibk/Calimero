#ifndef FUNCTIONLOADER_H
#define FUNCTIONLOADER_H

#include <CalimeroGlob.h>
#include <string>

using namespace std;

class CALIMERO_PUBLIC FunctionLoader
{
public:
    static void loadNative(const string &path);
};

#endif // FUNCTIONLOADER_H
