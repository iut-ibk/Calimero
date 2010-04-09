#ifndef LOGGER_H
#define LOGGER_H

#include <Log.h>
#include <CalimeroGlob.h>

using namespace boost;

using namespace std;

class QString;
class LogSink;
class Variable;

class CALIMERO_PUBLIC Logger {
public:
        Logger(LogLevel level = Standard);
        virtual ~Logger();
        Logger &operator<< (LogLevel level);
        Logger &operator<< (const Variable *var);
        Logger &operator<< (const char* s);
        Logger &operator<< (const int i);
        Logger &operator<< (const size_t i);
        Logger &operator<< (const long i);
        Logger &operator<< (const double f);
        Logger &operator<< (const float f);
        Logger &operator<< (const string &i);
        Logger &operator<< (const QString &s);
private:
        string logLevel() const;
        string date() const;
        LogSink &sink;
        LogLevel level, max;
        bool dirty;
};


#endif // LOGGER_H
