#ifndef PYTHONEXCEPTION_H
#define PYTHONEXCEPTION_H

#include <string>
#include <CalimeroGlob.h>

class CALIMERO_PUBLIC PythonException {
public:
	PythonException() {}
	PythonException(std::string error);
	std::string type;
	std::string value;
	std::string traceback;
};

#endif // PYTHONEXCEPTION_H
