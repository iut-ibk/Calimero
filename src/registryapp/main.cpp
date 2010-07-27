#include <iostream>
#include <QSettings>
#include <QApplication>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace std;

int main(int argc, char **argv)
{
    po::options_description desc("Registry Handler command line options");

    desc.add_options()
            ("help,h", "produce help message")
            ("appname,a", po::value<std::string>(), "Application name")
            ("orgname,o", po::value<std::string>(), "Organization name")
            ("orgdomain,d", po::value<std::string>(), "Organization domain")
            ("regname,r", po::value<std::string>(), "Registry entry name")
            ("setvalue,s", po::value<std::string>(), "set value of registry entry");
    ;

    boost::program_options::positional_options_description p;
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

    if(!vm.count("appname")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if(!vm.count("orgname")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if(!vm.count("orgdomain")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if(!vm.count("regname")) {
        std::cout << desc << std::endl;
        return 1;
    }

    //try do start app
    QApplication a(argc, argv);
    QApplication::setApplicationName(QString::fromStdString(vm["appname"].as<std::string>()));
    QApplication::setOrganizationName(QString::fromStdString(vm["orgname"].as<std::string>()));
    QApplication::setOrganizationDomain(QString::fromStdString(vm["orgdomain"].as<std::string>()));
    QSettings settings;

    if(!vm.count("setvalue")) {
        std::cout << settings.value("calimerohome","./").toString().toStdString() << std::endl;
        return 1;
    }
    else
    {
        settings.setValue("calimerohome",QString::fromStdString(vm["setvalue"].as<std::string>()));
    }

    return 1;
}
