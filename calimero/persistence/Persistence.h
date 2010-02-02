#ifndef PERSISTENCE_H_INCLUDED
#define PERSISTENCE_H_INCLUDED

#include <QtXml>

class Model;

class Persistence
{

    public:
        static bool loadFileAsString(QString filename, QString *file);
        static bool saveStringAsFile(QString filename, QString text);
        static bool saveModel(QString filename, Model* model);
        static int loadModel(QString filename, Model* model, bool forcescriptupdate = false);
        static QString vectorToString(QVector<double> vec);
        static QVector<double> stringToVector(QString string, bool *ok=NULL);

    private:
        static int loadModelParameters(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadCompareParameters(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadInputParameters(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadIterationParameters(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadResultParameters(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadGroups(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadIterationResults(QDomDocument *doc, Model *model, bool forcescriptupdate = false);
        static int loadCalibrationScript(QDomDocument *doc, Model *model, bool forcescriptupdate = false);

        static bool saveModelParameters(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveCompareParameters(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveInputParameters(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveIterationParameters(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveResultParameters(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveGroups(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveIterationResults(QDomElement &root, QDomDocument *doc, Model *model);
        static bool saveCalibrationScript(QDomElement &root, QDomDocument *doc, Model *model);

};

#endif // PERSISTENCE_H_INCLUDED
