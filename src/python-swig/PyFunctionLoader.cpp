#include <Python.h>
#include <PyFunctionLoader.h>
#include <QDir>
#include <Exception.h>
#include <CalibrationEnv.h>
#include <PyEnv.h>

using namespace std;

void PyFunctionLoader::loadScripts(const string &path)
{
    QDir dir(QString::fromStdString(path));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    PyEnv::getInstance()->addPythonPath(path);

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);

        QString file = fileInfo.fileName();

        if(!file.contains(".py") || file.contains(".pyc") || file.contains(".pyd"))
            continue;

        try
        {
            file = file.replace(".py","");
            Logger(Debug) << "Loading python module: " << file.toStdString();
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getCalibrationAlgReg(),file.toStdString(),true);
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getObjectiveFunctionReg(),file.toStdString(),false);
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getModelSimulatorReg(),file.toStdString(),false);
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getResultHandlerReg(),file.toStdString(),false);
        }
        catch (CalimeroException e)
        {
            Logger(Error) << "Not able to load python script file";
        }
    }
}
