#include <LogSink.h>

OStreamLogSink::OStreamLogSink(std::ostream &ostream) : out(ostream) {
    mutex = new QMutex(QMutex::Recursive);
}

OStreamLogSink::~OStreamLogSink()
{
    delete mutex;
}

LogSink &OStreamLogSink::operator<<(const std::string &string) {
        QMutexLocker locker(mutex);
        out << string;
        return *this;
}

LogSink &OStreamLogSink::operator<<(const char *string) {
        QMutexLocker locker(mutex);
        out << string;
        return *this;
}

LogSink &OStreamLogSink::operator<<(int i) {
        QMutexLocker locker(mutex);
        out << i;
        return *this;
}

LogSink &OStreamLogSink::operator<<(LSEndl e) {
        QMutexLocker locker(mutex);
        out << std::endl;
        return *this;
}
