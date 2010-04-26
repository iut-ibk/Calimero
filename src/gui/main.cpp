#include <QtGui/QApplication>
#include <iostream>
#include <mainwindow.h>
#include <CalibrationEnv.h>


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QApplication::setApplicationName("Calimero");
	QApplication::setOrganizationName("University of Innsbruck - "
								"Unit of Environmental Engineering");
	QApplication::setOrganizationDomain("http://www.uibk.ac.at/umwelttechnik/");
        CalibrationEnv::getInstance()->setCalibration(new Calibration());
        MainWindow w;
        w.show();
	return a.exec();
}
