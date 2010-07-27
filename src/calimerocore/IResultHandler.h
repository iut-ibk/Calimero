#ifndef IRESULTHANDLER_H
#define IRESULTHANDLER_H

#include <IFunction.h>
#include <CalimeroGlob.h>
#include <Logger.h>
#include <map>
#include <IterationResult.h>

#define CALIMERO_DECLARE_RESULTFUNCTION(function)  \
class CALIMERO_PUBLIC function : public IResultHandler { \
public: \
        static const char *name; \
        const char *getClassName() const; \
private:

#define CALIMERO_DECLARE_RESULTFUNCTION_NAME(functionname) \
const char *functionname::name = #functionname; \
const char *functionname::getClassName() const { return functionname::name; }

class CALIMERO_PUBLIC IResultHandler : public IFunction
{
    public:
        static IFUNCTIONTYPE getType()
        {
            return RESULTFUNCTION;
        };

        IResultHandler(){};
        virtual bool run(std::vector<shared_ptr<IterationResult> > iterationresults) = 0;
        bool test(){Logger(Error) << "test"; return false;};
};

#endif // IRESULTHANDLER_H
