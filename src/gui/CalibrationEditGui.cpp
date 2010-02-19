#include "CalibrationEditGui.h"
#include "CoreEngine.h"

CalibrationEditGui::CalibrationEditGui(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    model=NULL;
    QObject::connect(listWidget_allplusgroups, SIGNAL(itemClicked(QListWidgetItem * )),this, SLOT(plusGroup(QListWidgetItem *)));
    QObject::connect(listWidget_allminusgroups, SIGNAL(itemClicked(QListWidgetItem * )),this, SLOT(minusGroup(QListWidgetItem *)));
    QObject::connect(listWidget_allcompareparameters, SIGNAL(itemClicked(QListWidgetItem * )),this, SLOT(calulationparameter(QListWidgetItem*)));
    QObject::connect(lineEdit_iteration, SIGNAL(textEdited (QString)),this, SLOT(textEdited (QString)));
    QObject::connect(checkBox_clear, SIGNAL(clicked()),this, SLOT(checkBoxStateChanged()));
    QObject::connect(b_newscript, SIGNAL(clicked()),this,SLOT(newScript()));
    QObject::connect(b_editscript, SIGNAL(clicked()),this,SLOT(editScript()));
    QObject::connect(comboBox_script, SIGNAL(activated( const QString)),this,SLOT(setScript(const QString)));
    groupBox_7->setVisible(false);
    groupBox_8->setVisible(false);
    groupBox_9->setVisible(false);
}

CalibrationEditGui::~CalibrationEditGui()
{
    QObject::disconnect(listWidget_allplusgroups, SIGNAL(itemClicked(QListWidgetItem * )),this, SLOT(plusGroup(QListWidgetItem *)));
    QObject::disconnect(listWidget_allplusgroups, SIGNAL(itemEntered(QListWidgetItem * )),this, SLOT(selectionChanged()));
    QObject::disconnect(listWidget_allminusgroups, SIGNAL(itemClicked(QListWidgetItem * )),this, SLOT(minusGroup(QListWidgetItem *)));
    QObject::disconnect(listWidget_allminusgroups, SIGNAL(itemEntered(QListWidgetItem * )),this, SLOT(selectionChanged()));
    QObject::disconnect(lineEdit_iteration, SIGNAL(textEdited (QString)),this, SLOT(textEdited (QString)));
    QObject::disconnect(listWidget_allcompareparameters, SIGNAL(itemClicked(QListWidgetItem * )),this, SLOT(calulationparameter(QListWidgetItem*)));
    QObject::disconnect(checkBox_clear, SIGNAL(clicked()),this, SLOT(checkBoxStateChanged()));
    QObject::disconnect(b_newscript, SIGNAL(clicked()),this,SLOT(newScript()));
    QObject::disconnect(b_editscript, SIGNAL(clicked()),this,SLOT(editScript()));
    QObject::disconnect(comboBox_script, SIGNAL(activated( const QString)),this,SLOT(setScript(const QString)));
}

void CalibrationEditGui::selectionChanged()
{
    update(model);
}

void CalibrationEditGui::plusGroup( QListWidgetItem * item )
{
    QStringList plusgroups = model->getEvaluatingGroups();

    if(plusgroups.contains(item->text()))
    {
        if(!model->removeEvaluatingGroup(item->text()))
            update(model);
    }
    else
    {
        if(!model->addEvaluatingGroup(item->text()))
            update(model);
    }
}

void CalibrationEditGui::minusGroup( QListWidgetItem * item )
{
    QStringList minusgroups = model->getNotEvaluatingGroups();

    if(minusgroups.contains(item->text()))
    {
        if(!model->removeNotEvaluatingGroup(item->text()))
            update(model);
    }
    else
    {
        if(!model->addNotEvaluatingGroup(item->text()))
            update(model);
    }
}

void CalibrationEditGui::update(Model *m)
{
    qDebug("CalibrationEditGui::update()");
    if(model!=m)
        model=m;

    //clear listwidgets
    listWidget_allplusgroups->clear();
    listWidget_allminusgroups->clear();
    listWidget_allcompareparameters->clear();


    //setcompareparameters
    QVector<CalculationVariable*> calcvars(model->getAllCompareParameters());
    QVector<CalculationVariable*> algvars(model->getAlgParameters());
    
    for(int counter=0; counter<calcvars.size(); counter++)
    {
        listWidget_allcompareparameters->addItem(calcvars[counter]->getName());
        if(algvars.contains(static_cast<CalculationVariable*>(model->getParameter(calcvars[counter]->getName()))))
            listWidget_allcompareparameters->setCurrentRow(counter);
    }
    
    //set plusgroups and minusgroups
    QStringList allgroups = model->getGroupNames();
    QStringList plusgroups = model->getEvaluatingGroups();
    QStringList minusgroups = model->getNotEvaluatingGroups();

    if(allgroups.size()>1)
    {
        groupBox_9->setVisible(true);
        for(int counter=0; counter<allgroups.size(); counter++)
        {
            listWidget_allplusgroups->addItem(allgroups.at(counter));
            if(plusgroups.contains(allgroups.at(counter)))
                listWidget_allplusgroups->setCurrentRow(listWidget_allplusgroups->count()-1);


            listWidget_allminusgroups->addItem(allgroups.at(counter));
            if(minusgroups.contains(allgroups.at(counter)))
                listWidget_allminusgroups->setCurrentRow(listWidget_allminusgroups->count()-1);

        }
    }
    else
    {
        groupBox_9->setVisible(false);
    }

    //cleariterations
    checkBox_clear->setCheckState(Qt::Unchecked);
    if(model->getClearIterations())
        checkBox_clear->setCheckState(Qt::Checked);

    //setmaxIterations
    lineEdit_iteration->setText(QString::number(model->getMaxIterations()));

    //script
    comboBox_script->clear();

    if(model->getAlg()!="")
    {
        comboBox_script->addItem(model->getAlg());
    }
    else
    {
        comboBox_script->addItem("");
    }

    comboBox_script->addItems(*(ScriptLibary::getInstance()->getAllCalibrationScriptNames()));

    line_preexec->setText(model->getPreExec());
    line_preexecargument->setText(model->getPreExecArgument());
    line_postexecargument->setText(model->getPostExecArgument());
    line_postexec->setText(model->getPostExec());
    line_iterationexec->setText(model->getIterationExec());
    line_iterationexecargument->setText(model->getIterationExecArgument());
    line_iterationexec_w->setText(model->getIterationWorkspace());
    line_postexec_w->setText(model->getPostWorkspace());
    line_preexec_w->setText(model->getPreWorkspace());
    spinBox_cpus->setValue(model->getCpus());
    lineEdit_max_error->setText(QString::number(model->getMaxError()));
}

void CalibrationEditGui::textEdited(const QString & text)
{
    model->setMaxIterations(text.toInt());
}

void CalibrationEditGui::newScript()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Please enter a scriptname:"),tr("Scriptname:"), QLineEdit::Normal,"", &ok);

    if (ok && !name.isEmpty() && !ScriptLibary::getInstance()->existsCalibrationScript(name))
    {
        model->setAlg(name);
        ScriptLibary::getInstance()->saveCalibrationScript(name);
    }
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("Could not create script"));
    }
}

void CalibrationEditGui::editScript()
{
    QString script = comboBox_script->currentText();
    scriptedit = new ::ScriptEditWidget(this);
    scriptedit->setVisible(false);

    if(ScriptLibary::getInstance()->existsCalibrationScript(script))
    {
        QScriptEngine *scriptengine = model->getCoreEngine()->getScriptEngine();
        if(scriptengine==NULL)
        {
            QMessageBox::warning(this,tr("Warning"),tr("Please stop calibration"));
            return;
        }

        scriptedit->setScriptEngine(scriptengine);
        scriptedit->setScriptPathDir(ScriptLibary::getInstance()->getCalibrationScriptPath()  + script);
        scriptedit->setNameFilter("*.qs");
        scriptedit->setVisible(true);
    }
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("Not allowed to edit this script"));
    }

}

void CalibrationEditGui::setScript(const QString & text)
{
        model->setAlg(QString(text));
}

void CalibrationEditGui::checkBoxStateChanged()
{
    model->setClearIterations(checkBox_clear->isChecked());
}

void CalibrationEditGui::on_line_preexec_returnPressed()
{
    model->setPreExec(line_preexec->text());
}

void CalibrationEditGui::on_line_preexecargument_returnPressed()
{
    model->setPreExecArgument(line_preexecargument->text());
}

void CalibrationEditGui::on_line_postexec_returnPressed()
{
    model->setPostExec(line_postexec->text());
}

void CalibrationEditGui::on_line_postexecargument_returnPressed()
{
    model->setPostExecArgument(line_postexecargument->text());
}

void CalibrationEditGui::on_line_iterationexec_returnPressed()
{
    model->setIterationExec(line_iterationexec->text());
}

void CalibrationEditGui::on_line_iterationexecargument_returnPressed()
{
    model->setIterationExecArgument(line_iterationexecargument->text());
}

void CalibrationEditGui::on_b_pre_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose a program"),model->getPreWorkspace());

    if(file!="")
    {
        model->setPreExec(file);
    }
}

void CalibrationEditGui::on_b_exec_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose a program"),model->getIterationWorkspace());

    if(file!="")
    {
        model->setIterationExec(file);
    }
}

void CalibrationEditGui::on_b_post_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose a program"),model->getPostWorkspace());

    if(file!="")
    {
        model->setPostExec(file);
    }
}

void CalibrationEditGui::on_spinBox_cpus_valueChanged ( int i )
{
    model->setCpus(i);
}

void CalibrationEditGui::calulationparameter(QListWidgetItem * item)
{
    if(model==NULL)
        return;

    if(!model->addCalculationVariableToCalibration(item->text()))
        model->removeCalculationVariableFromCalibration(item->text());
}

void CalibrationEditGui::changeEvent(QEvent *e)
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

void CalibrationEditGui::on_b_pre_w_clicked()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(dirname!="")
        model->setPreWorkspace(dirname);
}

void CalibrationEditGui::on_b_exec_w_clicked()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(dirname!="")
        model->setIterationWorkspace(dirname);
}
void CalibrationEditGui::on_b_post_w_clicked()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(dirname!="")
        model->setIterationWorkspace(dirname);
}

void CalibrationEditGui::on_line_postexec_w_returnPressed()
{
    model->setPostWorkspace(line_postexec_w->text());
}

void CalibrationEditGui::on_line_iterationexec_w_returnPressed()
{
    model->setIterationWorkspace(line_iterationexec_w->text());
}

void CalibrationEditGui::on_line_preexec_w_returnPressed()
{
    model->setPreWorkspace(line_preexec_w->text());
}

void CalibrationEditGui::on_lineEdit_max_error_returnPressed()
{
    bool ok = true;
    double value = lineEdit_max_error->text().simplified().toDouble(&ok);

    if(!ok)
    {
        lineEdit_max_error->setText(QString::number(model->getMaxError()));
        return;
    }

    model->setMaxError(value);
    lineEdit_max_error->setText(QString::number(model->getMaxError()));
}
