#ifndef PYFUNCTIONLOADER_H
#define PYFUNCTIONLOADER_H

#include <string>
#include <CalimeroGlob.h>

using namespace std;

class CALIMERO_PUBLIC PyFunctionLoader
{
public:
    static void loadScripts(const string &path);
};

#endif // PYFUNCTIONLOADER_H
