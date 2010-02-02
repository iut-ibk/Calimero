#include "CompareParameterEditGui.h"
#include "../core/CalculationVariable.h"
#include "../qtscript/ScriptLibary.h"
#include <QtDebug>
#include <QInputDialog>
#include <QMessageBox>


CompareParameterEditGui::CompareParameterEditGui(QWidget *parent) : QWidget(parent)
{
    qDebug("CompareParameterEditGui::CompareParameterEditGui()");
    setupUi(this);

    currentparameter=NULL;
    currentparametername = "";

    label_name->setText("");
    label_value->setText("");

    scriptedit = new ::ScriptEditWidget(this);
    scriptedit->setVisible(false);

    comparechooser = new ParameterChooserGui(this);
    resultchooser = new ParameterChooserGui(this);
    iterationchooser = new ParameterChooserGui(this);

    comparechooser->setVisible(false);
    resultchooser->setVisible(false);
    iterationchooser->setVisible(false);

    QObject::connect(b_addcompareparameter, SIGNAL(clicked()),this,SLOT(addCompareParameter()));
    QObject::connect(b_addresultparameter, SIGNAL(clicked()),this,SLOT(addResultParameter()));
    QObject::connect(b_additerationparameter, SIGNAL(clicked()),this,SLOT(addIterationParameter()));

    QObject::connect(b_removecompareparameter, SIGNAL(clicked()),this,SLOT(removeCompareParameter()));
    QObject::connect(b_removeresultparameter, SIGNAL(clicked()),this,SLOT(removeResultParameter()));
    QObject::connect(b_removeiterationparameter, SIGNAL(clicked()),this,SLOT(removeIterationParameter()));

    QObject::connect(comparechooser->buttonBox, SIGNAL(accepted()),this,SLOT(compareParameterOk()));
    QObject::connect(resultchooser->buttonBox, SIGNAL(accepted()),this,SLOT(resultParameterOk()));
    QObject::connect(iterationchooser->buttonBox, SIGNAL(accepted()),this,SLOT(iterationParameterOk()));
    QObject::connect(comparechooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));
    QObject::connect(resultchooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));
    QObject::connect(iterationchooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));

    QObject::connect(b_newcompareparameter, SIGNAL(clicked()),this,SLOT(newCompareParameter()));
    QObject::connect(b_deletecompareparameter, SIGNAL(clicked()),this,SLOT(deleteCompareParameter()));

    QObject::connect(listWidget_compareparameter, SIGNAL(itemClicked ( QListWidgetItem * )),this,SLOT(selectionChanged( QListWidgetItem * )));
    QObject::connect(b_newscript, SIGNAL(clicked()),this,SLOT(newScript()));
    QObject::connect(b_editscript, SIGNAL(clicked()),this,SLOT(editScript()));

    QObject::connect(comboBox_script, SIGNAL(activated( const QString)),this,SLOT(setScript(const QString)));
}

CompareParameterEditGui::~CompareParameterEditGui()
{
    qDebug("CompareParameterEditGui::~CompareParameterEditGui()");
    QObject::disconnect(b_addcompareparameter, SIGNAL(clicked()),this,SLOT(addCompareParameter()));
    QObject::disconnect(b_addresultparameter, SIGNAL(clicked()),this,SLOT(addResultParameter()));
    QObject::disconnect(b_additerationparameter, SIGNAL(clicked()),this,SLOT(addIterationParameter()));

    QObject::disconnect(b_removecompareparameter, SIGNAL(clicked()),this,SLOT(removeCompareParameter()));
    QObject::disconnect(b_removeresultparameter, SIGNAL(clicked()),this,SLOT(removeResultParameter()));
    QObject::disconnect(b_removeiterationparameter, SIGNAL(clicked()),this,SLOT(removeIterationParameter()));

    QObject::disconnect(comparechooser->buttonBox, SIGNAL(accepted()),this,SLOT(compareParameterOk()));
    QObject::disconnect(resultchooser->buttonBox, SIGNAL(accepted()),this,SLOT(resultParameterOk()));
    QObject::disconnect(iterationchooser->buttonBox, SIGNAL(accepted()),this,SLOT(iterationParameterOk()));
    QObject::disconnect(comparechooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));
    QObject::disconnect(resultchooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));
    QObject::disconnect(iterationchooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));

    QObject::disconnect(b_newcompareparameter, SIGNAL(clicked()),this,SLOT(newCompareParameter()));
    QObject::disconnect(b_deletecompareparameter, SIGNAL(clicked()),this,SLOT(deleteCompareParameter()));

    QObject::disconnect(listWidget_compareparameter, SIGNAL(itemClicked ( QListWidgetItem * )),this,SLOT(selectionChanged( QListWidgetItem * )));
    QObject::disconnect(b_newscript, SIGNAL(clicked()),this,SLOT(newScript()));
    QObject::disconnect(b_editscript, SIGNAL(clicked()),this,SLOT(editScript()));

    QObject::disconnect(comboBox_script, SIGNAL(activated( const QString)),this,SLOT(setScript(const QString)));

    delete comparechooser;
    delete resultchooser;
    delete iterationchooser;
    delete scriptedit;
}

void CompareParameterEditGui::newCompareParameter()
{
    qDebug("CompareParameterEditGui::newCompareParameter()");
    bool ok;
    QString name = QInputDialog::getText(this, tr("Please enter a parametername:"),tr("Parametername:"), QLineEdit::Normal,"", &ok);

    if (ok && !name.isEmpty())
    {
        if(model->isFreeParameterName(name))
        {
                CalculationVariable *newvar = new CalculationVariable(name);
                if(!(model->addCompareParameter(newvar)))
                {
                    delete newvar;
                    QMessageBox::warning(this,tr("Warning"),tr("Could not create parameter"));
                }
                newvar->setAlg("default");
                currentparameter=newvar;
        }
        else
        {
            QMessageBox::warning(this,tr("Warning"),tr("Parametername already exists"));
        }
    }
}

void CompareParameterEditGui::deleteCompareParameter()
{
    if(listWidget_compareparameter->selectedItems().size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("Please choose a parameter"));
    }
    else
    {
        currentparameter=NULL;
        model->deleteCompareParameter(listWidget_compareparameter->selectedItems().at(0)->text());

    }

}

void CompareParameterEditGui::compareParameterOk()
{
    QList<QListWidgetItem *> items = comparechooser->listWidget->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        Variable *tmpvar = model->getParameter(items.at(counter)->text());
        if(tmpvar==NULL)
        {
            return;
        }

        if(!currentparameter->addCompareParameter(static_cast<CalculationVariable*>(tmpvar)))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Including parameter \"") + tmpvar->getName() + tr("\" will lead to an infinite loop "));
            return;
        }
    }

    comparechooser->setVisible(false);
}

void CompareParameterEditGui::iterationParameterOk()
{
    QList<QListWidgetItem *> items = iterationchooser->listWidget->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        Variable *tmpvar = model->getParameter(items.at(counter)->text());
        if(tmpvar==NULL)
            return;

        currentparameter->addIterationParameter(tmpvar);
    }

    iterationchooser->setVisible(false);
}

void CompareParameterEditGui::resultParameterOk()
{
    QList<QListWidgetItem *> items = resultchooser->listWidget->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        Variable *tmpvar = model->getParameter(items.at(counter)->text());
        if(tmpvar==NULL)
            return;

        currentparameter->addResultParameter(tmpvar);
    }

    resultchooser->setVisible(false);
}

void CompareParameterEditGui::parameterChooserRejected()
{
    resultchooser->setVisible(false);
    iterationchooser->setVisible(false);
    comparechooser->setVisible(false);
}

void CompareParameterEditGui::removeCompareParameter()
{
    QList<QListWidgetItem *> items = listWidget_compareparameterinput->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        Variable *tmpvar = model->getParameter(items.at(counter)->text());
        if(tmpvar==NULL)
            return;

        currentparameter->removeCompareParameter(static_cast<CalculationVariable*>(tmpvar));
    }
}

void CompareParameterEditGui::removeResultParameter()
{
    QList<QListWidgetItem *> items = listWidget_resultparameterinput->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        Variable *tmpvar = model->getParameter(items.at(counter)->text());
        if(tmpvar==NULL)
            return;

        currentparameter->removeResultParameter(tmpvar);
    }
}

void CompareParameterEditGui::removeIterationParameter()
{
    QList<QListWidgetItem *> items = listWidget_iterationparameterinput->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        Variable *tmpvar = model->getParameter(items.at(counter)->text());
        if(tmpvar==NULL)
            return;

        currentparameter->removeIterationParameter(tmpvar);
    }
}

void CompareParameterEditGui::addCompareParameter()
{
    resultchooser->setVisible(false);
    iterationchooser->setVisible(false);
    comparechooser->setVisible(false);
    if(model->getAllCompareParameters().size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("No parameter available"));
        return;
    }

    comparechooser->listWidget->clear();

    for(int counter=0; counter<model->getAllCompareParameters().size(); counter++)
    {
        comparechooser->listWidget->addItem(model->getAllCompareParameters().at(counter)->getName());
    }
    comparechooser->setVisible(true);
}

void CompareParameterEditGui::addResultParameter()
{
    resultchooser->setVisible(false);
    iterationchooser->setVisible(false);
    comparechooser->setVisible(false);
    if(model->getAllResultParameters().size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("No parameter available"));
        return;
    }

    resultchooser->listWidget->clear();

    for(int counter=0; counter<model->getAllResultParameters().size(); counter++)
    {
        resultchooser->listWidget->addItem(model->getAllResultParameters().at(counter)->getName());
    }
    resultchooser->setVisible(true);
}

void CompareParameterEditGui::addIterationParameter()
{
    resultchooser->setVisible(false);
    iterationchooser->setVisible(false);
    comparechooser->setVisible(false);
    if(model->getAllIterationParameters().size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("No parameter available"));
        return;
    }

    iterationchooser->listWidget->clear();

    for(int counter=0; counter<model->getAllIterationParameters().size(); counter++)
    {
        iterationchooser->listWidget->addItem(model->getAllIterationParameters().at(counter)->getName());
    }
    iterationchooser->setVisible(true);
}

void CompareParameterEditGui::newScript()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Please enter a scriptname:"),tr("Scriptname:"), QLineEdit::Normal,"", &ok);

    if (ok && !name.isEmpty() && !ScriptLibary::getInstance()->existsCompareScript(name))
    {
        currentparameter->setAlg(name);
        ScriptLibary::getInstance()->saveCompareScript(name);
        this->update(model);
    }
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("Could not create script"));
    }
}

void CompareParameterEditGui::editScript()
{
    QString script = comboBox_script->currentText();



    scriptedit = new ::ScriptEditWidget(this);
    scriptedit->setVisible(false);

    if(ScriptLibary::getInstance()->existsCompareScript(script))
    {

         scriptedit->setScriptEngine(dynamic_cast<CalculationVariable*>(model->getParameter(listWidget_compareparameter->selectedItems().at(0)->text()))->getScriptEngine());
         scriptedit->setScriptPathDir(ScriptLibary::getInstance()->getCompareScriptPath()  + script);
         scriptedit->setNameFilter("*.qs");
         scriptedit->setVisible(true);
    }
}

void CompareParameterEditGui::selectionChanged( QListWidgetItem * item)
{
    currentparameter = dynamic_cast<CalculationVariable*>(model->getParameter(item->text()));
    currentparametername = currentparameter->getName();
    update(model);
}

void CompareParameterEditGui::update(Model *m)
{
    qDebug("CompareParameterEditGui::update()");

    if(model!=m)
    {
        model=m;
    }

    listWidget_compareparameter->clear();
    listWidget_compareparameterinput->clear();
    listWidget_iterationparameterinput->clear();
    listWidget_resultparameterinput->clear();
    comboBox_script->clear();

    int currentselection = 0;

    if(!model->existsParameter(currentparametername))
    {
        currentparameter = NULL;
        currentparametername = "";
    }
    else
    {
        currentparameter=dynamic_cast<CalculationVariable*>(model->getParameter(currentparametername));
    }

    for(int counter=0; counter<model->numberOfCompareParameters(); counter++)
    {

        if(currentparameter==NULL && counter==0 && model->numberOfCompareParameters()>0)
        {
            currentparameter=model->getCompareParameter(counter);
            currentparametername = currentparameter->getName();
        }

        listWidget_compareparameter->addItem(model->getCompareParameter(counter)->getName());
        if(currentparameter==model->getCompareParameter(counter))
            currentselection=counter;
    }

    if(model->numberOfCompareParameters()>0)
        listWidget_compareparameter->setCurrentRow(currentselection);

    if(currentparameter!=NULL)
    {
        label_name->setText(currentparameter->getName());

        QVector<double> vector(currentparameter->getValues());
        QString values = "";

        int maxsize = vector.size();
        if(maxsize>10)
            maxsize=10;

        for(int index=0; index<maxsize; index++)
            values+=QString::number(vector.at(index),'f',2) + " ";

        label_value->setText(values);

        if(currentparameter->getAlg()!="")
        {
            comboBox_script->addItem(currentparameter->getAlg());
        }
        else
        {
            comboBox_script->addItem("");
        }

        comboBox_script->addItems(*(ScriptLibary::getInstance()->getAllCompareScriptNames()));

        for(int counter=0; counter<currentparameter->sizeOfIterationParameter(); counter++)
        {
            listWidget_iterationparameterinput->addItem(currentparameter->atIterationParameter(counter)->getName());
        }

        for(int counter=0; counter<currentparameter->sizeOfResultParameter(); counter++)
        {
            listWidget_resultparameterinput->addItem(currentparameter->atResultParameter(counter)->getName());
        }

        for(int counter=0; counter<currentparameter->sizeOfCompareParameter(); counter++)
        {
            listWidget_compareparameterinput->addItem(currentparameter->atCompareParameter(counter)->getName());
        }
    }

    if(currentparameter==NULL)
    {
        label_name->setText("");
        label_value->setText("");
        groupBox->setEnabled(false);
    }
    else
    {
        groupBox->setEnabled(true);
    }
}

void CompareParameterEditGui::setScript(const QString & text)
{
    currentparameter->setAlg(QString(text));
}

void CompareParameterEditGui::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}


