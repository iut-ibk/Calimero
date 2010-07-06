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

#define DEFAULTLOGLEVEL Standard

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    po::options_description desc("Calimero command line options");

    desc.add_options()
            ("help,h", "produce help message")
            ("nogui,c", "run calimero in command line mode")
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
            PyFunctionLoader::loadScripts("./");
            PyFunctionLoader::loadScripts(settings.value("calimerohome","./").toString().toStdString());
            FunctionLoader::loadNative("./");
            FunctionLoader::loadNative(settings.value("calimerohome","./").toString().toStdString());

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
                usleep(1000000);
            }

            persistence.saveCalibration(projectpath);
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
