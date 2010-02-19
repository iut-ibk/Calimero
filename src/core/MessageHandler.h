#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QtCore>

class LoggingOptions
{
    private:
        static LoggingOptions* _instance;
        QTextStream *logfile;
        QMutex *mutex;
        QFile *file;
        bool printtoterminal;

        LoggingOptions()
        {
            logfile = NULL;
            debug = true;
            warning = true;
            critical = true;
            fatal = true;
            printtoterminal=true;
            mutex = new QMutex();
        }
        ~LoggingOptions()
        {
          if( NULL != LoggingOptions::_instance )
          {
             delete LoggingOptions::_instance;
             LoggingOptions::_instance = NULL;
          }

          if(containsLogFile())
          {
              logfile->flush();
              file->close();
              delete file;
              delete logfile;
          }
          delete mutex;
        }

    public:
        bool debug;
        bool warning;
        bool critical;
        bool fatal;

        QMutex* getMutex()
        {
            return mutex;
        }

        QTextStream* getLogFile()
        {
            return logfile;
        }

        bool getPrintToTerminal()
        {
            return printtoterminal;
        }

        void setPrintToTerminal(bool print)
        {
            printtoterminal=print;
        }

        bool containsLogFile()
        {
            return (logfile!=NULL);
        }

        bool setLogFile(QString filename)
        {
            file = new QFile(filename);

            if (file->open(QFile::WriteOnly | QFile::Truncate))
            {
                logfile = new QTextStream(file);
                return true;
            }
            return false;
        }

        static LoggingOptions* getInstance()
        {
            if (!_instance)
                _instance = new LoggingOptions();
            return _instance;
        }
};

LoggingOptions* LoggingOptions::_instance = 0;

void myMessageOutput(QtMsgType type, const char *msg)
{
    QMutexLocker locker(LoggingOptions::getInstance()->getMutex());

    switch (type) {
    case QtDebugMsg:
        if(LoggingOptions::getInstance()->getPrintToTerminal() && LoggingOptions::getInstance()->debug)
            fprintf(stderr, "Debug: %s\n", msg);
        if(LoggingOptions::getInstance()->containsLogFile() && LoggingOptions::getInstance()->debug)
            *LoggingOptions::getInstance()->getLogFile() << "Debug: " << msg << "\n";
        break;
    case QtWarningMsg:
        if(LoggingOptions::getInstance()->warning)
            fprintf(stderr, "Warning: %s\n", msg);
        if(LoggingOptions::getInstance()->containsLogFile() && LoggingOptions::getInstance()->warning)
            *LoggingOptions::getInstance()->getLogFile() << "Warning: " << msg << "\n";
        break;
    case QtCriticalMsg:
        if(LoggingOptions::getInstance()->critical)
            fprintf(stderr, "Critical: %s\n", msg);
        if(LoggingOptions::getInstance()->containsLogFile() && LoggingOptions::getInstance()->critical)
            *LoggingOptions::getInstance()->getLogFile() << "Critical: " << msg << "\n";
        break;
    case QtFatalMsg:
        if(LoggingOptions::getInstance()->fatal)
            fprintf(stderr, "Fatal: %s\n", msg);
        if(LoggingOptions::getInstance()->containsLogFile() && LoggingOptions::getInstance()->fatal)
            *LoggingOptions::getInstance()->getLogFile() << "Fatal: " << msg << "\n";
        abort();
    }

    if(LoggingOptions::getInstance()->containsLogFile())
        LoggingOptions::getInstance()->getLogFile()->flush();
}
#endif // MESSAGEHANDLER_H
