#include <ExternalParameterRegistry.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <QString>
#include <Logger.h>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QIODevice>

ExternalParameterRegistry::ExternalParameterRegistry(){}

bool ExternalParameterRegistry::registerTemplate(string name, const string &templatestring)
{
    if(containsTemplate(name))
        return false;

    regtemplates[name]=templatestring;
    regparameters[name] = map<string, map<string, Variable* > >();

    return true;
}

bool ExternalParameterRegistry::containsTemplate(string name)
{
    return regtemplates.find(name)!=regtemplates.end();
}

bool ExternalParameterRegistry::importFile(string templatename, string path)
{
    if(!containsTemplate(templatename))
        return false;
    if(containsRegFile(templatename,path))
        return false;

    regparameters[templatename][path]=map<string, Variable* >();
    return true;
}

bool ExternalParameterRegistry::registerParameter(Variable* parameter, string templatename, string regfile)
{
    if(!containsRegFile(templatename,regfile))
        return false;

    if(containsParameter(parameter,templatename,regfile))
        return false;

    if(parameter->getType()==Variable::OBJECTIVEFUNCTIONVARIABLE)
    {
        set<Variable*>* iterationpar = (static_cast<ObjectiveFunctionVariable*>(parameter))->getIterationParameters();
        set<Variable*>* observedpar = (static_cast<ObjectiveFunctionVariable*>(parameter))->getObservedParameters();
        set<ObjectiveFunctionVariable*>* objectivepar = (static_cast<ObjectiveFunctionVariable*>(parameter))->getObjectiveFunctionParameters();

        set<Variable*>::const_iterator cii;
        set<ObjectiveFunctionVariable*>::const_iterator ci;
        for(cii=observedpar->begin(); cii!=observedpar->end(); cii++)
            registerParameter(*cii,templatename,regfile);

        for(cii=iterationpar->begin(); cii!=iterationpar->end(); cii++)
            registerParameter(*cii,templatename,regfile);

        for(ci=objectivepar->begin(); ci!=objectivepar->end(); ci++)
            registerParameter(*ci,templatename,regfile);

    }

    regparameters[templatename][regfile][parameter->getName()]=parameter;
    return true;
}

bool ExternalParameterRegistry::deleteTemplate(string templatename)
{
    if(!containsTemplate(templatename))
        return false;

    regtemplates.erase(regtemplates.find(templatename));
    regparameters.erase(regparameters.find(templatename));

    return true;
}

bool ExternalParameterRegistry::deleteRegFile(string templatename, string regfile)
{
    if(!containsRegFile(templatename,regfile))
        return false;

    regparameters[templatename].erase(regparameters[templatename].find(regfile));
    return true;
}

bool ExternalParameterRegistry::containsRegFile(string templatename, string regfile)
{
    if(!containsTemplate(templatename))
        return false;

    return regparameters[templatename].find(regfile)!=regparameters[templatename].end();
}

bool ExternalParameterRegistry::containsParameter(Variable* var, string templatename, string regfile)
{
    if(!containsRegFile(templatename,regfile))
        return false;

    if(!containsParameter(var->getName(),templatename, regfile))
        return false;

    return regparameters[templatename][regfile][var->getName()]==var;
}

bool ExternalParameterRegistry::containsParameter(const string &name, string templatename, string regfile)
{
    if(!containsRegFile(templatename,regfile))
        return false;

    return regparameters[templatename][regfile].find(name)!=regparameters[templatename][regfile].end();
}

bool ExternalParameterRegistry::updateParameters(string templatename, string regfile)
{
    //check existence of templatename and regfile
    if(!containsRegFile(templatename,regfile))
        return false;

    if(!regparameters[templatename][regfile].size())
    {
        Logger(Error) << "No parameters registered for [" << regfile << "] in template [" << templatename << "]";
        return false;
    }

    if(!QFile::exists(QString::fromStdString(regfile)))
    {
        Logger(Error) << "[" << regfile << "] does not exist";
        return false;
    }

    //read input data
    QFile inputfile(QString::fromStdString(regfile));

    if (!inputfile.open(QIODevice::ReadOnly))
         return false;

    QString valuefilestring = "";
    QString parameterfilestring(QString::fromStdString(regtemplates[templatename]));

    QTextStream inputstream(&inputfile);
    valuefilestring+=inputstream.readAll();
    inputfile.close();

    //start extracting data
    parameterfilestring = parameterfilestring.simplified();
    valuefilestring = valuefilestring.simplified();

    QStringList templateparameters;
    QStringList parametersplit = parameterfilestring.split("$");

    int textposition = 0;

    for(int index=0; index < parametersplit.size(); index++)
    {
        if(index%2==1)
        {
            QString parametername=parametersplit.at(index);

            //searching for double
            bool doubleok = true;
            int valuelength = 10;
            double value = 0;

            while(doubleok)
            {
                value = valuefilestring.mid(textposition,valuelength).toDouble(&doubleok);
                if(valuefilestring.length() < textposition+valuelength)
                    break;
                valuelength+=5;
            }

            while(!doubleok)
            {
                valuelength--;
                if(valuelength<1)
                    return false;

                value = valuefilestring.mid(textposition,valuelength).toDouble(&doubleok);
            }

            valuelength=valuefilestring.mid(textposition,valuelength).trimmed().length();
            textposition+=valuelength;

            if(textposition>valuefilestring.length())
                return false;

            //check if parameter is a vector element
            bool isvectorelement=false;

            if(parametername.contains(QRegExp(".+_.+")))
            {
                isvectorelement=true;
                parametername=parametername.remove(QRegExp("_.+"));
            }

            if(containsParameter(parametername.toStdString(),templatename,regfile))
            {
                Variable* tmpvar = regparameters[templatename][regfile][parametername.toStdString()];
                vector<double> tmpvec = tmpvar->getValues();

                if(!isvectorelement)
                    tmpvec.clear();

                tmpvec.assign(1,value);
                tmpvar->setValues(tmpvec);
            }
        }
        else
        {
            QString s1 = parametersplit.at(index);
            QString s2 = valuefilestring.mid(textposition,parametersplit.at(index).length());

            if(s1.length()!=s2.length())
                return false;

            textposition+=parametersplit.at(index).length();
        }
    }

    return true;

}

bool ExternalParameterRegistry::createValueFile(const string &templatename, const string &regfile)
{
    //check existence of templatename and regfile
    if(!containsRegFile(templatename,regfile))
        return false;

    if(!regparameters[templatename][regfile].size())
    {
        Logger(Error) << "No parameters registered for [" << regfile << "] in template [" << templatename << "]";
        return false;
    }

    QString result = "";
    QStringList stringlist = QString::fromStdString(regtemplates[templatename]).split("\n");

    for(int counter=0; counter<stringlist.size(); counter++)
    {
        QString line = stringlist.at(counter);

        while(line.contains("$"))
        {
            QStringList tokens = line.split("$");

            Variable *var = NULL;

            for(int tokenindex=0; tokenindex<tokens.size(); tokenindex++)
            {
                std::pair<string,Variable*>p;
                BOOST_FOREACH(p, regparameters[templatename][regfile])
                        if(tokens.at(tokenindex).contains(QRegExp("^" + QString::fromStdString(p.first) + "(_\\d+)?$")))
                        {
                            var = p.second;
                            break;
                        }

                if(var!=NULL)
                {

                    uint parameterindex = 0;

                    if(tokens.at(tokenindex).contains(QString::fromStdString(var->getName())))
                    {
                        if(tokens.at(tokenindex).contains("_"))
                        {
                            QStringList parametersplit = tokens.at(tokenindex).split("_");

                            if(parametersplit.size()<2)
                                return false;

                            bool indexok = false;

                            parameterindex = parametersplit.at(1).toInt(&indexok);

                            if(!indexok)
                                return false;
                        }
                    }

                    if(parameterindex >= var->getValues().size())
                        return false;

                    if(parameterindex==0)
                    {
                        line.replace(QRegExp("\\$" + QString::fromStdString(var->getName()) + "\\$"), QString::number(var->getValues().at(parameterindex)));
                    }
                    else
                    {
                        line.replace(QRegExp("\\$" + QString::fromStdString(var->getName()) + QString("_") + QString::number(parameterindex) + "\\$"), QString::number(var->getValues().at(parameterindex)));
                    }
                }
            }
        }

        result+=line;

        if(counter<stringlist.size()-1)
            result+="\n";
    }

    QFile outputfile(QString::fromStdString(regfile));

    if(!outputfile.open(QIODevice::WriteOnly))
        return false;

    QTextStream outputstream(&outputfile);
    outputstream << result;
    outputfile.close();
    return true;
}

ExternalParameterRegistry* ExternalParameterRegistry::clone()
{
    ExternalParameterRegistry *result = new ExternalParameterRegistry();
    result->regtemplates=this->regtemplates;
    map<string, map<string, map<string, Variable* > > > tmpregparameters;

    std::pair<string,string> p;
    BOOST_FOREACH(p,result->regtemplates)
            tmpregparameters[p.first]= map<string, map<string, Variable* > >();

    return result;
}
