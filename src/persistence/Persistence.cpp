#include <QtXml>
#include <QtCore>
#include "Persistence.h"
#include "../core/Model.h"
#include "../core/CalculationVariable.h"
#include "../core/CalibrationVariable.h"
#include "../core/Variable.h"
#include "../core/IterationResult.h"
#include "../qtscript/ScriptLibary.h"
#include <iostream>
#include "../gui/ProgressBar.h"
#include <qdom.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <qtextstream.h>


using namespace std;

bool Persistence::loadFileAsString(QString filename, QString *file)
{
    if(!QFile::exists(filename))
        return false;

    QFile inputfile(filename);

    if (!inputfile.open(QIODevice::ReadOnly))
    {
         return false;
    }

    QTextStream inputstream(&inputfile);
    *file+=inputstream.readAll();
    inputfile.close();
    return true;
}

bool Persistence::saveStringAsFile(QString filename, QString text)
{
    QFile outputfile(filename);

    if (!outputfile.open(QIODevice::WriteOnly))
    {
         return false;
    }

    QTextStream outputstream(&outputfile);

    outputstream << text;

    outputfile.close();
    return true;
}

bool Persistence::saveModel(QString filename, Model* model)
{
    qDebug() << "Persistence::saveModel()";
    QDomDocument doc;
    QDomElement root = doc.createElement("Calimero");
    doc.appendChild(root);

    if(!Persistence::saveModelParameters(root, &doc, model))
        return false;
    if(!Persistence::saveCalibrationScript(root, &doc, model))
        return false;
    if(!Persistence::saveInputParameters(root, &doc, model))
        return false;
    if(!Persistence::saveResultParameters(root, &doc, model))
        return false;
    if(!Persistence::saveIterationParameters(root, &doc, model))
        return false;
    if(!Persistence::saveCompareParameters(root, &doc, model))
        return false;
    if(!Persistence::saveGroups(root, &doc, model))
        return false;
    if(!Persistence::saveIterationResults(root, &doc, model))
        return false;

    //saving file
    QFile file(filename);
    if(!file.open( QIODevice::WriteOnly  ))
    {
        file.close();
        return false;
    }

    QTextStream out( &file );
    doc.save(out,1);
    qDebug() << "Persistence::saveModel() Complete";
    return true;
}

int Persistence::loadModel(QString filename, Model* model, bool forcescriptupdate)
{
    qDebug() << "Persistence::loadModel()";
    QFile file(filename);
    int error = 1;

    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        model->clear();
        return false;
    }


    QDomDocument *doc = new QDomDocument();
    doc->setContent(&file);

    QDomElement docElem = doc->documentElement();

    if(docElem.tagName() == "Calimero")
    {
        qDebug() << "Persistence::loadModel() STAGE 1";
        model->clear();
        if(!Persistence::loadInputParameters(doc,model,forcescriptupdate))
        {
            delete doc;
            file.close();
            model->clear();
            return false;
        }
        qDebug() << "Persistence::loadModel() STAGE 2";
        if(!Persistence::loadIterationParameters(doc,model,forcescriptupdate))
        {
            delete doc;
            file.close();
            model->clear();
            return false;
        }
        qDebug() << "Persistence::loadModel() STAGE 3";
        if(!Persistence::loadResultParameters(doc,model,forcescriptupdate))
        {
            delete doc;
            file.close();
            model->clear();
            return false;
        }
        qDebug() << "Persistence::loadModel() STAGE 4";
        error = Persistence::loadCompareParameters(doc,model,forcescriptupdate);
        if(error<1)
        {
            delete doc;
            file.close();
            model->clear();
            return error;
        }
        qDebug() << "Persistence::loadModel() STAGE 5";
        error = Persistence::loadCalibrationScript(doc,model,forcescriptupdate);
        if(error<1)
        {
            delete doc;
            file.close();
            model->clear();
            return error;
        }
        qDebug() << "Persistence::loadModel() STAGE 6";
        if(!Persistence::loadModelParameters(doc,model,forcescriptupdate))
        {
            delete doc;
            file.close();
            model->clear();
            return error;
        }
        qDebug() << "Persistence::loadModel() STAGE 7";
        if(!Persistence::loadIterationResults(doc,model,forcescriptupdate))
        {
            delete doc;
            file.close();
            model->clear();
            return false;
        }
        qDebug() << "Persistence::loadModel() STAGE 8";
        if(!Persistence::loadGroups(doc,model,forcescriptupdate))
        {
            delete doc;
            file.close();
            model->clear();
            return false;
        }
    }
    else
    {
        delete doc;
        file.close();
        model->clear();
        return false;
    }

    delete doc;
    file.close();
    qDebug() << "Persistence::loadModel() DONE";
    return true;
}

int Persistence::loadCompareParameters(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    qDebug() << "Persistence::loadCompareParameters()";
    QDomNodeList varlist = doc->elementsByTagName("parameter:compare");

    for(int counter=0; counter<varlist.size(); counter++)
    {
        QDomElement element = varlist.at(counter).toElement();
        CalculationVariable *tmpvar = new CalculationVariable(element.attribute("name"));

        QDomNodeList scriptsectionlist = element.elementsByTagName("comparealg");

        if(scriptsectionlist.size()!=1)
            return false;

        QDomElement scriptsection = scriptsectionlist.at(0).toElement();

        if(element.attribute("script")!=scriptsection.attribute("algname"))
            return false;

        QDomNodeList scriptlist = scriptsection.elementsByTagName("scriptfile");

        if(!ScriptLibary::getInstance()->existsCompareScript(element.attribute("script")))
        {
            qDebug() << "Persistence::loadCompareParameters() Import new script files: " << element.attribute("script");
            ScriptLibary::getInstance()->saveCompareScript(element.attribute("script"));

            for(int index=0; index<scriptlist.size(); index++)
            {
                QDomElement script = scriptlist.at(index).toElement();
                QDomNodeList codelist = script.childNodes();

                if(codelist.size()!=1)
                    return 0;

                QDomCDATASection code = codelist.at(0).toCDATASection();
                if(!Persistence::saveStringAsFile(ScriptLibary::getInstance()->getCompareScriptPath() + element.attribute("script") + "/" + script.attribute("filename"),code.nodeValue()))
                {
                    qWarning() << "Persistence::loadCompareParameters() Error: code not open Scriptfile";
                    return false;
                }
            }
        }
        else
        {
            //diff check

            QStringList filelist;

            for(int index=0; index<scriptlist.size(); index++)
            {
                QDomElement script = scriptlist.at(index).toElement();
                QDomNodeList codelist = script.childNodes();

                if(codelist.size()!=1)
                    return 0;

                QDomCDATASection code = codelist.at(0).toCDATASection();
                QString currentcode = "";
                if(!Persistence::loadFileAsString(ScriptLibary::getInstance()->getCompareScriptPath() + element.attribute("script") + "/" + script.attribute("filename"),&currentcode) || currentcode!=code.nodeValue())
                {
                    qDebug() << "Persistence::loadCompareParameters() Script with same name already exist";

                    if(!forcescriptupdate)
                        return -1;

                    if(!Persistence::saveStringAsFile(ScriptLibary::getInstance()->getCompareScriptPath() + element.attribute("script") + "/" + script.attribute("filename"),code.nodeValue()))
                    {
                        qWarning() << "Persistence::loadCompareParameters Error: Not able to save script file";
                    }
                }
                filelist.append(script.attribute("filename"));
            }

            QDir dir(ScriptLibary::getInstance()->getCompareScriptPath() + element.attribute("script"));
            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            QFileInfoList list = dir.entryInfoList();

            for (int i = 0; i < list.size(); ++i)
            {
                QFileInfo fileInfo = list.at(i);
                QString filename = fileInfo.fileName();

                if(!filelist.contains(filename))
                {
                    if(!QFile::remove(ScriptLibary::getInstance()->getCompareScriptPath() + element.attribute("script") + "/" + filename))
                        return -2;
                }
            }
        }

        if(!ScriptLibary::getInstance()->existsCompareScript(element.attribute("script")))
            return false;

        tmpvar->setAlg(element.attribute("script"));
        model->addCompareParameter(tmpvar);

        if(element.attribute("alg").toInt())
            model->addCalculationVariableToCalibration(tmpvar->getName());
    }

    QDomNodeList compareconnectionlist = doc->elementsByTagName("parameter:connection");

    for(int counter=0; counter<compareconnectionlist.size(); counter++)
    {
        QDomElement element = compareconnectionlist.at(counter).toElement();
        if(!model->existsParameter(element.attribute("destination")))
            return false;
        CalculationVariable *tmpvar = static_cast<CalculationVariable*>(model->getParameter(element.attribute("destination")));
        if(!model->existsParameter(element.attribute("source")))
            return false;
        tmpvar->addCompareParameter(static_cast<CalculationVariable*>(model->getParameter(element.attribute("source"))));
    }

    QDomNodeList iterationconnectionlist = doc->elementsByTagName("parameter:inputiteration");

    for(int counter=0; counter<iterationconnectionlist.size(); counter++)
    {
        QDomElement element = iterationconnectionlist.at(counter).toElement();
        if(!model->existsParameter(element.attribute("destination")))
            return false;
        CalculationVariable *tmpvar = static_cast<CalculationVariable*>(model->getParameter(element.attribute("destination")));
        if(!model->existsParameter(element.attribute("source")))
            return false;
        tmpvar->addIterationParameter(static_cast<CalculationVariable*>(model->getParameter(element.attribute("source"))));
    }

    QDomNodeList resultconnectionlist = doc->elementsByTagName("parameter:inputresult");

    for(int counter=0; counter<resultconnectionlist.size(); counter++)
    {
        QDomElement element = resultconnectionlist.at(counter).toElement();
        if(!model->existsParameter(element.attribute("destination")))
            return false;
        CalculationVariable *tmpvar = static_cast<CalculationVariable*>(model->getParameter(element.attribute("destination")));
        if(!model->existsParameter(element.attribute("source")))
            return false;
        tmpvar->addResultParameter(static_cast<CalculationVariable*>(model->getParameter(element.attribute("source"))));
    }

    return true;
}

int Persistence::loadInputParameters(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(forcescriptupdate);
    QDomNodeList varlist = doc->elementsByTagName("parameterset:input");

    for(int counter=0; counter<varlist.size(); counter++)
    {
        QDomElement element = varlist.at(counter).toElement();
        QDomNodeList templatelist = element.childNodes();

        int cdataindex=0;

        for(cdataindex=0; cdataindex<templatelist.size(); cdataindex++ )
        {
            if(templatelist.at(cdataindex).isCDATASection())
                break;
        }

        if(!templatelist.at(cdataindex).isCDATASection())
            return false;

        QString templatestring = templatelist.at(cdataindex).toCDATASection().nodeValue();


        model->addInputFileName(element.attribute("valuefilename"),templatestring);
        QDomNodeList parameters = element.elementsByTagName("parameter");
        for(int counter2=0; counter2<parameters.size(); counter2++)
        {

            QDomElement parameter = parameters.at(counter2).toElement();

            CalibrationVariable *modelvar = new CalibrationVariable(parameter.attribute("name"), stringToVector(parameter.attribute("initvalue")));
            modelvar->setInitValue(stringToVector(parameter.attribute("initvalue")));
            modelvar->setValues(stringToVector(parameter.attribute("value")));
            modelvar->setMax(parameter.attribute("max").toDouble());
            modelvar->setMin(parameter.attribute("min").toDouble());
            model->addInputParameter(modelvar,model->getInputTemplate(element.attribute("valuefilename")) , element.attribute("valuefilename"),false);
        }
    }
    return true;
}

int Persistence::loadIterationParameters(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(forcescriptupdate);
    QDomNodeList varlist = doc->elementsByTagName("parameterset:iteration");

    for(int counter=0; counter<varlist.size(); counter++)
    {
        QDomElement element = varlist.at(counter).toElement();
        QDomNodeList templatelist = element.childNodes();

        int cdataindex=0;

        for(cdataindex=0; cdataindex<templatelist.size(); cdataindex++ )
        {
            if(templatelist.at(cdataindex).isCDATASection())
                break;
        }

        if(!templatelist.at(cdataindex).isCDATASection())
            return false;

        QString templatestring = templatelist.at(cdataindex).toCDATASection().nodeValue();

        model->addIterationFileName(element.attribute("valuefilename"),templatestring);


        QDomNodeList parameters = element.elementsByTagName("parameter");

        for(int counter2=0; counter2<parameters.size(); counter2++)
        {
            QDomElement parameter = parameters.at(counter2).toElement();

            Variable *modelvar = new Variable(parameter.attribute("name"), stringToVector(parameter.attribute("value")),Variable::ITERATIONVARIABLE);
            model->addIterationParameter(modelvar,model->getIterationTemplate(element.attribute("valuefilename")) , element.attribute("valuefilename"),false);

        }
    }
    return true;
}

int Persistence::loadResultParameters(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(forcescriptupdate);
    QDomNodeList varlist = doc->elementsByTagName("parameterset:result");

    for(int counter=0; counter<varlist.size(); counter++)
    {
        QDomElement element = varlist.at(counter).toElement();
        QDomNodeList templatelist = element.childNodes();

        int cdataindex=0;

        for(cdataindex=0; cdataindex<templatelist.size(); cdataindex++ )
        {
            if(templatelist.at(cdataindex).isCDATASection())
                break;
        }

        if(!templatelist.at(cdataindex).isCDATASection())
            return false;

        QString templatestring = templatelist.at(cdataindex).toCDATASection().nodeValue();

        model->addResultFileName(element.attribute("valuefilename"),templatestring);


        QDomNodeList parameters = element.elementsByTagName("parameter");

        for(int counter2=0; counter2<parameters.size(); counter2++)
        {
            QDomElement parameter = parameters.at(counter2).toElement();
            Variable *modelvar = new Variable(parameter.attribute("name"), stringToVector(parameter.attribute("value")),Variable::RESULTVARIABLE);
            model->addResultParameter(modelvar,model->getResultTemplate(element.attribute("valuefilename")) , element.attribute("valuefilename"),false);
        }
    }

    return true;
}

int Persistence::loadGroups(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(forcescriptupdate);
    QDomNodeList groups = doc->elementsByTagName("group");

    for(int counter=0; counter<groups.size(); counter++)
    {
        QDomElement element = groups.at(counter).toElement();

        model->newGroup(element.attribute("name"));

        QDomNodeList parameters = element.elementsByTagName("parameter");

        for(int counter2=0; counter2<parameters.size(); counter2++)
        {
            QDomElement parameter = parameters.at(counter2).toElement();
            if(!model->existsParameter(parameter.attribute("name")))
                    return false;
            model->addParameterToGroup(element.attribute("name"), static_cast<CalibrationVariable*>(model->getParameter(parameter.attribute("name"))));
        }
    }

    QDomNodeList evaluating = doc->elementsByTagName("group:evaluating");

    for(int counter=0; counter<evaluating.size(); counter++)
    {
        QDomElement element = evaluating.at(counter).toElement();
        model->addEvaluatingGroup(element.attribute("name"));
    }

    QDomNodeList notevaluating = doc->elementsByTagName("group:notevaluating");

    for(int counter=0; counter<notevaluating.size(); counter++)
    {
        QDomElement element = notevaluating.at(counter).toElement();
        model->addNotEvaluatingGroup(element.attribute("name"));
    }


    return true;
}

bool Persistence::saveCompareParameters(QDomElement &root, QDomDocument *doc, Model *model)
{
    QVector<CalculationVariable*> algvars(model->getAlgParameters());
    //save compareparameterset
    for(int counter=0; counter<model->numberOfCompareParameters(); counter++)
    {
        CalculationVariable *tmpvar = model->getCompareParameter(counter);
        QDomElement var = doc->createElement("parameter:compare");
        root.appendChild(var);
        var.setAttribute("name", tmpvar->getName());
        var.setAttribute("value", vectorToString(tmpvar->getValues()));
        var.setAttribute("script", tmpvar->getAlg());
        var.setAttribute("alg", algvars.contains(tmpvar));

        //save script
        QDomElement alg = doc->createElement("comparealg");
        var.appendChild(alg);
        alg.setAttribute("algname",tmpvar->getAlg());

        if((ScriptLibary::getInstance()->getCompareScriptNames(ScriptLibary::getInstance()->getCompareScriptPath()))->contains(tmpvar->getAlg()))
        {
            QDir dir(ScriptLibary::getInstance()->getCompareScriptPath() + tmpvar->getAlg());
            dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
            QFileInfoList list = dir.entryInfoList();

            for (int i = 0; i < list.size(); ++i)
            {
                QFileInfo fileInfo = list.at(i);
                QString filename = fileInfo.fileName();

                if(filename.contains(QRegExp("qs$")))
                {
                    QString script = "";
                    if(!Persistence::loadFileAsString(ScriptLibary::getInstance()->getCompareScriptPath() + tmpvar->getAlg() + "/" + filename,&script))
                        return false;

                    QDomElement scriptelement = doc->createElement("scriptfile");
                    alg.appendChild(scriptelement);
                    scriptelement.setAttribute("filename",filename);
                    QDomCDATASection code  = doc->createCDATASection(filename);
                    scriptelement.appendChild(code);
                    code.setNodeValue(script);
                }
            }
        }
        else
        {
            return false;
        }
    }

    for(int counter=0; counter<model->numberOfCompareParameters(); counter++)
    {
        CalculationVariable *tmpvar = model->getCompareParameter(counter);

        for(int counter2=0; counter2<tmpvar->sizeOfCompareParameter(); counter2++)
        {
            QDomElement connection = doc->createElement("parameter:connection");
            connection.setAttribute("destination", tmpvar->getName());
            connection.setAttribute("source", tmpvar->atCompareParameter(counter2)->getName());
            root.appendChild(connection);
        }

        for(int counter2=0; counter2<tmpvar->sizeOfIterationParameter(); counter2++)
        {
            QDomElement connection = doc->createElement("parameter:inputiteration");
            connection.setAttribute("destination", tmpvar->getName());
            connection.setAttribute("source", tmpvar->atIterationParameter(counter2)->getName());
            root.appendChild(connection);
        }

        for(int counter2=0; counter2<tmpvar->sizeOfResultParameter(); counter2++)
        {
            QDomElement connection = doc->createElement("parameter:inputresult");
            connection.setAttribute("destination", tmpvar->getName());
            connection.setAttribute("source", tmpvar->atResultParameter(counter2)->getName());
            root.appendChild(connection);
        }
    }
    return true;
}

bool Persistence::saveInputParameters(QDomElement &root, QDomDocument *doc, Model *model)
{
    //save inputparameterset
    for(int counter=0; counter<model->numberOfInputFiles(); counter++)
    {
        //create head
        QDomElement parameterset = doc->createElement("parameterset:input");
        QDomCDATASection templateelement = doc->createCDATASection("template");

        root.appendChild(parameterset);
        parameterset.appendChild(templateelement);
        parameterset.setAttribute("valuefilename", model->getInputFileName(counter));
        templateelement.setNodeValue(model->getInputTemplate(model->getInputFileName(counter)));

        //add parameters
        for(int counter2=0; counter2<model->numberOfInputParameters(model->getInputFileName(counter)); counter2++)
        {
            CalibrationVariable *tmpvar = model->getInputParameter(counter2, model->getInputFileName(counter));
            QDomElement var = doc->createElement("parameter");
            parameterset.appendChild(var);
            var.setAttribute("name", tmpvar->getName());
            var.setAttribute("value", vectorToString(tmpvar->getValues()));
            var.setAttribute("initvalue", vectorToString(tmpvar->getInitValue()));
            var.setAttribute("min", QString::number(tmpvar->getMin()));
            var.setAttribute("max", QString::number(tmpvar->getMax()));
        }
    }
    return true;
}

bool Persistence::saveIterationParameters(QDomElement &root, QDomDocument *doc, Model *model)
{
    //save iterationparameterset
    for(int counter=0; counter<model->numberOfIterationFiles(); counter++)
    {
        //create head
        QDomElement parameterset = doc->createElement("parameterset:iteration");
        QDomCDATASection templateelement = doc->createCDATASection("template");

        root.appendChild(parameterset);
        parameterset.appendChild(templateelement);
        parameterset.setAttribute("valuefilename", model->getIterationFileName(counter));
        templateelement.setNodeValue(model->getIterationTemplate(model->getIterationFileName(counter)));

        //add parameters
        for(int counter2=0; counter2<model->numberOfIterationParameters(model->getIterationFileName(counter)); counter2++)
        {
            Variable *tmpvar = model->getIterationParameter(counter2, model->getIterationFileName(counter));
            QDomElement var = doc->createElement("parameter");
            parameterset.appendChild(var);
            var.setAttribute("name", tmpvar->getName());
            var.setAttribute("value", vectorToString(tmpvar->getValues()));
        }
    }
    return true;
}

bool Persistence::saveResultParameters(QDomElement &root, QDomDocument *doc, Model *model)
{
    //save resultparameterset
    for(int counter=0; counter<model->numberOfResultFiles(); counter++)
    {
        //create head
        QDomElement parameterset = doc->createElement("parameterset:result");
        QDomCDATASection templateelement = doc->createCDATASection("template");

        root.appendChild(parameterset);
        parameterset.appendChild(templateelement);
        parameterset.setAttribute("valuefilename", model->getResultFileName(counter));
        templateelement.setNodeValue(model->getResultTemplate(model->getResultFileName(counter)));

        //add parameters
        for(int counter2=0; counter2<model->numberOfResultParameters(model->getResultFileName(counter)); counter2++)
        {
            Variable *tmpvar = model->getResultParameter(counter2, model->getResultFileName(counter));
            QDomElement var = doc->createElement("parameter");
            parameterset.appendChild(var);
            var.setAttribute("name", tmpvar->getName());
            var.setAttribute("value", vectorToString(tmpvar->getValues()));
        }
    }
    return true;
}

bool Persistence::saveGroups(QDomElement &root, QDomDocument *doc, Model *model)
{
    //save groups
    for(int counter=0; counter<model->getGroupNames().size(); counter++)
    {
        //create head
        QVector<CalibrationVariable*> groupvector = model->getGroup(model->getGroupNames().at(counter));
        QDomElement group = doc->createElement("group");
        root.appendChild(group);
        group.setAttribute("name", model->getGroupNames().at(counter));

        //add parameters
        for(int counter2=0; counter2<groupvector.size(); counter2++)
        {
            QDomElement var = doc->createElement("parameter");
            group.appendChild(var);
            var.setAttribute("name", groupvector.at(counter2)->getName());
        }
    }


    for(int counter=0; counter<model->getEvaluatingGroups().size(); counter++)
    {
        //create head
        QDomElement group = doc->createElement("group:evaluating");
        root.appendChild(group);
        group.setAttribute("name", model->getEvaluatingGroups().at(counter));
    }

    for(int counter=0; counter<model->getNotEvaluatingGroups().size(); counter++)
    {
        //create head
        QDomElement group = doc->createElement("group:notevaluating");
        root.appendChild(group);
        group.setAttribute("name", model->getNotEvaluatingGroups().at(counter));
    }



    return true;
}

bool Persistence::saveModelParameters(QDomElement &root, QDomDocument *doc, Model *model)
{
    QDomElement var = doc->createElement("model:parameters");
    root.appendChild(var);
    var.setAttribute("maxerror", QString::number(model->getMaxError()));
    var.setAttribute("maxiterations", QString::number(model->getMaxIterations()));
    var.setAttribute("script", model->getAlg());
    var.setAttribute("cleariterations", model->getClearIterations());
    var.setAttribute("cpunum", model->getCpus());
    var.setAttribute("preexec", model->getPreExec());
    var.setAttribute("preexecargument", model->getPreExecArgument());
    var.setAttribute("iterationexec", model->getIterationExec());
    var.setAttribute("iterationexecargument", model->getIterationExecArgument());
    var.setAttribute("postexec", model->getPostExec());
    var.setAttribute("postexecargument", model->getPostExecArgument());
    var.setAttribute("preworkspace", model->getPreWorkspace());
    var.setAttribute("postworkspace", model->getPostWorkspace());
    var.setAttribute("iterationworkspace", model->getIterationWorkspace());


    return true;
}

int Persistence::loadModelParameters(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(forcescriptupdate);
    qDebug() << "Persistence::loadModelParameters()";
    QDomNodeList varlist = doc->elementsByTagName("model:parameters");

    for(int counter=0; counter<varlist.size(); counter++)
    {
        QDomElement element = varlist.at(counter).toElement();
        model->setMaxError(element.attribute("maxerror").toDouble());
        model->setMaxIterations(element.attribute("maxiterations").toInt());
        model->setAlg(element.attribute("script"));
        model->setClearIterations(element.attribute("cleariterations").toInt());
        model->setPreExec(element.attribute("preexec"));
        model->setPreExecArgument(element.attribute("preexecargument"));
        model->setIterationExec(element.attribute("iterationexec"));
        model->setIterationExecArgument(element.attribute("iterationexecargument"));
        model->setPostExec(element.attribute("postexec"));
        model->setPostExecArgument(element.attribute("postexecargument"));
        model->setIterationWorkspace(element.attribute("iterationworkspace"));
        model->setPostWorkspace(element.attribute("postworkspace"));
        model->setPreWorkspace(element.attribute("preworkspace"));
        model->setCpus(element.attribute("cpunum").toInt());

    }
    return true;
}

bool Persistence::saveIterationResults(QDomElement &root, QDomDocument *doc, Model *model)
{
    //save inputparameterset
    qDebug() << "Persistence::saveIterationResults";
    int complete = model->numberOfCompleteIterations();
    for(int counter=0; counter<complete; counter++)
    {
        IterationResult *tmpresult = model->getIterationResult(counter);
        //create head
        QDomElement iterationresult = doc->createElement("iterationresult");
        root.appendChild(iterationresult);
        iterationresult.setAttribute("index", tmpresult->getIterationIndex());

        //add calibration parameters
        for(int counter2=0; counter2<model->getAllCalibrationParameters().size(); counter2++)
        {
            CalibrationVariable *tmpvar = model->getAllCalibrationParameters().at(counter2);
            QDomElement var = doc->createElement("parameterresult:input");
            iterationresult.appendChild(var);
            var.setAttribute("name", tmpvar->getName());
            var.setAttribute("value", vectorToString(tmpresult->getValueOfInputParameter(tmpvar->getName())));

        }

        //add iteration parameters
        for(int counter2=0; counter2<model->getAllIterationParameters().size(); counter2++)
        {
            Variable *tmpvar = model->getAllIterationParameters().at(counter2);
            QDomElement var = doc->createElement("parameterresult:iteration");
            iterationresult.appendChild(var);
            var.setAttribute("name", tmpvar->getName());
            var.setAttribute("value", vectorToString(tmpresult->getValueOfIterationParameter(tmpvar->getName())));
        }

        //add compare parameters
        for(int counter2=0; counter2<model->getAllCompareParameters().size(); counter2++)
        {
            CalculationVariable *tmpvar = model->getAllCompareParameters().at(counter2);
            QDomElement var = doc->createElement("parameterresult:compare");
            iterationresult.appendChild(var);
            var.setAttribute("name", tmpvar->getName());
            var.setAttribute("value", vectorToString(tmpresult->getValueOfCompareParameter(tmpvar->getName())));
        }
    }
    return true;
}

int Persistence::loadIterationResults(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(forcescriptupdate);
    QDomNodeList varlist = doc->elementsByTagName("iterationresult");

    for(int counter=0; counter<varlist.size(); counter++)
    {

        QDomElement element = varlist.at(counter).toElement();
        QDomNodeList inputvars = element.elementsByTagName("parameterresult:input");

        for(int counter2=0; counter2<inputvars.size(); counter2++)
        {
            QDomElement var = inputvars.at(counter2).toElement();
            model->getParameter(var.attribute("name"))->setValues(stringToVector(var.attribute("value")));
        }

        QDomNodeList iterationvars = element.elementsByTagName("parameterresult:iteration");

        for(int counter2=0; counter2<iterationvars.size(); counter2++)
        {
            QDomElement var = iterationvars.at(counter2).toElement();
            model->getParameter(var.attribute("name"))->setValues(stringToVector(var.attribute("value")));
        }

        QDomNodeList comparevars = element.elementsByTagName("parameterresult:compare");

        for(int counter2=0; counter2<comparevars.size(); counter2++)
        {
            QDomElement var = comparevars.at(counter2).toElement();
            model->getParameter(var.attribute("name"))->setValues(stringToVector(var.attribute("value")));
        }

        model->saveIteration(element.attribute("index").toInt());
    }

    QVector<CalculationVariable*> comparevars = model->getAllCompareParameters();
    for(int counter=0; counter<comparevars.size(); counter++)
        comparevars.at(counter)->enableUpdate();
    return true;
}

QString Persistence::vectorToString(QVector<double> vec)
{
    QString result="";

    for(int counter=0; counter<vec.size(); counter++)
    {
        result+= QString::number(vec[counter]);

        if(counter != vec.size()-1)
            result+=QString(" ");
    }
    return result;
}

QVector<double> Persistence::stringToVector(QString string, bool *ok)
{
    if(ok!=NULL)
        *ok=true;

    QVector<double> vec;
    QStringList values = string.trimmed().split(" ");

    for(int counter=0; counter<values.size(); counter++)
    {
        bool currentok = true;

        vec.append(values[counter].toDouble(&currentok));

        if(!currentok && ok!=NULL)
        {
            *ok=false;
        }
    }

    return vec;
}



bool Persistence::saveCalibrationScript(QDomElement &root, QDomDocument *doc, Model *model)
{
    //save calibrationscript
    QDomElement alg = doc->createElement("calibrationalg");
    root.appendChild(alg);
    alg.setAttribute("algname",model->getAlg());

    if((ScriptLibary::getInstance()->getCalibrationScriptNames(ScriptLibary::getInstance()->getCalibrationScriptPath()))->contains(model->getAlg()))
    {
        QDir dir(ScriptLibary::getInstance()->getCalibrationScriptPath() + model->getAlg());
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();

        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            QString filename = fileInfo.fileName();

            if(filename.contains(QRegExp("qs$")))
            {
                QString script = "";
                if(!Persistence::loadFileAsString(ScriptLibary::getInstance()->getCalibrationScriptPath() + model->getAlg() + "/" + filename,&script))
                    return false;

                QDomElement scriptelement = doc->createElement("scriptfile");
                alg.appendChild(scriptelement);
                scriptelement.setAttribute("filename",filename);
                QDomCDATASection code  = doc->createCDATASection(filename);
                scriptelement.appendChild(code);
                code.setNodeValue(script);
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

int Persistence::loadCalibrationScript(QDomDocument *doc, Model *model, bool forcescriptupdate)
{
    Q_UNUSED(model);
    qDebug() << "Persistence::loadCalibrationScript";
    QDomNodeList scriptsectionlist = doc->elementsByTagName("calibrationalg");

    if(scriptsectionlist.size()!=1)
        return false;

    QDomElement scriptsection = scriptsectionlist.at(0).toElement();

    QString algname = scriptsection.attribute("algname");
    QDomNodeList scriptlist = scriptsection.elementsByTagName("scriptfile");

    if(!ScriptLibary::getInstance()->existsCalibrationScript(algname))
    {
        qDebug() << "Persistence::loadCalibrationScript() Import new script from project file";
        ScriptLibary::getInstance()->saveCalibrationScript(algname);

        for(int index=0; index<scriptlist.size(); index++)
        {
            QDomElement script = scriptlist.at(index).toElement();
            QDomNodeList codelist = script.childNodes();

            if(codelist.size()!=1)
                return 0;

            QDomCDATASection code = codelist.at(0).toCDATASection();
            if(!Persistence::saveStringAsFile(ScriptLibary::getInstance()->getCalibrationScriptPath() + algname + "/" + script.attribute("filename"),code.nodeValue()))
            {
                qWarning() << "Persistence::loadCalibrationScript() Error: Not able to save script file";
                return 0;
            }
        }
    }
    else
    {
        //diff check
        qDebug() << "Persistence::loadCalibrationScript() Check diff in script";
        QStringList filelist;

        for(int index=0; index<scriptlist.size(); index++)
        {
            QDomElement script = scriptlist.at(index).toElement();
            QDomNodeList codelist = script.childNodes();

            if(codelist.size()!=1)
                return 0;

            QDomCDATASection code = codelist.at(0).toCDATASection();
            QString currentcode = "";
            if(!Persistence::loadFileAsString(ScriptLibary::getInstance()->getCalibrationScriptPath() + algname + "/" + script.attribute("filename"),&currentcode) || currentcode!=code.nodeValue())
            {
                qDebug() << "Persistence::loadCalibrationScript() Script with same name already exist";

                if(!forcescriptupdate)
                    return -1;

                if(!Persistence::saveStringAsFile(ScriptLibary::getInstance()->getCalibrationScriptPath() + algname + "/" + script.attribute("filename"),code.nodeValue()))
                {
                    qWarning() << "Persistence::loadCalibrationScript() Error: Not able to save script file";
                    return 0;
                }
            }

            filelist.append(script.attribute("filename"));
        }

        QDir dir(ScriptLibary::getInstance()->getCalibrationScriptPath() + algname);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();

        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            QString filename = fileInfo.fileName();

            if(!filelist.contains(filename))
            {
                if(!QFile::remove(ScriptLibary::getInstance()->getCalibrationScriptPath() + algname + "/" + filename))
                    return -2;
            }
        }
    }
    return true;
}
