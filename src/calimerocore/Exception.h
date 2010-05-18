#ifndef PYTHONEXCEPTION_H
#define PYTHONEXCEPTION_H

#include <string>
#include <CalimeroGlob.h>

enum  EXCEPTIONTYPE {   CALIMEROEXCEPTION = 0,
                        BOOSTPYTHONEXCEPTION = 1
              };

class CALIMERO_PUBLIC CalimeroException{
public:
    EXCEPTIONTYPE exceptiontype;
    std::string exceptionmsg;

    CalimeroException()
    {
        exceptiontype=CALIMEROEXCEPTION;
        exceptionmsg="Default calimero exception";
    }

    CalimeroException(std::string exceptionmsg)
    {
        this->exceptiontype=CALIMEROEXCEPTION;
        this->exceptionmsg=exceptionmsg;
    }
};

class CALIMERO_PUBLIC PythonException : public CalimeroException {
public:
        PythonException(std::string error, std::string msg);
        std::string type;
        std::string value;
        std::string traceback;
};
#endif // PYTHONEXCEPTION_H
