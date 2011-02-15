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
#include <QStatusBar>
#include <QXmlInputSource>
#include <QXmlContentHandler>

using namespace boost;
using namespace std;

CalimeroXmlHandler::CalimeroXmlHandler(Calibration *calibration,QStatusBar *status)
{
    this->calibration=calibration;
    this->status=status;
}

bool CalimeroXmlHandler::startDocument()
{
    connections.clear();
    objectiveparameters.clear();
    calibrationparameters.clear();
    settings.clear();
    loadediterations=0;
    return true;
}

bool CalimeroXmlHandler::characters(const QString &ch)
{
    templatestring += ch.toStdString();
    return true;
}

bool CalimeroXmlHandler::endDocument()
{
    for(uint index = 0; index < connections.size(); index++)
    {
        ObjectiveFunctionVariable *tmpvar = static_cast<ObjectiveFunctionVariable*>(calibration->getDomain()->getPar(connections[index].second));
        if(!tmpvar->addParameter(connections[index].first))
            return false;
    }

    if(status)
    {
        status->showMessage("");
        QCoreApplication::processEvents();
    }

    return calibration->setIterationResults(results);
}

bool CalimeroXmlHandler::endElement( const QString&, const QString&, const QString &name )
{
    if(name == "iterationresult")
    {
        results[iterationnr] = new IterationResult(iterationnr,calibrationparameters,objectiveparameters);
        calibrationparameters.clear();
        objectiveparameters.clear();

        if(status)
        {
            status->showMessage("Loaded iteration-results: " + QString::number(++loadediterations));
            QCoreApplication::processEvents();
        }
    }

    if(name == "objectivefunctionparameter")
    {
        ObjectiveFunctionVariable* regparameter = static_cast<ObjectiveFunctionVariable*>(calibration->getDomain()->getPar(currentname));

        if(function!="")
            if(!regparameter->setObjectiveFunction(function,settings))
                return false;
    }

    if(name == "calibrationalgorithm")
    {
        if(function!="")
            if(!calibration->setCalibrationAlg(function,settings))
                return false;
    }

    if(name == "modelsimulatoralgorithm")
    {
        if(function!="")
            if(!calibration->setModelSimulator(function,settings))
                return false;
    }

    if(name == "resulthandler")
    {
        if(function!="")
            if(!calibration->addResultHandler(resulthandlername,function,settings,reshandlerenabled))
                return false;
    }

    if(name == "observedparametertemplate")
    {
        if(!calibration->getExternalParameterRegistry()->registerTemplate(templatename,templatepath,QString::fromStdString(templatestring).replace("calimero///","]]>").toStdString(),calibration,OBSERVEDVARIABLE))
            return false;

        templatename="";
        templatestring="";
        templatepath="";
    }

    if(name == "iterationparametertemplate")
    {
        if(!calibration->getExternalParameterRegistry()->registerTemplate(templatename,templatepath,QString::fromStdString(templatestring).replace("calimero///","]]>").toStdString(),calibration,ITERATIONVARIABLE))
            return false;

        templatename="";
        templatestring="";
        templatepath="";
    }

    if(name == "calibrationparametertemplate")
    {
        if(!calibration->getExternalParameterRegistry()->registerTemplate(templatename,templatepath,QString::fromStdString(templatestring).replace("calimero///","]]>").toStdString(),calibration,CALIBRATIONVARIABLE))
            return false;

        templatename="";
        templatestring="";
        templatepath="";
    }

    return true;
}

bool CalimeroXmlHandler::startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs )
{
    if(name == "calibrationparameter")
        if(!loadCalibrationParameters(attrs))
            return false;

    if(name == "iterationparameter")
        if(!loadIterationParameters(attrs))
            return false;

    if(name == "objectivefunctionparameter")
        if(!loadObjectiveFunctionParameters(attrs))
            return false;

    if(name == "observedparameter")
        if(!loadObservedParameters(attrs))
            return false;

    if(!loadIterationResults(name,attrs))
        return false;

    if(name == "function")
    {
        settings.clear();
        function = attrs.value("function").toStdString();
    }

    if(name == "functionparameter")
        settings[attrs.value("key").toStdString()]=attrs.value("value").toStdString();

    if(name == "connection")
        connections.push_back(std::pair<string,string>(attrs.value("source").toStdString(),attrs.value("destination").toStdString()));

    if(name == "enabledobjectivefunction")
    {
        if(!calibration->addEnabledOParameter(attrs.value("name").toStdString()))
                return false;
    }

    if(name == "resulthandler")
    {
        resulthandlername = attrs.value("name").toStdString();
        reshandlerenabled = attrs.value("enabled").toInt();
    }

    if(name ==  "group")
    {
        currentgroup = attrs.value("name").toStdString();
        calibration->addGroup(currentgroup);

        if(attrs.value("enabledgroup").toDouble())
            calibration->addEnabledGroup(currentgroup);
        else
            calibration->removeEnabledGroup(currentgroup);

        if(attrs.value("disabledgroup").toDouble())
            calibration->addDisabledGroup(currentgroup);
        else
            calibration->removeDisabledGroup(currentgroup);
    }

    if(name == "groupmember")
        if(!calibration->addParameterToGroup(attrs.value("name").toStdString(),currentgroup))
            return false;

    if(name == "observedparametertemplate")
    {
        templatename=attrs.value("name").toStdString();
        templatepath=attrs.value("path").toStdString();
        templatestring="";
    }

    if(name == "iterationparametertemplate")
    {
        templatename=attrs.value("name").toStdString();
        templatepath=attrs.value("path").toStdString();
        templatestring="";
    }

    if(name == "calibrationparametertemplate")
    {
        templatename=attrs.value("name").toStdString();
        templatepath=attrs.value("path").toStdString();
        templatestring="";
    }

    if(name == "templatestring")
        templatestring="";

    return true;
}

bool CalimeroXmlHandler::loadObjectiveFunctionParameters(const QXmlAttributes &attrs )
{
    std::string name = attrs.value("name").toStdString();

    ObjectiveFunctionVariable newparameter(name);
    currentname = name;

    if(!calibration->addParameter(&newparameter))
        return false;

    return true;
}

bool CalimeroXmlHandler::loadIterationResults(const QString &name, const QXmlAttributes &attrs )
{
    if(name == "iterationresult")
        iterationnr = attrs.value("iterationnumber").toInt();

    if(name == "calibrationparameterresult")
        calibrationparameters[attrs.value("name").toStdString()]=Persistence::stringToVector(attrs.value("values").toStdString());

    if(name == "objectivefunctionparameterresult")
        objectiveparameters[attrs.value("name").toStdString()]=Persistence::stringToVector(attrs.value("values").toStdString());

    return true;
}

bool CalimeroXmlHandler::loadCalibrationParameters(const QXmlAttributes &attrs)
{
    std::string name;
    double step=0, max=0, min=0;
    std::vector<double> init, values;

    for(int index=0; index<attrs.count(); index++)
    {
        if(attrs.localName(index)=="name")
            name = attrs.value(index).toStdString();

        if(attrs.localName(index)=="step")
            step = attrs.value(index).toDouble();

        if(attrs.localName(index)=="max")
            max = attrs.value(index).toDouble();

        if(attrs.localName(index)=="min")
            min = attrs.value(index).toDouble();

        if(attrs.localName(index)=="initvalue")
            init = Persistence::stringToVector(attrs.value(index).toStdString());

        if(attrs.localName(index)=="value")
            values = Persistence::stringToVector(attrs.value(index).toStdString());
    }

    CalibrationVariable calibrationparameter(name, vector<double>());
    calibrationparameter.setStep(step);
    calibrationparameter.setMax(max);
    calibrationparameter.setMin(min);
    calibrationparameter.setInitValues(init);
    calibrationparameter.setValues(values);
    if(!calibration->addParameter(&calibrationparameter))
        return false;

    return true;
}

bool CalimeroXmlHandler::loadIterationParameters(const QXmlAttributes &attrs)
{
    std::string name;
    std::vector<double> values;

    for(int index=0; index<attrs.count(); index++)
    {
        if(attrs.localName(index)=="name")
            name = attrs.value(index).toStdString();

        if(attrs.localName(index)=="value")
            values = Persistence::stringToVector(attrs.value(index).toStdString());
    }

    Variable newparameter(name, values, ITERATIONVARIABLE);

    if(!calibration->addParameter(&newparameter))
        return false;

    return true;
}

bool CalimeroXmlHandler::loadObservedParameters(const QXmlAttributes &attrs)
{
    std::string name;
    std::vector<double> values;

    for(int index=0; index<attrs.count(); index++)
    {
        if(attrs.localName(index)=="name")
            name = attrs.value(index).toStdString();

        if(attrs.localName(index)=="value")
            values = Persistence::stringToVector(attrs.value(index).toStdString());
    }

    Variable newparameter(name, values, OBSERVEDVARIABLE);

    if(!calibration->addParameter(&newparameter))
        return false;

    return true;
}

bool CalimeroXmlHandler::fatalError ( const QXmlParseException & exception )
{
    Logger(Error) << "Cannot load calimero project";
    Logger(Error) << "Calimero cmp file: Line: " << exception.lineNumber() << " Column: " << exception.columnNumber() << " <" << exception.message() << ">";
    return false;
}

Persistence::Persistence(Calibration *calibration)
{
    this->calibration=calibration;
}

Persistence::~Persistence()
{
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
        if(status)
        {
            status->showMessage("Saved iteration-results: " + QString::number(result->getIterationNumber()+1) + "/" + QString::number(results.size()));
            QCoreApplication::processEvents();
        }

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
    if(status)
    {
        status->showMessage("");
        QCoreApplication::processEvents();
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
                      "<templatestring>" +
                      "<![CDATA[" + QString::fromStdString(reg->getTemplate(name)).replace("]]>","calimero///") +
                      "]]>" +
                      "</templatestring>\n";

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
                      "<templatestring>" +
                      "<![CDATA[" + QString::fromStdString(reg->getTemplate(name)) +
                      "]]>" +
                      "</templatestring>\n";

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
                      "<templatestring>" +
                      "<![CDATA[" + QString::fromStdString(reg->getTemplate(name)) +
                      "]]>" +
                      "</templatestring>\n";

        *out << par;

        BOOST_FOREACH(Variable *var, vars)
                if(reg->containsParameter(var->getName(),name))
                    *out << "\t\t<templatemember name=\"" + QString::fromStdString(var->getName()) + "\"/>\n";

          *out << "\t</observedparametertemplate>\n";
    }
    return true;
}

bool Persistence::saveCalibration(QString filename, QStatusBar *status)
{
    this->status=status;
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

bool Persistence::loadCalibration(QString filename,QStatusBar *status)
{
    bool noerror = true;
    calibration->clear();
    CalimeroXmlHandler handler(calibration,status);
    QFile file(filename);
    QXmlInputSource source(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
    noerror = reader.parse(source);
    file.close();
    return noerror;
}
