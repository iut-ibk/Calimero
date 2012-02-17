/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#include <QtGui/QApplication>
#include <iostream>
#include <mainwindow.h>
#include <CalibrationEnv.h>
#include <PyFunctionLoader.h>
#include <FunctionLoader.h>
#include <QSettings>
#include <Log.h>
#include <Logger.h>
#include <LogSink.h>
#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <PyEnv.h>

#define DEFAULTLOGLEVEL Standard

namespace po = boost::program_options;
using namespace std;


class Xsleep : public QThread
{
public:
    static void msleep(int ms)
    {
        QThread::msleep(ms);
    }
};

int main(int argc, char **argv)
{
    po::options_description desc("Calimero command line options");

    desc.add_options()
            ("help,h", "produce help message")
            ("nogui,c", "run calimero in command line mode")
            ("analyses,a", "run all enabled result analyses after calibration")
            ("log,l", po::value<std::string>(), "write log to specified file")
            ("maxlog,v", po::value<int>(), "max loglevel\n0 all debug\n1 all standard\n2 all warnings\n3 only errors")
            ("project,p", po::value<std::string>(), "project which specifies a calibration");
    ;

    boost::program_options::positional_options_description p;
    p.add("project", 1);

    boost::program_options::variables_map vm;
    try {
            po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    } catch (po::unknown_option o) {
            std::cout << "unknown option " << endl << endl;
            std::cout << desc << std::endl;
            return 1;
    }

    po::notify(vm);

    if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 1;
    }

    //check debug level
    LogLevel max = DEFAULTLOGLEVEL;
    if (vm.count("maxlog")) {
            int l = vm["maxlog"].as<int>();
            if (l < 0 || l > 3) {
                    cerr << desc << endl;
                    cerr << "log level must be between 0 an 3" << endl;
                    return -1;
            }
            max = LogLevel(l);
    }

    //check for loading file
    //loading project if exist
    QString projectpath = "";
    if (vm.count("project")) {
        projectpath = QString::fromStdString(vm["project"].as<std::string>());
    }

    //try do start app
    QApplication a(argc, argv);
    QApplication::setApplicationName("Calimero");
    QApplication::setOrganizationName("IUT");
    QApplication::setOrganizationDomain("http://www.uibk.ac.at/umwelttechnik/");
    CalibrationEnv::getInstance()->setCalibration(new Calibration());

    QSettings settings;
    if(!settings.contains("calimerohome") || settings.value("calimerohome").toString().isEmpty())
        settings.setValue("calimerohome","/usr/share/calimero/libs,/usr/share/calimero/scripts/site-packages,/usr/share/calimero/scripts");

    if(vm.count("nogui"))
    {
            if (!vm.count("project")) {
                    std::cout << desc << std::endl;
                    std::cerr << "provide a project file" << std::endl;
                    return -1;
            }

            ostream *out = &cout;
            if (vm.count("log")) {
                    std::string log_file = vm["log"].as<std::string>();
                    if (ifstream(log_file.c_str())) {
                            cerr << "file already exists " << log_file << endl;
                            return -1;
                    }
                    out = new ofstream(log_file.c_str());
            }

            Log::init(new OStreamLogSink(*out), max);

            Logger(Debug) << "starting";


            //load scripts
            QSettings settings;

            QStringList pythonhome = settings.value("pythonhome",QStringList()).toString().replace("\\","/").split(",");
            for (int index = 0; index < pythonhome.size(); index++)
                PyEnv::getInstance()->addPythonPath(pythonhome.at(index).toStdString());

            QStringList pathlist = settings.value("calimerohome",QStringList()).toString().replace("\\","/").split(",");

            for (int index = 0; index < pathlist.size(); index++)
            {
                PyEnv::getInstance()->addPythonPath(pathlist.at(index).toStdString());
            }

            for (int index = 0; index < pathlist.size(); index++)
            {
                if(pathlist.at(index).contains("site-packages"))
                    continue;
                PyFunctionLoader::loadScripts(pathlist.at(index).toStdString());
                FunctionLoader::loadNative(pathlist.at(index).toStdString());
            }

            Logger(Standard) << "Loading file project:" << vm["project"].as<std::string>();
            Persistence persistence(CalibrationEnv::getInstance()->getCalibration());
            if(!persistence.loadCalibration(projectpath))
            {
                Logger(Error) << "Cannot load project";
                return -1;
            }

            CalibrationEnv::getInstance()->startCalibration();

            while(CalibrationEnv::getInstance()->isCalibrationRunning())
            {
                Logger(Standard) << "Complete Iterations: " << CalibrationEnv::getInstance()->getCalibration()->getIterationResults().size();
                Xsleep::msleep(1000);
            }

            Logger(Standard) << "Save calibration data";
            persistence.saveCalibration(projectpath);
            Logger(Standard) << "Calibration data saved";

            if (vm.count("analyses"))
            {
                    Logger(Standard) << "Run all enabled result analyses";
                    CalibrationEnv::getInstance()->runAllEnabledResultHandler();
                    Logger(Standard) << "Result analyses finished";
            }

            Logger(Standard) << "Calibration finished";
            Logger(Debug) << "shutting down";
            Log::shutDown();
            return 0;
    }
    else
    {
        MainWindow w(0,projectpath, max);
        w.show();

        return a.exec();
    }
}
