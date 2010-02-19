#ifndef VARIABLECONTAINER_H
#define VARIABLECONTAINER_H

#include <QtCore>
#include "Variable.h"
#include "CalibrationVariable.h"
#include "ParameterPersistenceEvaluator.h"

template<class T = Variable> class VariableContainer
{
    private:
        int type;

        ParameterPersistenceEvaluator *evaluator;

        QMap<QString, QString> templates;
        QMap<QString, QVector<T*> *> vars;
        QMap<QString, T*> var;


    public:
        VariableContainer(ParameterPersistenceEvaluator *eval, int type)
        {
            if(eval == NULL)
                qFatal("VariableContainer: ParameterPersistenceEvaluator is a Null pointer");

            if(type < 0 || type > 4)
                qFatal("VariableContainer: VARIABLE TYPE IS NOT VALID");

            evaluator=eval;
            this->type = type;
        }

        ~VariableContainer()
        {
            for(int index=0; index < var.size(); index++)
                delete var.values().at(index);

            var.clear();
            templates.clear();
            vars.clear();
        }

        bool removeVar(QString name)
        {
            return removeVar(getVar(name));
        }

        bool removeVar(T *var)
        {
            qDebug("->VariableContainer::removeVar()");
            if(!contains(var))
                return false;

            QStringList stringlist = getTemplate(getTemplateName(var->getName())).split("\n");
            QString resulttemplate = "";

            for(int counter=0; counter<stringlist.size(); counter++)
            {
                QCoreApplication::processEvents();
                QString line = stringlist.at(counter);

                while(line.contains("$" + var->getName()))
                {
                    QCoreApplication::processEvents();
                    QStringList tokens = line.split("$");

                    for(int tokenindex=0; tokenindex<tokens.size(); tokenindex++)
                    {
                        int parameterindex = 0;

                        if(tokens.at(tokenindex).contains(var->getName()))
                        {
                            if(tokens.at(tokenindex).contains("_"))
                            {
                                QStringList parametersplit = tokens.at(tokenindex).split("_");

                                if(parametersplit.size()<2)
                                    qFatal("VariableContainer::removeVar(): ERROR in Templatefile (No Vector)");

                                bool indexok = false;

                                parameterindex = parametersplit.at(1).toInt(&indexok);

                                if(!indexok)
                                {
                                    qDebug() << "VariableContainer::removeVar(): Wrong Index = " << parameterindex;
                                    qFatal("VariableContainer::removeVar(): ERROR in Templatefile (Wrong index)");
                                }
                            }
                        }

                        if(parameterindex>=var->getValues().size())
                            qFatal("VariableContainer::removeVar(): ERROR in Templatefile (No Vector index)");

                        if(type==Variable::CALIBRATIONVARIABLE)
                        {
                            if(parameterindex==0)
                            {
                                line.replace(QRegExp("\\$" + var->getName() + "\\$"), QString::number(dynamic_cast<CalibrationVariable*>(var)->getInitValue().at(parameterindex)));
                            }
                            else
                            {
                                line.replace(QRegExp("\\$" + var->getName() + QString("_") + QString::number(parameterindex) + "\\$"), QString::number(dynamic_cast<CalibrationVariable*>(var)->getInitValue().at(parameterindex)));
                            }
                        }
                        else
                        {
                            if(parameterindex==0)
                            {
                                line.replace(QRegExp("\\$" + var->getName() + "\\$"), QString::number(var->getValues().at(parameterindex)));
                            }
                            else
                            {
                                line.replace(QRegExp("\\$" + var->getName() + QString("_") + QString::number(parameterindex) + "\\$"), QString::number(var->getValues().at(parameterindex)));
                            }
                        }
                    }
                }

                resulttemplate+=line;

                if(counter<stringlist.size()-1)
                    resulttemplate+="\n";
            }

            qDebug("VariableContainer::removeVar()");
            QString templatename = getTemplateName(var);
            templates.remove(templatename);
            templates.insert(templatename,resulttemplate);
            QVector<T*> *vector = vars.value(getTemplateName(var->getName()));
            vector->remove(vector->indexOf(var));
            this->var.remove(var->getName());
            evaluator->removeParameter(var->getName());
            delete var;

            qDebug("<-VariableContainer::removeVar()");
            return true;
        }

        bool removeVars(QString templatename)
        {
            if(!containsTemplate(templatename))
                return false;

            templates.remove(templatename);

            QVector<T*> *vector = vars.value(templatename);

            for(int counter=0; counter< vector->size(); counter++)
            {
                if(!removeVar(vector->at(counter)))
                    qFatal("VariableContainer: ERROR cannot delete Template");

            }

            vars.remove(templatename);
            delete vector;

            return true;
        }

        bool addVar(T *newvar, QString templatename, QString templatestring)
        {  
            if(newvar == NULL)
                return false;

            if(newvar->getType() != type)
                return false;

            if(contains(newvar))
                return false;

            if(templatename == "")
                return false;

            if(!containsTemplate(templatename))
                return false;

            if(!evaluator->isFreeParameterName(newvar->getName()))
                return false;

            if(templatestring == "")
                return false;

            templates.remove(templatename);
            templates.insert(templatename,templatestring);

            vars.value(templatename)->append(newvar);
            var.insert(newvar->getName(),newvar);
            evaluator->addParameter(newvar->getName());

            return true;
        }



        bool addVars(QVector<T*> *varsvector, QString templatename, QString templatestring)
        {   
            if(varsvector == NULL)
                return false;

            if(templatename == "")
                return false;

            if(templatestring == "")
                return false;

            if(containsTemplate(templatename))
                return false;

            for(int counter=0; counter<varsvector->size(); counter++)
            {
                if(contains(varsvector->at(counter)) || !evaluator->isFreeParameterName(varsvector->at(counter)->getName()))
                    return false;
            }

            templates.insert(templatename,templatestring);
            vars.insert(templatename, varsvector);

            for(int counter=0; counter<varsvector->size(); counter++)
            {
                var.insert(varsvector->at(counter)->getName(),varsvector->at(counter));
                evaluator->addParameter(varsvector->at(counter)->getName());
            }

            return true;
        }

        T* getVar(QString name)
        {
            if(name == "")
                qFatal("VariableContainer: Variable name is empty");

            if(!contains(name))
                qFatal("VariableContainer: Variable is not part of the VariableContainer");

            return var.value(name);
        }

        QVector<T*> getAllVars()
        {
            return var.values().toVector();
        }

        QVector<T*> * getVars(QString templatename)
        {
            if(templatename=="")
                qFatal("VariableContainer: Templatename is empty");

            if(!containsTemplate(templatename))
                qFatal("VariableContainerr: Template does not exist");

            return vars.value(templatename);
        }

        bool renameTemplateName(QString oldname, QString newname)
        {
            if(!templates.contains(oldname))
                qFatal("VariableContainer: Template does not exist");

            if(newname == "")
                return false;

            QString templatestring = templates.value(oldname);
            templates.remove(oldname);
            templates.insert(newname,templatestring);

            QVector<T*>* parameters = vars.value(oldname);
            vars.remove(oldname);
            vars.insert(newname,parameters);

            return true;
        }

        QVector<QString> getAllTemplateNames()
        {
            return templates.keys().toVector();
        }

        QString getTemplateName(T *var)
        {
            if(var == NULL)
                qFatal("VariableContainer: Variable pointer is a NULL pointer");

            if(!contains(var))
                qFatal("VariableContainer: Variable is not part of the VariableContainer");

            QVector<QVector<T*> *> vector = vars.values().toVector();

            for(int counter=0; counter<vector.size(); counter++)
            {
                if(vector.at(counter)->contains(var))
                {
                    return vars.key(vector.at(counter));
                }
            }

            qFatal("VariableContainer: ERROR in VariableContainer this section should never be reached");
            return "";
        }

        QString getTemplateName(QString varname)
        {
            return getTemplateName(getVar(varname));
        }

        bool newTemplate(QString templatename, QString templatestring)
        {
            if(templates.contains(templatename))
                return false;

            if(templatestring=="")
                return false;

            templates.insert(templatename,templatestring);
            vars.insert(templatename, new QVector<T*>());
            return true;
        }

        QString getTemplate(QString templatename)
        {
            if(templatename == "")
                qFatal("VariableContainer: Templatename is empty");

            if(!containsTemplate(templatename))
                qFatal("VariableContainer: Templatename is not part of the VariableContainer");

            return templates.value(templatename);
        }

        QString getTemplateByVarName(QString varname)
        {
            return getTemplate(getTemplateName(varname));
        }

        QString getTemplate(T var)
        {
            return getTemplate(var.getName());
        }

        bool contains(T* var)
        {
            if(var == NULL)
                return false;

            return contains(var->getName());
        }

        bool contains(QString varname)
        {
            return var.contains(varname);
        }

        bool containsTemplate(QString templatename)
        {
            return templates.contains(templatename);
        }
};

#endif // VARIABLECONTAINER_H
