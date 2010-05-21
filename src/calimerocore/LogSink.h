#ifndef LOGSINK_H
#define LOGSINK_H

#include <string>
#include <ostream>
#include <CalimeroGlob.h>
#include <QMutex>
#include <QMutexLocker>

class LSEndl {};

class CALIMERO_PUBLIC LogSink
{
public:
        virtual ~LogSink() {}
        virtual void close() = 0;
        virtual LogSink &operator<<(const std::string &string) = 0;
        virtual LogSink &operator<<(const char *string) = 0;
        virtual LogSink &operator<<(int i) = 0;
        virtual LogSink &operator<<(LSEndl) = 0;
};

class CALIMERO_PUBLIC OStreamLogSink : public LogSink {
public:
        OStreamLogSink(std::ostream &ostream);
        ~OStreamLogSink();
        void close() {}
        LogSink &operator<<(const std::string &string);
        LogSink &operator<<(const char *string);
        LogSink &operator<<(int i);
        LogSink &operator<<(LSEndl e);

private:
        std::ostream &out;
        QMutex *mutex;
};

#endif // LOGSINK_H
