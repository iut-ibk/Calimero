#include "guilogsink.h"
#include <QPlainTextEdit>
#include <sstream>
#include <Log.h>

GuiLogSink::GuiLogSink() {
    mutex = new QMutex(QMutex::Recursive);
}

GuiLogSink::~GuiLogSink() {
    delete mutex;
}

LogSink &GuiLogSink::operator<<(const std::string &string) {
        QMutexLocker locker(mutex);
	buf += QString::fromStdString(string);
	return *this;
}

LogSink &GuiLogSink::operator<<(const char *string) {
        QMutexLocker locker(mutex);
	buf += QString(string);
	return *this;
}

LogSink &GuiLogSink::operator<<(int i) {
        QMutexLocker locker(mutex);
	buf += QString("%0").arg(i);
	return *this;
}

LogSink &GuiLogSink::operator<<(LSEndl i) {
        QMutexLocker locker(mutex);
	Q_EMIT newLogLine(buf);
	buf.clear();
	return *this;
}
