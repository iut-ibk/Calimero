#include <FunctionLoader.h>
#include <QDir>
#include <iostream>
#include <CalibrationEnv.h>

void FunctionLoader::loadNative(const string &path)
{
    QDir dir(QString::fromStdString(path));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString file = fileInfo.absoluteFilePath();

        if(!(file.contains(".dll") || file.contains(".so")))
            continue;

        if(!CalibrationEnv::getInstance()->getCalibrationAlgReg()->addNativePlugin(file.toStdString()))
            continue;
        if(!CalibrationEnv::getInstance()->getObjectiveFunctionReg()->addNativePlugin(file.toStdString()))
            continue;
        if(!CalibrationEnv::getInstance()->getModelSimulatorReg()->addNativePlugin(file.toStdString()))
            continue;
    }
}
