#include <Logger.h>
#include <QDateTime>
#include <QString>
#include <LogSink.h>
#include <Variable.h>

Logger::Logger(LogLevel level)
 : sink(*Log::getInstance()->sink), level(level) {
        dirty = false;
        this->max = Log::getInstance()->max;
        if (level >= max) {
                sink << logLevel() << " " << date() << "|";
                dirty = true;
        }
}

Logger::~Logger() {
        if (dirty)
                sink << LSEndl();
}

Logger &Logger::operator <<(LogLevel new_level) {
        level = new_level;
        sink << "\n" << logLevel() << " " << date() << "|";
        dirty = true;
        return *this;
}

Logger &Logger::operator <<(const Variable *var) {
        if (level < max) {
                return *this;
        }

        switch(var->getType())
        {
        case Domain::CALIBRATIONVARIABLE:
            sink << " CalibrationParameter[" << var->getName() << "] ";
            break;
        case Variable::OBJECTIVEFUNCTIONVARIABLE:
            sink << " ObjectiveFunctionParameter[" << var->getName() << "] ";
            break;
        case Variable::OBSERVEDVARIABLE:
            sink << " ObservedParameter[" << var->getName() << "] ";
            break;
        case Variable::ITERATIONVARIABLE:
            sink << " IterationParameter[" << var->getName() << "] ";
            break;
        }

        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const char* s) {
        if (level < max) {
                return *this;
        }
        sink << " " << s;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const int i) {
        if (level < max) {
                return *this;
        }
        sink << " " << i;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const size_t i) {
        if (level < max) {
                return *this;
        }
        sink << " " << i;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const long i) {
        if (level < max) {
                return *this;
        }
        sink << " " << i;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const double f) {
        if (level < max) {
                return *this;
        }
        sink << " " << f;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const float f) {
        if (level < max) {
                return *this;
        }
        sink << " " << f;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const string &s) {
        if (level < max) {
                return *this;
        }
        sink << " " << s;
        dirty = true;
        return *this;
}

Logger &Logger::operator<< (const QString &s) {
        if (level < max) {
                return *this;
        }
        sink << " " << s.toStdString();
        dirty = true;
        return *this;
}


string Logger::logLevel() const {
        switch (level) {
                case Debug:
                        return "DEBUG";
                case Warning:
                        return "WARN";
                case Standard:
                        return "STANDARD";
                case Error:
                        return "ERROR";
        }
        return "UNKNOWN";
}

string Logger::date() const {
        return QDateTime::currentDateTime().toString().toStdString();
}

