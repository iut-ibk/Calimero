#include <ExternalModel.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>
#include <CalibrationEnv.h>
#include <QProcess>
#include <Logger.h>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(ExternalModel)

ExternalModel::ExternalModel()
{
    setDataType("Timeout",INT,"-1");

    setDataType("Pre-Workspace",DIRSTRING,"");
    setDataType("Pre-exec-path",FILESTRING,"");
    setDataType("Pre-arguments",STRING,"");

    setDataType("Iteration-Workspace",DIRSTRING,"");
    setDataType("Iteration-exec-path", FILESTRING,"");
    setDataType("Iteration-arguments",STRING,"");

    setDataType("Post-Workspace",DIRSTRING,"");
    setDataType("Post-exec-path",FILESTRING,"");
    setDataType("Post-arguments",STRING,"");
}

bool ExternalModel::exec(Domain *dom)
{
    int timeout = QString::fromStdString(getValueOfParameter("Timeout")).toInt();

    QString iterationworkspace = QString::fromStdString(getValueOfParameter("Iteration-Workspace"));
    QString preworkspace = QString::fromStdString(getValueOfParameter("Pre-Workspace"));
    QString postworkspace = QString::fromStdString(getValueOfParameter("Post-Workspace"));

    QString preprog = QString::fromStdString(getValueOfParameter("Pre-exec-path"));
    QString iterationprog = QString::fromStdString(getValueOfParameter("Iteration-exec-path"));
    QString postprog = QString::fromStdString(getValueOfParameter("Post-exec-path"));

    QStringList preprogarguments = QString::fromStdString(getValueOfParameter("Pre-arguments")).split(" ");
    QStringList iterationprogarguments = QString::fromStdString(getValueOfParameter("Iteration-arguments")).split(" ");
    QStringList postprogarguments = QString::fromStdString(getValueOfParameter("Post-arguments")).split(" ");

    QProcess process;

    QDir dir(preworkspace);

    if(preprog!="" && !dir.exists(preprog))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Not able to start external program --- file or directory does not exist";
        return false;
    }

    dir.setPath(iterationworkspace);

    if(!dir.exists(iterationprog))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Not able to start external program --- file or directory does not exist";
        return false;
    }

    dir.setPath(postworkspace);

    if(postprog!="" && !dir.exists(postprog))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Not able to start external program --- file or directory does not exist";
        return false;
    }

    if(preprog!="")
    {
        process.setWorkingDirectory(preworkspace);
        process.start(preprog,preprogarguments);
        process.waitForStarted();

        if(!process.waitForFinished(timeout))
        {
            CalibrationEnv::getInstance()->stopCalibration();
            Logger(Error) << "Not able to start external pre - program";
            return false;
        }
    }

    process.setWorkingDirectory(iterationworkspace);
    process.start(iterationprog,iterationprogarguments);
    process.waitForStarted();

    if(!process.waitForFinished(timeout))
    {
        CalibrationEnv::getInstance()->stopCalibration();
        Logger(Error) << "Timeout of external iteration - program";
        return false;
    }

    if(postprog!="")
    {
        process.setWorkingDirectory(postworkspace);
        process.start(postprog,postprogarguments);
        process.waitForStarted();

        if(!process.waitForFinished(timeout))
        {
            CalibrationEnv::getInstance()->stopCalibration();
            Logger(Error) << "Not able to start external post - program";
            return false;
        }
    }
    return true;
}
