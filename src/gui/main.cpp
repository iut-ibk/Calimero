#include <QtGui/QApplication>
#include <iostream>
#include <mainwindow.h>
#include <CalibrationEnv.h>
#include <PyFunctionLoader.h>
#include <FunctionLoader.h>
#include <QSettings>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QApplication::setApplicationName("Calimero");
        QApplication::setOrganizationName("IUT");
	QApplication::setOrganizationDomain("http://www.uibk.ac.at/umwelttechnik/");
        CalibrationEnv::getInstance()->setCalibration(new Calibration());
        MainWindow w;
        QSettings settings;
        PyFunctionLoader::loadScripts("./");
        PyFunctionLoader::loadScripts(settings.value("calimerohome","./").toString().toStdString());
        FunctionLoader::loadNative("./");
        FunctionLoader::loadNative(settings.value("calimerohome","./").toString().toStdString());
        //w.updateAll();
        w.show();
        return a.exec();
}
