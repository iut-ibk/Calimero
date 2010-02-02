#include <QApplication>
#include "./gui/Gui.h"
#include "./core/CoreEngine.h"
#include "./gui/Controller.h"
#include "./core/MessageHandler.h"
#include "./core/Language.h"
#include <iostream>

using namespace std;


void printHelp()
{
    QString output =  "Usage: calimero [OPTION]\n";
            output += "Report bugs to <michael.mair@uibk.ac.at>";

    std::cout << output.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    //init
    int projectloaded = 0;
    LoggingOptions::getInstance()->debug=false;
    LoggingOptions::getInstance()->setPrintToTerminal(false);
    qInstallMsgHandler(myMessageOutput);
    QCoreApplication::setOrganizationName("IUT");
    QCoreApplication::setOrganizationDomain("http://www.uibk.ac.at/umwelttechnik/");
    QCoreApplication::setApplicationName("Calimero");

    CoreEngine *engine = new CoreEngine();

    for(int index=1; index<argc; index++)
    {
        if(QString(argv[index])=="-h")
        {
            printHelp();
            exit(0);
        }

        if(QString(argv[index])=="-d")
        {
            if(index+1>=argc)
            {
                std::cout << "-d needs a filename as parameter" << std::endl;
                exit(0);
            }

            LoggingOptions::getInstance()->debug=true;
            index++;
            LoggingOptions::getInstance()->setLogFile(argv[index]);
        }

        if(QString(argv[index])=="-l")
        {
            if(index+1>=argc)
            {
                std::cout << "-l needs a filename as parameter" << std::endl;
                exit(0);
            }

            index++;

            if(!engine->load(argv[index]))
            {
                std::cout << "Could not load calimero project file - Please start calimero with Gui ---> EXIT PROGRAM" << std::endl;
                exit(0);
            }
            else
            {
                projectloaded=true;
            }
        }

        if(QString(argv[index])=="-c")
        {
            if(index != argc-1)
            {
                std::cout << "-c must be the last argument" << std::endl;
                exit(0);
            }

            if(!projectloaded)
            {
                std::cout << "-l before -c" << std::endl;
                exit(0);
            }
            engine->setExitOnWarning(true);
            engine->enabledGui(false);
            engine->calc();
            return 0;
        }

        if(QString(argv[index])=="-v")
        {
            LoggingOptions::getInstance()->setPrintToTerminal(true);
            LoggingOptions::getInstance()->debug = true;
        }

        if(QString(argv[index])=="-r")
        {
            if(!projectloaded)
            {
                std::cout << "-l before -r" << std::endl;
                exit(0);
            }

            if(index+1>=argc)
            {
                std::cout << "-r needs a boolean as parameter [true|false]" << std::endl;
                exit(0);
            }

            index++;

            if(QString(argv[index])!="false" && QString(argv[index])!="true")
            {
                std::cout << "-r needs a boolean as parameter [true|false]" << std::endl;
                exit(0);
            }

            if(QString(argv[index])=="false")
            {
                engine->getModel()->setClearIterations(false);
            }
            else
            {
                engine->getModel()->setClearIterations(true);
            }
        }
    }

    if(engine->hasGui())
    {
        QApplication app( argc, argv );
        Controller controller(engine, 100);
        app.exec();
    }
    else
    {
        if(engine->getPersistenceFileName()=="")
        {
            qWarning() << "No calimero project loaded";
            return 0;
        }

        qWarning() << "Start calibration without Gui";
        engine->setExitOnWarning(true);
        engine->enabledGui(false);
        engine->calc();
    }

    return 0;
}

