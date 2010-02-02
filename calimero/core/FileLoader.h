#ifndef FILELOADER_H
#define FILELOADER_H

#include <QMutex>
#include "Model.h"

class FileLoader
{
    private:
        QMutex mutex;
        Model *model;
        QString filename;
        bool fileload;
        int fileloaderrorcode;
        bool forcescriptupdate;
    public:
        FileLoader();
        bool fileLoadFinished();
        int fileLoadErrorCode();
        void loadFile(QString file, Model *m, bool forcescriptupdates=false);
};

#endif // FILELOADER_H
