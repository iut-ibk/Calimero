#include <ExternalParameterRegistry.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <QString>
#include <Logger.h>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QIODevice>
#include <Domain.h>

ExternalParameterRegistry::ExternalParameterRegistry(){}

ExternalParameterRegistry::ExternalParameterRegistry(const ExternalParameterRegistry &oldreg)
{
    regtemplates = oldreg.regtemplates;
    templatepaths = oldreg.templatepaths;
    regparameters = oldreg.regparameters;
    types = oldreg.types;
}

bool ExternalParameterRegistry::registerTemplate(const string &name, const string &templatepath, const string &templatestring, VARTYPE type)
{
    if(regtemplates.find(name)!=regtemplates.end())
        return false;

    if(type==OBJECTIVEFUNCTIONVARIABLE)
    {
        Logger(Error) << "Not possible to register objective function parameter in external file input handler";
        return false;
    }

    regtemplates[name]=templatestring;
    templatepaths[name]=templatepath;
    types[name]=type;
    return true;
}

bool ExternalParameterRegistry::registerParameter(const string &parametername, const string &templatename, Domain *domain)
{
    if(regtemplates.find(templatename)==regtemplates.end())
        return false;

    Variable *var = domain->getPar(parametername);

    if(var->getType()!=types[parametername])
        return false;

    if(regparameters.find(parametername)!=regparameters.end())
        return false;

    regparameters[parametername]=templatename;
    return true;
}

bool ExternalParameterRegistry::deleteTemplate(const string &templatename)
{
    if(regtemplates.find(templatename)==regtemplates.end())
        return false;

    regparameters.erase(regparameters.find(templatename));
    templatepaths.erase(templatepaths.find(templatename));
    types.erase(types.find(templatename));

    vector<string> delvars;
    std::pair<string, string>p;

    BOOST_FOREACH(p, regparameters)
        if(p.second==templatename)
            delvars.push_back(p.first);

    BOOST_FOREACH(string var, delvars)
            regparameters.erase(regparameters.find(var));

    return true;
}

bool ExternalParameterRegistry::updateParameters(Domain *domain, int iteration)
{
    std::pair<string,string> p;
    BOOST_FOREACH(p,templatepaths)
    {
        QString path = QString::fromStdString(p.second);
        path.replace(QRegExp("\\$iteration\\$"), QString::number(iteration));
        if(!QFile::exists(path))
            return false;

         QFile inputfile(path);

         if (!inputfile.open(QIODevice::ReadOnly))
              return false;

         QString values = "";
         QTextStream inputstream(&inputfile);
         values+=inputstream.readAll();
         inputfile.close();

        if(!updateParameters(domain,p.first,values.toStdString()))
            return false;
    }

    return false;
}

bool ExternalParameterRegistry::updateParameters(Domain *domain, const string &templatename, const string &values)
{
    //check existence of templatename and regfile
    if(regtemplates.find(templatename)==regtemplates.end())
        return false;

    bool containspar=false;
    std::pair<string, string> p;
    BOOST_FOREACH(p,regparameters)
            if(p.second==templatename)
            {
                containspar=true;
                break;
            }

    if(!containspar)
        return false;

    //start extracting data
    QString parameterfilestring = QString::fromStdString(regtemplates[templatename]).simplified();
    QString valuefilestring = QString::fromStdString(values).simplified();

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

            if(!domain->contains(parametername.toStdString()))
                return false;

            Variable* tmpvar = domain->getPar(parametername.toStdString());
            vector<double> tmpvec = tmpvar->getValues();

            if(!isvectorelement)
                tmpvec.clear();

            tmpvec.assign(1,value);
            tmpvar->setValues(tmpvec);
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

bool ExternalParameterRegistry::createvalueFiles(Domain *dom, int iteration)
{
    bool result = true;
    std::pair<string,VARTYPE>p;
    BOOST_FOREACH(p,types)
            if(p.second==CALIBRATIONVARIABLE)
                result = result && createValueFile(p.first,dom,iteration);

    return result;
}

bool ExternalParameterRegistry::createValueFile(const string &templatename, Domain *domain, int iteration)
{
    if(regtemplates.find(templatename)==regtemplates.end())
        return false;

    QString path = QString::fromStdString(templatepaths[templatename]);
    if(path=="")
        return false;

    path.replace(QRegExp("\\$iteration\\$"), QString::number(iteration));
    return createValueFile(templatename,domain,path.toStdString());
}


bool ExternalParameterRegistry::createValueFile(const string &templatename, Domain *domain, const string &filepath)
{
    //check existence of templatename and regfile
    if(regtemplates.find(templatename)==regtemplates.end())
        return false;

    bool containspar=false;
    std::pair<string, string> p;
    BOOST_FOREACH(p,regparameters)
            if(p.second==templatename)
            {
                containspar=true;
                break;
            }

    if(!containspar)
        return false;

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
                std::pair<string,string>p;
                BOOST_FOREACH(p, regparameters)
                        if(tokens.at(tokenindex).contains(QRegExp("^" + QString::fromStdString(p.first) + "(_\\d+)?$")))
                        {
                            var = domain->getPar(p.first);
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

    //save file
    QFile outputfile(QString::fromStdString(filepath));
    if(!outputfile.open(QIODevice::WriteOnly))
        return false;

    QTextStream outputstream(&outputfile);
    outputstream << result;
    outputfile.close();
    return true;
}

string ExternalParameterRegistry::getPath(const string &templatename)
{
    if(templatepaths.find(templatename)==templatepaths.end())
        return false;

    return templatepaths[templatename];
}

vector<string> ExternalParameterRegistry::getAllTemplateNames()
{
    vector<string> result;
    std::pair<string,string>p;

    BOOST_FOREACH(p,regtemplates)
            result.push_back(p.first);

    return result;
}
