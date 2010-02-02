#include "FileLoader.h"
#include <QMutexLocker>

FileLoader::FileLoader()
{
}

bool FileLoader::fileLoadFinished()
{
    return fileload;
}

int FileLoader::fileLoadErrorCode()
{
    QMutexLocker locker(&mutex);
    return fileloaderrorcode;
}

void FileLoader::loadFile(QString file, Model *m, bool forcescriptupdates)
{
    QMutexLocker locker(&mutex);
    filename = file;
    model = m;
    fileload=false;
    forcescriptupdate=forcescriptupdates;

    if(filename!="" && model!=NULL)
    {
        fileload=false;
        fileloaderrorcode=1;
        qDebug() << "FileLoader::run()";

        int error = model->modelLoad(filename,forcescriptupdate);

        if(error<1)
        {
            model->clear();
            fileload=true;
            fileloaderrorcode=error;
            filename="";

        }
        else
        {
            fileload=true;
            fileloaderrorcode=error;
            filename="";
        }
    }
}

