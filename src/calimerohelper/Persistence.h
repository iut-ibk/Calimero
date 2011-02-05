#ifndef PERSISTENCE_H_INCLUDED
#define PERSISTENCE_H_INCLUDED

#include <QtXml>
#include <CalimeroGlob.h>
#include <string>
#include <vector>
#include <map>

class Calibration;
class IterationResult;
class QStatusBar;

#define CALIMERO_VERSION "Calimero_v1.10.05"

class CALIMERO_PUBLIC Persistence
{
    private:
        Calibration *calibration;
        QDomDocument *doc;
        QDomElement root;
        QStatusBar *status;

    public:
        Persistence(Calibration *calibration);
        ~Persistence();
        bool saveCalibration(QString filename,QStatusBar *status=0);
        bool loadCalibration(QString filename);

    private:
        //load functions
        bool loadCalibrationParameters();
        bool loadIterationParameters();
        bool loadObservedParameters();
        bool loadObjectiveFunctionParameters();
        bool loadFunction(QString &functionname, std::map<std::string,std::string> &functionparameters, QDomElement *element );
        bool loadConnections();
        bool loadModelSimulator();
        bool loadCalibrationAlgorithm();
        bool loadEnabledObjectiveFunctionParameters();
        bool loadGroups();
        bool loadIterationResults();
        bool loadTemplates();
        bool loadResultHandler();

        //save functions
        bool saveResultHandler(QTextStream *out);
        bool saveTemplates(QTextStream *out);
        bool saveIterationResults(QTextStream *out);
        bool saveGroups(QTextStream *out);
        bool saveEnabledObjectiveFunctionParameters(QTextStream *out);
        bool saveModelSimulator(QTextStream *out);
        bool saveCalibrationAlgorithm(QTextStream *out);
        bool saveCalibrationParameters(QTextStream *out);
        bool saveIterationParameters(QTextStream *out);
        bool saveObservedParameters(QTextStream *out);
        bool saveObjectiveFunctionParameters(QTextStream *out);
        bool saveFunction(QString functionname, std::map<std::string, std::string> parameters, QString *string);
        std::string vectorToString(std::vector<double> vec);
        std::vector<double> stringToVector(std::string stringvalue, bool *ok=NULL);
        /*
        bool saveGroups();
        bool saveIterationResults();
        */
};

#endif // PERSISTENCE_H_INCLUDED
