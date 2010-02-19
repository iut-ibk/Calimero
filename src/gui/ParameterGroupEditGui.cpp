#include "ParameterGroupEditGui.h"
#include "../core/CalibrationVariable.h"
#include "../core/Model.h"
#include <QMessageBox>
#include <QInputDialog>
#include <limits>
#include "../persistence/Persistence.h"

using namespace std;

ParameterGroupEditGui::ParameterGroupEditGui(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    currentgroup = "";
    currentparameter = "";
    parameterchooser = new ParameterChooserGui(this);
    QObject::connect(listWidget_group, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentGroupChange(QString)));
    QObject::connect(listWidget_inputparameter, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentParameterChange(QString)));
    QObject::connect(b_newgroup, SIGNAL(clicked()), this, SLOT(newGroup()));
    QObject::connect(b_deletegroup, SIGNAL(clicked()), this, SLOT(deleteGroup()));

    QObject::connect(b_addparameter, SIGNAL(clicked()), this, SLOT(addParameterToGroup()));
    QObject::connect(b_removeparameter, SIGNAL(clicked()), this, SLOT(removeParameterFromGroup()));

    QObject::connect(parameterchooser->buttonBox, SIGNAL(accepted()),this,SLOT(addSelectedParameters()));
    QObject::connect(parameterchooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));

    lineEdit_max->setMaximum(numeric_limits<double>::max());
    lineEdit_max->setMinimum(-(numeric_limits<double>::max()));
    lineEdit_min->setMaximum(numeric_limits<double>::max());
    lineEdit_min->setMinimum(-(numeric_limits<double>::max()));
}

ParameterGroupEditGui::~ParameterGroupEditGui()
{
    QObject::disconnect(listWidget_group, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentGroupChange(QString)));
    QObject::disconnect(listWidget_inputparameter, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentParameterChange(QString)));
    QObject::disconnect(b_newgroup, SIGNAL(clicked()), this, SLOT(newGroup()));
    QObject::disconnect(b_deletegroup, SIGNAL(clicked()), this, SLOT(deleteGroup()));

    QObject::disconnect(b_addparameter, SIGNAL(clicked()), this, SLOT(addParameterToGroup()));
    QObject::disconnect(b_removeparameter, SIGNAL(clicked()), this, SLOT(removeParameterFromGroup()));

    QObject::disconnect(parameterchooser->buttonBox, SIGNAL(accepted()),this,SLOT(addSelectedParameters()));
    QObject::disconnect(parameterchooser->buttonBox, SIGNAL(rejected()),this,SLOT(parameterChooserRejected()));

    delete parameterchooser;
}

void ParameterGroupEditGui::addParameterToGroup()
{
    if(currentgroup=="default")
        return;

    parameterchooser->listWidget->clear();

    QVector<CalibrationVariable*> parameters = model->getAllCalibrationParameters();

    QVector<CalibrationVariable*> group = model->getGroup(currentgroup);

    for(int counter=0; counter<parameters.size(); counter++)
    {
        if(!group.contains(parameters.at(counter)))
            parameterchooser->listWidget->addItem(parameters.at(counter)->getName());
    }
    parameterchooser->setVisible(true);
}

void ParameterGroupEditGui::addSelectedParameters()
{
    QList<QListWidgetItem *> items = parameterchooser->listWidget->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        CalibrationVariable *tmpvar = static_cast<CalibrationVariable*>(model->getParameter(items.at(counter)->text()));

        model->addParameterToGroup(currentgroup, tmpvar);
    }

    parameterchooser->setVisible(false);
}

void ParameterGroupEditGui::removeParameterFromGroup()
{
    QList<QListWidgetItem *> items = listWidget_inputparameter->selectedItems();

    for(int counter=0; counter<items.size(); counter++)
    {
        CalibrationVariable *tmpvar = static_cast<CalibrationVariable*>(model->getParameter(items.at(counter)->text()));

        model->removeParameterFromGroup(currentgroup, tmpvar->getName());
    }
}

void ParameterGroupEditGui::newGroup()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Please enter a group name:"),tr("Groupname:"), QLineEdit::Normal,"", &ok);

    if (ok && !name.isEmpty())
    {
        if(!model->existsGroup(name))
        {
            currentgroup=name;
            model->newGroup(name);
        }
        else
        {
            QMessageBox::warning(this,tr("Warning"),tr("Groupname already exists"));
        }
    }
}

void ParameterGroupEditGui::deleteGroup()
{
    if(listWidget_group->selectedItems().size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),"Please choose a group");
    }
    else
    {
        model->deleteGroup(listWidget_group->selectedItems().at(0)->text());
    }
}

void ParameterGroupEditGui::currentParameterChange( const QString & currentText )
{
    currentparameter=currentText;

    if(currentgroup!=""&&model->existsGroup(currentgroup))
    {
        QVector<CalibrationVariable*> group = model->getGroup(currentgroup);

        if(!model->existsParameter(currentparameter))
        {
            if(group.size()>0)
            {
                currentparameter=group.at(0)->getName();
            }
            else
            {
                currentparameter="";
            }
        }
        else
        {
            if(!group.contains(static_cast<CalibrationVariable*>(model->getParameter(currentparameter))))
            {

                if(group.size()>0)
                {
                    currentparameter=group.at(0)->getName();
                }
                else
                {
                    currentparameter="";
                }
            }
        }

        if(currentparameter!="")
        {
            CalibrationVariable* parameter = static_cast<CalibrationVariable*>(model->getParameter(currentparameter));
            label_name->setText(parameter->getName());

            QString value="";
            QVector<double> vec = parameter->getInitValue();
            int maxsize = vec.size();
            if(maxsize>10)
                maxsize=10;

            for(int counter=0; counter<maxsize; counter++)
                value+=QString::number(vec[counter]) + " ";

            lineEdit_initvalue->setText(value);

            value="";
            vec = parameter->getInitValue();

            for(int counter=0; counter<maxsize; counter++)
                value+=QString::number(vec[counter]) + " ";


            label_value->setText(value);
            lineEdit_max->setValue(parameter->getMax());
            lineEdit_min->setValue(parameter->getMin());
        }
        else
        {
            label_name->setText("");
            lineEdit_initvalue->setText("");
            label_value->setText("");
            lineEdit_max->setValue(0);
            lineEdit_min->setValue(0);
        }
    }
}

void ParameterGroupEditGui::currentGroupChange( const QString & currentText )
{
    currentgroup=currentText;

    if(currentgroup!="" && model->existsGroup(currentgroup))
    {
        listWidget_inputparameter->clear();

        QVector<CalibrationVariable*> parameters = model->getGroup(currentgroup);

        for(int counter=0; counter<parameters.size(); counter++)
        {
            listWidget_inputparameter->addItem(parameters.at(counter)->getName());
        }
        listWidget_inputparameter->setSortingEnabled(true);


        currentParameterChange(currentparameter);
    }
    else
    {
        return currentGroupChange(model->getGroupNames().at(0));
    }
}

void ParameterGroupEditGui::update(Model* m)
{
    qDebug("ParameterGroupEditGui::update()");

    QObject::disconnect(listWidget_group, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentGroupChange(QString)));
    QObject::disconnect(listWidget_inputparameter, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentParameterChange(QString)));

    if(model!=m)
        model=m;

    //clear section
    listWidget_group->clear();
    listWidget_inputparameter->clear();

    //groups
    QStringList groups = model->getGroupNames();
    for(int counter=0; counter<groups.size(); counter++)
    {
        if(currentgroup=="")
            currentgroup=groups.at(counter);

        listWidget_group->addItem(groups.at(counter));
    }

    listWidget_group->setSortingEnabled(true);

    //update currentgroup
    currentGroupChange(currentgroup);

    QList<QListWidgetItem*> currentgroupitem = listWidget_group->findItems(currentgroup,0);


    if(currentgroupitem.size()==1)
        currentgroupitem.at(0)->setSelected(true);
    QObject::connect(listWidget_group, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentGroupChange(QString)));
    QObject::connect(listWidget_inputparameter, SIGNAL(currentTextChanged ( QString )), this, SLOT(currentParameterChange(QString)));
}

void ParameterGroupEditGui::parameterChooserRejected()
{
    parameterchooser->setVisible(false);
}

void ParameterGroupEditGui::changeEvent(QEvent *e)
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

void ParameterGroupEditGui::on_lineEdit_max_valueChanged(double value)
{
    if(!model->existsParameter(currentparameter))
        return;

    (dynamic_cast<CalibrationVariable*>(model->getParameter(currentparameter)))->setMax(value);
}

void ParameterGroupEditGui::on_lineEdit_min_valueChanged(double value)
{
    if(!model->existsParameter(currentparameter))
        return;

    (dynamic_cast<CalibrationVariable*>(model->getParameter(currentparameter)))->setMin(value);
}

void ParameterGroupEditGui::on_lineEdit_initvalue_editingFinished()
{
    bool ok = false;
    QVector<double> vars = Persistence::stringToVector(lineEdit_initvalue->text(),&ok);
    CalibrationVariable *var = dynamic_cast<CalibrationVariable*>(model->getParameter(currentparameter));

    if(!ok)
    {
        lineEdit_initvalue->setText(Persistence::vectorToString(var->getInitValue()));
        qWarning() << "ParameterGroupEditGui::on_lineEdit_initvalue_editingFinished() Wrong Vector in lineEdit -> initvalue";
        return;
    }

    var->setInitValue(vars);
    lineEdit_initvalue->setText(Persistence::vectorToString(var->getInitValue()));
}
