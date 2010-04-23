#include <FunctionLoader.h>
#include <QDir>
#include <iostream>
#include <PyEnv.h>
#include <CalibrationEnv.h>
#include <PyException.h>

void FunctionLoader::loadScripts(const string &path)
{
    QDir dir(QString::fromStdString(path));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    PyEnv::getInstance()->addPythonPath(path);

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);

        QString file = fileInfo.fileName();

        if(!file.contains(".py") || file.contains(".pyc"))
            continue;

        try
        {
            file = file.replace(".py","");
            Logger(Debug) << "Loading python module: " << file.toStdString();
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getCalibrationAlgReg(),file.toStdString());
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getObjectiveFunctionReg(),file.toStdString());
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getModelSimulatorReg(),file.toStdString());
        }
        catch (PythonException e)
        {
            Logger(Error) << e.type;
            Logger(Error) << e.value;
        }
    }
}

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
