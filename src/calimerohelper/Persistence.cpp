#include <QtXml>
#include <QtCore>
#include <Persistence.h>
#include <Calibration.h>
#include <CalibrationVariable.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <IterationResult.h>
#include <iostream>
#include <qdom.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qtextstream.h>
#include <Logger.h>
#include <set>
#include <Variable.h>
#include <map>
#include <ExternalParameterRegistry.h>
#include <boost/shared_ptr.hpp>

using namespace boost;
using namespace std;

Persistence::Persistence(Calibration *calibration)
{
    this->calibration=calibration;
    doc = new QDomDocument();
}

Persistence::~Persistence()
{
    delete doc;
}

bool Persistence::saveCalibrationParameters(QTextStream *out)
{
    vector<Variable*> calibrationparameters = calibration->getDomain()->getAllPars(CALIBRATIONVARIABLE);

    BOOST_FOREACH(Variable * currentvar, calibrationparameters)
    {
        CalibrationVariable *tmpvar = static_cast<CalibrationVariable*>(currentvar);
        QString par = "\t<calibrationparameter value=\"" + QString::fromStdString(vectorToString(tmpvar->getValues())) +
                      "\" min=\"" + QString::number(tmpvar->getMin()) +
                      "\" step=\"" + QString::number(tmpvar->getStep()) +
                      "\" name=\"" + QString::fromStdString(tmpvar->getName()) +
                      "\" max=\"" + QString::number(tmpvar->getMax()) +
                      "\" initvalue=\"" + QString::fromStdString(vectorToString(tmpvar->getInitValues())) +
                      "\"/>\n";
        *out << par;
    }

    return true;
}

bool Persistence::saveObservedParameters(QTextStream *out)
{
    vector<Variable*> calibrationparameters = calibration->getDomain()->getAllPars(OBSERVEDVARIABLE);

    BOOST_FOREACH(Variable * tmpvar, calibrationparameters)
    {
        QString par = "\t<observedparameter value=\"" + QString::fromStdString(vectorToString(tmpvar->getValues())) +
                      "\" name=\"" + QString::fromStdString(tmpvar->getName()) +
                      "\"/>\n";
        *out << par;
    }

    return true;
}

bool Persistence::saveIterationParameters(QTextStream *out)
{
    vector<Variable*> calibrationparameters = calibration->getDomain()->getAllPars(ITERATIONVARIABLE);

    BOOST_FOREACH(Variable * tmpvar, calibrationparameters)
    {
        QString par = "\t<iterationparameter value=\"" + QString::fromStdString(vectorToString(tmpvar->getValues())) +
                      "\" name=\"" + QString::fromStdString(tmpvar->getName()) +
                      "\"/>\n";
        *out << par;
    }

    return true;
}

bool Persistence::saveObjectiveFunctionParameters(QTextStream *out)
{
    vector<Variable*> calibrationparameters = calibration->getDomain()->getAllPars(OBJECTIVEFUNCTIONVARIABLE);

    BOOST_FOREACH(Variable * currentvar, calibrationparameters)
    {
        ObjectiveFunctionVariable *tmpvar = static_cast<ObjectiveFunctionVariable*>(currentvar);
        QString par = "\t<objectivefunctionparameter value=\"" + QString::fromStdString(vectorToString(tmpvar->getValues())) +
                      "\" name=\"" + QString::fromStdString(tmpvar->getName()) +
                      "\">\n";

        saveFunction(QString::fromStdString(tmpvar->getObjectiveFunction()),tmpvar->getObjectiveFunctionSettings(),&par);

        par += "\t</objectivefunctionparameter>\n";

        //save members
        set<string> members;

        members = tmpvar->getIterationParameters();
        for( std::set<string>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            par += "\t<connection destination=\"" + QString::fromStdString(tmpvar->getName()) +
                   "\" source=\"" + QString::fromStdString(*it) +
                   "\"/>\n";
        }

        members = tmpvar->getObservedParameters();
        for( std::set<string>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            par += "\t<connection destination=\"" + QString::fromStdString(tmpvar->getName()) +
                   "\" source=\"" + QString::fromStdString(*it) +
                   "\"/>\n";
        }

        members = tmpvar->getObjectiveFunctionParameters();
        for( std::set<string>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            par += "\t<connection destination=\"" + QString::fromStdString(tmpvar->getName()) +
                   "\" source=\"" + QString::fromStdString(*it) +
                   "\"/>\n";
        }

        *out << par;
    }

    return true;
}

bool Persistence::loadResultHandler()
{
    QDomNodeList handlers = root.elementsByTagName("resulthandler");

    for(int index=0; index<handlers.size(); index++)
    {
        QDomElement handler = handlers.at(index).toElement();
        map<string, string> functionparameters;
        QString functionname;
        bool enabled = false;

        if(!loadFunction(functionname,functionparameters,&handler))
            return false;

        if(handler.attribute("enabled").toDouble())
            enabled=true;

        if(functionname!="")
            if(!calibration->addResultHandler(handler.attribute("name").toStdString(),functionname.toStdString(),functionparameters,enabled))
                return false;
    }
    return true;
}

bool Persistence::saveResultHandler(QTextStream *out)
{
    map<string, string> handlers = calibration->getResultHandlers();
    std::pair<string, string> p;

    BOOST_FOREACH(p, handlers)
    {
        QString par = "\t<resulthandler enabled=\"" + QString::number(calibration->isResultHandlerEnabled(p.first)) +
                          "\" name=\"" + QString::fromStdString(p.first) +
                          "\">\n";

        saveFunction(QString::fromStdString(p.second),calibration->getResultHandlerSettings(p.first),&par);

        par += "\t</resulthandler>\n";
        *out << par;
    }

    return true;
}

bool Persistence::loadModelSimulator()
{
    QDomNodeList parameters = root.elementsByTagName("modelsimulatoralgorithm");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement alg = parameters.at(index).toElement();
        QString functionname;
        map<string, string> functionparameters;

        if(!loadFunction(functionname,functionparameters,&alg))
            return false;

        if(functionname!="")
            if(!calibration->setModelSimulator(functionname.toStdString(),functionparameters))
                return false;
    }
    return true;
}

bool Persistence::loadCalibrationAlgorithm()
{
    QDomNodeList parameters = root.elementsByTagName("calibrationalgorithm");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement alg = parameters.at(index).toElement();
        QString functionname;
        map<string, string> functionparameters;

        if(!loadFunction(functionname,functionparameters,&alg))
            return false;

        if(functionname!="")
            if(!calibration->setCalibrationAlg(functionname.toStdString(),functionparameters))
                return false;
    }
    return true;
}

bool Persistence::loadEnabledObjectiveFunctionParameters()
{
    QDomNodeList parameters = root.elementsByTagName("enabledobjectivefunction");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement currentparameter = parameters.at(index).toElement();
        if(!calibration->addEnabledOParameter(currentparameter.attribute("name").toStdString()))
                return false;
    }
    return true;
}

bool Persistence::loadGroups()
{
    QDomNodeList parameters = root.elementsByTagName("group");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement currentgroup = parameters.at(index).toElement();
        string name = currentgroup.attribute("name").toStdString();
        calibration->addGroup(name);

        if(currentgroup.attribute("enabledgroup").toDouble())
            calibration->addEnabledGroup(name);
        else
            calibration->removeEnabledGroup(name);

        if(currentgroup.attribute("disabledgroup").toDouble())
            calibration->addDisabledGroup(name);
        else
            calibration->removeDisabledGroup(name);

        //load members
        QDomNodeList members = currentgroup.elementsByTagName("groupmember");
        for(int i=0; i<members.size(); i++)
        {
            QDomElement currentmember = members.at(i).toElement();
            calibration->addParameterToGroup(currentmember.attribute("name").toStdString(),name);
        }
    }
    return true;
}

bool Persistence::saveGroups(QTextStream *out)
{
    //save all groups
    vector<string> groups = calibration->getAllGroups();
    BOOST_FOREACH(string name, groups)
    {
        QString par = "\t<group enabledgroup=\"" + QString::number(calibration->isEnabledGroup(name)) +
                      "\" disabledgroup=\"" + QString::number(calibration->isDisabledGroup(name)) +
                      "\" name=\"" + QString::fromStdString(name) +
                      "\">\n";

        *out << par;

        //save groupmembers
        set<string> members = calibration->getGroupMembers(name);
        for( std::set<string>::const_iterator it = members.begin(); it != members.end(); ++it)
            *out << "\t\t<groupmember name=\"" + QString::fromStdString(*it) + "\"/>\n";

        *out << "\t</group>\n";
    }
    return true;
}

bool Persistence::saveEnabledObjectiveFunctionParameters(QTextStream *out)
{
    set<string> enabledparameters = calibration->evalObjectiveFunctionParameters();
    for( std::set<string>::const_iterator it = enabledparameters.begin(); it != enabledparameters.end(); ++it)
        *out << "\t<enabledobjectivefunction name=\"" + QString::fromStdString(*it) + "\"/>\n";

    return true;
}

bool Persistence::saveModelSimulator(QTextStream *out)
{
    QString par = "\t<modelsimulatoralgorithm>\n";
    if(!saveFunction(QString::fromStdString(calibration->getModelSimulator()),calibration->getModelSimulatorSettings(),&par))
        return false;
    par += "\t</modelsimulatoralgorithm>\n";
    *out << par;
    return true;
}

bool Persistence::saveCalibrationAlgorithm(QTextStream *out)
{
    QString par = "\t<calibrationalgorithm>\n";
    if(!saveFunction(QString::fromStdString(calibration->getCalibrationAlg()),calibration->getCalibrationAlgSettings(),&par))
        return false;
    par += "\t</calibrationalgorithm>\n";
    *out << par;
    return true;
}

bool Persistence::loadConnections()
{
    QDomNodeList parameters = root.elementsByTagName("connection");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement connection = parameters.at(index).toElement();
        ObjectiveFunctionVariable *tmpvar = static_cast<ObjectiveFunctionVariable*>(calibration->getDomain()->getPar(connection.attribute("destination").toStdString()));
        if(!tmpvar->addParameter(connection.attribute("source").toStdString()))
            return false;
    }
    return true;
}

bool Persistence::saveFunction(QString functionname, map<std::string, std::string> parameters, QString *string)
{
    QString func = "\t\t<function function=\"" + functionname + "\">\n";

    std::pair<std::string, std::string>p;
    BOOST_FOREACH(p,parameters)
    {
        func += "\t\t\t<functionparameter key=\"" + QString::fromStdString(p.first) +
                "\" value=\"" + QString::fromStdString(p.second) +
                "\"/>\n";
    }
    func += "\t\t</function>\n";
    (*string) += func;
    return true;
}

bool Persistence::saveIterationResults(QTextStream *out)
{
    vector<IterationResult * > results = calibration->getIterationResults();

    BOOST_FOREACH(IterationResult * result,results)
    {
        QString par = "\t<iterationresult iterationnumber=\"" + QString::number(result->getIterationNumber()) +
                      "\">\n";

        *out << par;
        vector<Variable*> parameters;

        //calibration parameters
        parameters = calibration->getDomain()->getAllPars(CALIBRATIONVARIABLE);
        BOOST_FOREACH(Variable *var, parameters)
        {
            *out << "\t\t<calibrationparameterresult values=\"" + QString::fromStdString(vectorToString(result->getCalibrationParameterResults(var->getName()))) +
                   "\" name=\"" + QString::fromStdString(var->getName()) +
                   "\"/>\n";
        }

        //iteration parameters
        parameters = calibration->getDomain()->getAllPars(ITERATIONVARIABLE);
        BOOST_FOREACH(Variable *var, parameters)
        {
            *out << "\t\t<iterationparameterresult values=\"" + QString::fromStdString(vectorToString(result->getIterationParameterResults(var->getName()))) +
                   "\" name=\"" + QString::fromStdString(var->getName()) +
                   "\"/>\n";
        }

        //observed parameter
        parameters = calibration->getDomain()->getAllPars(OBSERVEDVARIABLE);
        BOOST_FOREACH(Variable *var, parameters)
        {
            *out << "\t\t<observedparameterresult values=\"" + QString::fromStdString(vectorToString(result->getObservedParameterResults(var->getName()))) +
                   "\" name=\"" + QString::fromStdString(var->getName()) +
                   "\"/>\n";
        }

        //objective function parameters
        parameters = calibration->getDomain()->getAllPars(OBJECTIVEFUNCTIONVARIABLE);
        BOOST_FOREACH(Variable *var, parameters)
        {
            *out << "\t\t<objectivefunctionparameterresult values=\"" + QString::fromStdString(vectorToString(result->getObjectiveFunctionParameterResults(var->getName()))) +
                   "\" name=\"" + QString::fromStdString(var->getName()) +
                   "\"/>\n";
        }

        par = "\t</iterationresult>\n";
        *out << par;
    }
    return true;
}

bool Persistence::loadTemplates()
{
    ExternalParameterRegistry* reg = calibration->getExternalParameterRegistry();

    QDomNodeList templates;

    //calibrationvariables
    templates = root.elementsByTagName("calibrationparametertemplate");

    for(int index=0; index<templates.size(); index++)
    {
        QDomElement currenttemplate = templates.at(index).toElement();
        QDomNodeList members = currenttemplate.elementsByTagName("templatemember");
        QDomNodeList templatestrings = currenttemplate.elementsByTagName("templatestring");
        if(templatestrings.size() != 1)
            return false;

        string name = currenttemplate.attribute("name").toStdString();
        string path = currenttemplate.attribute("path").toStdString();
        string templatestring = templatestrings.at(0).toElement().childNodes().at(0).toCDATASection().nodeValue().toStdString();
        if(!reg->registerTemplate(name,path,templatestring,calibration,CALIBRATIONVARIABLE))
            return false;

        for(int i=0; i < members.size(); i++)
            reg->registerParameter(members.at(i).toElement().attribute("name").toStdString(),name,calibration->getDomain());
    }

    //observedvariables
    templates = root.elementsByTagName("observedparametertemplate");

    for(int index=0; index<templates.size(); index++)
    {
        QDomElement currenttemplate = templates.at(index).toElement();
        QDomNodeList members = currenttemplate.elementsByTagName("templatemember");
        QDomNodeList templatestrings = currenttemplate.elementsByTagName("templatestring");
        if(templatestrings.size() != 1)
            return false;

        string name = currenttemplate.attribute("name").toStdString();
        string path = currenttemplate.attribute("path").toStdString();
        string templatestring = templatestrings.at(0).toElement().childNodes().at(0).toCDATASection().nodeValue().toStdString();
        if(!reg->registerTemplate(name,path,templatestring,calibration,OBSERVEDVARIABLE))
            return false;

        for(int i=0; i < members.size(); i++)
            reg->registerParameter(members.at(i).toElement().attribute("name").toStdString(),name,calibration->getDomain());
    }

    //iterationvariables
    templates = root.elementsByTagName("iterationparametertemplate");

    for(int index=0; index<templates.size(); index++)
    {
        QDomElement currenttemplate = templates.at(index).toElement();
        QDomNodeList members = currenttemplate.elementsByTagName("templatemember");
        QDomNodeList templatestrings = currenttemplate.elementsByTagName("templatestring");
        if(templatestrings.size() != 1)
            return false;

        string name = currenttemplate.attribute("name").toStdString();
        string path = currenttemplate.attribute("path").toStdString();
        string templatestring = templatestrings.at(0).toElement().childNodes().at(0).toCDATASection().nodeValue().toStdString();
        if(!reg->registerTemplate(name,path,templatestring,calibration,ITERATIONVARIABLE))
            return false;

        for(int i=0; i < members.size(); i++)
            reg->registerParameter(members.at(i).toElement().attribute("name").toStdString(),name,calibration->getDomain());
    }
    return true;
}

bool Persistence::saveTemplates(QTextStream *out)
{    
    ExternalParameterRegistry* reg = calibration->getExternalParameterRegistry();
    vector<string> names;
    vector<Variable*> vars;

    //calibrationvariables
    names = reg->getTemplateNames(CALIBRATIONVARIABLE);
    vars = calibration->getDomain()->getAllPars(CALIBRATIONVARIABLE);
    BOOST_FOREACH(string name, names)
    {
        QString par = "\t<calibrationparametertemplate path=\"" + QString::fromStdString(reg->getPath(name)) +
                      "\" name=\"" + QString::fromStdString(name) +
                      "\">\n" +
                      "\t\t<templatestring>\n" +
                      "\t\t\t<![CDATA[" + QString::fromStdString(reg->getTemplate(name)) +
                      "]]>\n" +
                      "\t\t</templatestring>\n";

        *out << par;

        BOOST_FOREACH(Variable *var, vars)
                if(reg->containsParameter(var->getName(),name))
                    *out << "\t\t<templatemember name=\"" + QString::fromStdString(var->getName()) + "\"/>\n";

          *out << "\t</calibrationparametertemplate>\n";
    }

    //iterationvariables
    names = reg->getTemplateNames(ITERATIONVARIABLE);
    vars = calibration->getDomain()->getAllPars(ITERATIONVARIABLE);
    BOOST_FOREACH(string name, names)
    {
        QString par = "\t<iterationparametertemplate path=\"" + QString::fromStdString(reg->getPath(name)) +
                      "\" name=\"" + QString::fromStdString(name) +
                      "\">\n" +
                      "\t\t<templatestring>\n" +
                      "\t\t\t<![CDATA[" + QString::fromStdString(reg->getTemplate(name)) +
                      "]]>\n" +
                      "\t\t</templatestring>\n";

        *out << par;

        BOOST_FOREACH(Variable *var, vars)
                if(reg->containsParameter(var->getName(),name))
                    *out << "\t\t<templatemember name=\"" + QString::fromStdString(var->getName()) + "\"/>\n";

          *out << "\t</iterationparametertemplate>\n";
    }

    //observedvariables
    names = reg->getTemplateNames(OBSERVEDVARIABLE);
    vars = calibration->getDomain()->getAllPars(OBSERVEDVARIABLE);
    BOOST_FOREACH(string name, names)
    {
        QString par = "\t<observedparametertemplate path=\"" + QString::fromStdString(reg->getPath(name)) +
                      "\" name=\"" + QString::fromStdString(name) +
                      "\">\n" +
                      "\t\t<templatestring>\n" +
                      "\t\t\t<![CDATA[" + QString::fromStdString(reg->getTemplate(name)) +
                      "]]>\n" +
                      "\t\t</templatestring>\n";

        *out << par;

        BOOST_FOREACH(Variable *var, vars)
                if(reg->containsParameter(var->getName(),name))
                    *out << "\t\t<templatemember name=\"" + QString::fromStdString(var->getName()) + "\"/>\n";

          *out << "\t</observedparametertemplate>\n";
    }
    return true;
}

bool Persistence::loadIterationResults()
{
    QDomNodeList iterationresults = root.elementsByTagName("iterationresult");
    map<int,IterationResult *  > resultmap;

    for(int index=0; index<iterationresults.size(); index++)
    {
        QDomElement currentresult = iterationresults.at(index).toElement();
        int iterationnumber = currentresult.attribute("iterationnumber").toDouble();
        map<string, vector<double> > calibrationresults, iterationresult, observedresults, objectivefunctionresults;

        //calibration results
        QDomNodeList list = currentresult.elementsByTagName("calibrationparameterresult");
        for(int i=0; i < list.size(); i++)
        {
            QDomElement currentelement = list.at(i).toElement();
            calibrationresults[currentelement.attribute("name").toStdString()] = stringToVector(currentelement.attribute("values").toStdString());
        }

        //iterationresult results
        list = currentresult.elementsByTagName("iterationparameterresult");
        for(int i=0; i < list.size(); i++)
        {
            QDomElement currentelement = list.at(i).toElement();
            iterationresult[currentelement.attribute("name").toStdString()] = stringToVector(currentelement.attribute("values").toStdString());
        }

        //observed results
        list = currentresult.elementsByTagName("observedparameterresult");
        for(int i=0; i < list.size(); i++)
        {
            QDomElement currentelement = list.at(i).toElement();
            observedresults[currentelement.attribute("name").toStdString()] = stringToVector(currentelement.attribute("values").toStdString());
        }

        //objectivefunctionresults results
        list = currentresult.elementsByTagName("objectivefunctionparameterresult");
        for(int i=0; i < list.size(); i++)
        {
            QDomElement currentelement = list.at(i).toElement();
            objectivefunctionresults[currentelement.attribute("name").toStdString()] = stringToVector(currentelement.attribute("values").toStdString());
        }

        resultmap[iterationnumber] = new IterationResult(iterationnumber,calibrationresults,iterationresult, objectivefunctionresults, observedresults);
    }

    return calibration->setIterationResults(resultmap);
}

bool Persistence::loadFunction(QString &functionname, map<std::string,std::string> &functionparameters, QDomElement *element )
{
    QDomNodeList functions = element->elementsByTagName("function");

    if(functions.size()!=1)
        return false;

    QDomElement function = functions.at(0).toElement();
    QDomNodeList functionparameterslist = function.elementsByTagName("functionparameter");
    functionname=function.attribute("function");

    for(int index=0; index<functionparameterslist.size(); index++)
    {
        QDomElement functionparameter = functionparameterslist.at(index).toElement();
        functionparameters[functionparameter.attribute("key").toStdString()]=functionparameter.attribute("value").toStdString();
    }

    return true;
}

bool Persistence::loadCalibrationParameters()
{
    QDomNodeList parameters = root.elementsByTagName("calibrationparameter");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement parameter = parameters.at(index).toElement();
        CalibrationVariable calibrationparameter(parameter.attribute("name").toStdString(), vector<double>());
        calibrationparameter.setStep(parameter.attribute("step").toDouble());
        calibrationparameter.setMax(parameter.attribute("max").toDouble());
        calibrationparameter.setMin(parameter.attribute("min").toDouble());
        calibrationparameter.setInitValues(stringToVector(parameter.attribute("initvalue").toStdString()));
        calibrationparameter.setValues(stringToVector(parameter.attribute("value").toStdString()));
        if(!calibration->addParameter(&calibrationparameter))
            return false;
    }
    return true;
}

bool Persistence::loadIterationParameters()
{
    QDomNodeList parameters = root.elementsByTagName("iterationparameter");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement parameter = parameters.at(index).toElement();
        Variable newparameter(parameter.attribute("name").toStdString(), stringToVector(parameter.attribute("value").toStdString()), ITERATIONVARIABLE);

        if(!calibration->addParameter(&newparameter))
            return false;
    }
    return true;
}

bool Persistence::loadObservedParameters()
{
    QDomNodeList parameters = root.elementsByTagName("observedparameter");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement parameter = parameters.at(index).toElement();
        Variable newparameter(parameter.attribute("name").toStdString(), stringToVector(parameter.attribute("value").toStdString()), OBSERVEDVARIABLE);

        if(!calibration->addParameter(&newparameter))
            return false;
    }
    return true;
}

bool Persistence::loadObjectiveFunctionParameters()
{
    QDomNodeList parameters = root.elementsByTagName("objectivefunctionparameter");

    for(int index=0; index<parameters.size(); index++)
    {
        QDomElement parameter = parameters.at(index).toElement();
        ObjectiveFunctionVariable newparameter(parameter.attribute("name").toStdString());

        if(!calibration->addParameter(&newparameter))
            return false;

        ObjectiveFunctionVariable* regparameter = static_cast<ObjectiveFunctionVariable*>(calibration->getDomain()->getPar(newparameter.getName()));
        QString functionname;
        map<string, string> functionparameters;

        if(!loadFunction(functionname,functionparameters,&parameter))
            return false;

        if(functionname!="")
            if(!regparameter->setObjectiveFunction(functionname.toStdString(),functionparameters))
                return false;
    }
    return true;
}

bool Persistence::saveCalibration(QString filename)
{
    //saving file
    QFile file(filename);
    if(!file.open( QIODevice::WriteOnly  ))
    {
        file.close();
        return false;
    }

    QTextStream out( &file );

    out << "<Calimero_v1.10.05>\n";
    if(!Persistence::saveCalibrationParameters(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveObservedParameters(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveIterationParameters(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveObjectiveFunctionParameters(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveCalibrationAlgorithm(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveModelSimulator(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveEnabledObjectiveFunctionParameters(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveGroups(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveTemplates(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveResultHandler(&out))
    {
        file.close();
        return false;
    }
    if(!Persistence::saveIterationResults(&out))
    {
        file.close();
        return false;
    }

    out << "</Calimero_v1.10.05>\n";
    file.close();
    return true;
}

string Persistence::vectorToString(vector<double> vec)
{
    string result="";

    for(uint counter=0; counter<vec.size(); counter++)
    {
        result+= QString::number(vec[counter]).toStdString();

        if(counter != vec.size()-1)
            result += string(" ");
    }
    return result;
}

vector<double> Persistence::stringToVector(string string, bool *ok)
{
    if(ok!=NULL)
        *ok=true;

    vector<double> vec;
    QStringList values = QString::fromStdString(string).trimmed().split(" ");

    for(int counter=0; counter<values.size(); counter++)
    {
        bool currentok = true;

        vec.push_back(values[counter].toDouble(&currentok));

        if(!currentok && ok!=NULL)
        {
            *ok=false;
        }
    }

    return vec;
}

bool Persistence::loadCalibration(QString filename)
{
    bool error = false;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        return false;
    }

    calibration->clear();
    doc->clear();
    root.clear();

    doc->setContent(&file);

    root = doc->documentElement();

    error  = root.tagName() != CALIMERO_VERSION;

    if(!error)
        error = error || !loadCalibrationParameters();

    if(!error)
        error = error || !loadIterationParameters();

    if(!error)
        error = error || !loadObservedParameters();

    if(!error)
        error = error || !loadObjectiveFunctionParameters();

    if(!error)
        error = error || !loadConnections();

    if(!error)
        error = error || !loadCalibrationAlgorithm();

    if(!error)
        error = error || !loadModelSimulator();

    if(!error)
        error = error || !loadEnabledObjectiveFunctionParameters();

    if(!error)
        error = error || !loadGroups();

    if(!error)
        error = error || !loadTemplates();

    if(!error)
        error = error || !loadResultHandler();

    if(!error)
        error = error || !loadIterationResults();

    if(error)
    {
        doc->clear();
        root.clear();
        calibration->clear();
    }

    file.close();
    return !error;
}
