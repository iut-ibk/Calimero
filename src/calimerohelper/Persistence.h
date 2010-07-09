#ifndef PERSISTENCE_H_INCLUDED
#define PERSISTENCE_H_INCLUDED

#include <QtXml>
#include <CalimeroGlob.h>
#include <string>
#include <vector>
#include <map>

class Calibration;
class IterationResult;

#define CALIMERO_VERSION "Calimero_v1.10.05"

class CALIMERO_PUBLIC Persistence
{
    private:
        Calibration *calibration;
        QDomDocument *doc;
        QDomElement root;
    public:
        Persistence(Calibration *calibration);
        ~Persistence();
        bool saveCalibration(QString filename);
        bool buildXMLTree();
        bool saveIterationResult(IterationResult *result);

        bool loadCalibration(QString filename);
        std::string vectorToString(std::vector<double> vec);
        std::vector<double> stringToVector(std::string stringvalue, bool *ok=NULL);


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
        bool saveResultHandler();
        bool saveTemplates();
        bool saveIterationResults();
        bool saveGroups();
        bool saveEnabledObjectiveFunctionParameters();
        bool saveModelSimulator();
        bool saveCalibrationAlgorithm();
        bool saveCalibrationParameters();
        bool saveIterationParameters();
        bool saveObservedParameters();
        bool saveObjectiveFunctionParameters();
        bool saveFunction(QString functionname, std::map<std::string, std::string> parameters, QDomElement *element);
        /*
        bool saveGroups();
        bool saveIterationResults();
        */
};

#endif // PERSISTENCE_H_INCLUDED
