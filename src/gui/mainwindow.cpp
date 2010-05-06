#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QStateMachine>
#include <Logger.h>
#include <iostream>
#include <Log.h>
#include <guilogsink.h>
#include <PyEnv.h>
#include <FunctionLoader.h>
#include <PyException.h>
#include <CalibrationEnv.h>
#include <PyFunctionLoader.h>
#include <QtGui>
#include <PyException.h>
#include <groupmanager.h>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
        ui->setupUi(this);
        setupStateMachine();

        log_updater = new GuiLogSink();
        Log::init(log_updater,DEFAULTLOGLEVEL);
        ui->log_widget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);

        QMessageBox::warning(this,tr("JJJJJYYYYYEEEEESSSSSS!!!!"),tr("BALD URLAUB"));
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::setupStateMachine() {
        state_machine = new QStateMachine();

        //init
        QState *init = new QState();

        //Parameter tab
        QState *tab_states = new QState(QState::ParallelStates);

            //variable group
            QState *var_group = new QState(tab_states);

                //Variable properties
                QState *var_prop = new QState(var_group);
                var_prop->assignProperty(ui->var_widget, "visible", true);
                var_prop->assignProperty(ui->button_groupmanage, "visible",false);
                var_prop->assignProperty(ui->calvar_widget, "visible", false);
                var_prop->assignProperty(ui->ovar_widget, "visible", false);
                var_prop->assignProperty(ui->delvar, "enabled",false);
                var_group->setInitialState(var_prop);
                    QState *var_clean = new QState(var_prop);
                    var_prop->setInitialState(var_clean);
                    var_clean->assignProperty(ui->var_widget, "enabled", false);
                    var_clean->assignProperty(ui->varname,"text", "");
                    var_clean->assignProperty(ui->vardelvalue,"enabled", false);
                    var_clean->assignProperty(ui->varvalues,"rowCount", 0);

                    QState *var_enabled = new QState(var_prop);
                    var_enabled->assignProperty(ui->var_widget, "enabled", true);

                    var_clean->addTransition(ui->vars, SIGNAL(itemClicked ( QListWidgetItem * )),var_enabled);
                    var_enabled->addTransition(this, SIGNAL(disable_varsettings()),var_clean);


                //Objective function variable properties
                QState *ovar_prop = new QState(var_group);
                ovar_prop->assignProperty(ui->var_widget, "visible", false);
                ovar_prop->assignProperty(ui->calvar_widget, "visible", false);
                ovar_prop->assignProperty(ui->ovar_widget, "visible", true);
                ovar_prop->assignProperty(ui->ovaradvanced, "enabled", false);
                ovar_prop->assignProperty(ui->delvar, "enabled",false);
                ovar_prop->assignProperty(ui->button_groupmanage, "visible",false);

                    QState *ovar_clean = new QState(ovar_prop);
                    ovar_prop->setInitialState(ovar_clean);
                    ovar_clean->assignProperty(ui->ovar_widget, "enabled", false);
                    ovar_clean->assignProperty(ui->ovardelmember, "enabled", false);
                    ovar_clean->assignProperty(ui->varname,"text", "");
                    ovar_clean->assignProperty(ui->ovarvalues, "text", "");

                    QState *ovar_enabled = new QState(ovar_prop);
                    ovar_enabled->assignProperty(ui->ovar_widget, "enabled", true);

                    ovar_clean->addTransition(ui->vars, SIGNAL(itemClicked ( QListWidgetItem * )),ovar_enabled);
                    ovar_enabled->addTransition(this, SIGNAL(disable_varsettings()),ovar_clean);

                //Calibration variable properties
                QState *calvar_prop = new QState(var_group);
                calvar_prop->assignProperty(ui->var_widget, "visible", false);
                calvar_prop->assignProperty(ui->calvar_widget, "visible", true);
                calvar_prop->assignProperty(ui->ovar_widget, "visible", false);
                calvar_prop->assignProperty(ui->delvar, "enabled",false);
                calvar_prop->assignProperty(ui->button_groupmanage, "visible",true);
                var_group->setInitialState(calvar_prop);

                    QState *calvar_clean = new QState(calvar_prop);
                    calvar_prop->setInitialState(calvar_clean);
                    calvar_clean->assignProperty(ui->calvar_widget, "enabled", false);
                    calvar_clean->assignProperty(ui->varname,"text", "");

                    calvar_clean->assignProperty(ui->calvarinit,"value",0);
                    calvar_clean->assignProperty(ui->calvarinit,"minimum",0);
                    calvar_clean->assignProperty(ui->calvarinit,"maximum",0);
                    calvar_clean->assignProperty(ui->calvarinit,"singleStep",1);

                    calvar_clean->assignProperty(ui->calvarmin,"value",0);
                    calvar_clean->assignProperty(ui->calvarmin,"minimum",-100000000);
                    calvar_clean->assignProperty(ui->calvarmin,"maximum",0);
                    calvar_clean->assignProperty(ui->calvarmin,"singleStep",1);

                    calvar_clean->assignProperty(ui->calvarmax,"value",0);
                    calvar_clean->assignProperty(ui->calvarmax,"minimum",0);
                    calvar_clean->assignProperty(ui->calvarmax,"maximum",1000000000);
                    calvar_clean->assignProperty(ui->calvarmax,"singleStep",1);

                    calvar_clean->assignProperty(ui->calvarstep,"value",1);
                    calvar_clean->assignProperty(ui->calvarstep,"minimum",0);
                    calvar_clean->assignProperty(ui->calvarstep,"maximum",1);
                    calvar_clean->assignProperty(ui->calvarstep,"singleStep",0.0000001);
                    calvar_clean->assignProperty(ui->del_group,"enabled",false);

                    QState *calvar_enabled = new QState(calvar_prop);
                    calvar_enabled->assignProperty(ui->calvar_widget, "enabled", true);

                    calvar_clean->addTransition(ui->vars, SIGNAL(itemClicked ( QListWidgetItem * )),calvar_enabled);
                    calvar_enabled->addTransition(this, SIGNAL(disable_varsettings()),calvar_clean);

                //transitions
                var_prop->addTransition(this, SIGNAL(show_ovar()), ovar_prop);
                var_prop->addTransition(this, SIGNAL(show_calvar()), calvar_prop);
                ovar_prop->addTransition(this, SIGNAL(show_var()), var_prop);
                ovar_prop->addTransition(this, SIGNAL(show_calvar()), calvar_prop);
                calvar_prop->addTransition(this, SIGNAL(show_ovar()), ovar_prop);
                calvar_prop->addTransition(this, SIGNAL(show_var()), var_prop);
                init->addTransition(this, SIGNAL(start_gui()),tab_states);

                QObject::connect(init,SIGNAL(entered()),this,SLOT(init()));
                QObject::connect(var_clean,SIGNAL(entered()),ui->varvalues, SLOT(clearContents()));
                QObject::connect(ovar_clean,SIGNAL(entered()),ui->ovarmembers, SLOT(clear()));
                QObject::connect(calvar_clean,SIGNAL(entered()),ui->groups, SLOT(clear()));


                //template editor
                QState *template_editor = new QState(tab_states);
                QState *iteration_template = new QState(template_editor);
                QState *observed_template = new QState(template_editor);
                QState *calibration_template = new QState(template_editor);
                template_editor->setInitialState(calibration_template);
                calibration_template->assignProperty(ui->button_settemplatepath, "visible", true);
                calibration_template->assignProperty(ui->group_editor,"enabled", false);
                calibration_template->assignProperty(ui->templateeditor, "plainText", "");
                calibration_template->assignProperty(ui->deltemplate, "enabled", false);
                calibration_template->assignProperty(ui->button_settemplatepath, "enabled", false);

                    QState *calibration_template_clean = new QState(calibration_template);
                    calibration_template->setInitialState(calibration_template_clean);
                    calibration_template_clean->assignProperty(ui->group_editor, "enabled", false);
                    calibration_template_clean->assignProperty(ui->templateeditor, "plainText", "");

                    QState *calibration_template_enabled = new QState(calibration_template);
                    calibration_template_enabled->assignProperty(ui->group_editor, "enabled", true);

                    calibration_template_clean->addTransition(ui->templates, SIGNAL(itemClicked ( QListWidgetItem * )),calibration_template_enabled);
                    calibration_template_enabled->addTransition(this, SIGNAL(disable_templateeditor()),calibration_template_clean);

                iteration_template->assignProperty(ui->button_settemplatepath,"visible", true);
                iteration_template->assignProperty(ui->group_editor,"enabled", false);
                iteration_template->assignProperty(ui->templateeditor, "plainText", "");
                iteration_template->assignProperty(ui->deltemplate, "enabled", false);
                iteration_template->assignProperty(ui->button_settemplatepath, "enabled", false);

                    QState *iteration_template_clean = new QState(iteration_template);
                    iteration_template->setInitialState(iteration_template_clean);
                    iteration_template_clean->assignProperty(ui->group_editor, "enabled", false);
                    iteration_template_clean->assignProperty(ui->templateeditor, "plainText", "");

                    QState *iteration_template_enabled = new QState(iteration_template);
                    iteration_template_enabled->assignProperty(ui->group_editor, "enabled", true);

                    iteration_template_clean->addTransition(ui->templates, SIGNAL(itemClicked ( QListWidgetItem * )),iteration_template_enabled);
                    iteration_template_enabled->addTransition(this, SIGNAL(disable_templateeditor()),iteration_template_clean);

                observed_template->assignProperty(ui->button_settemplatepath,"visible", false);
                observed_template->assignProperty(ui->group_editor,"enabled", false);
                observed_template->assignProperty(ui->templateeditor, "plainText", "");
                observed_template->assignProperty(ui->deltemplate, "enabled", false);
                observed_template->assignProperty(ui->button_settemplatepath, "enabled", false);

                    QState *observed_template_clean = new QState(observed_template);
                    observed_template->setInitialState(observed_template_clean);
                    observed_template_clean->assignProperty(ui->group_editor, "enabled", false);
                    observed_template_clean->assignProperty(ui->templateeditor, "plainText", "");

                    QState *observed_template_enabled = new QState(observed_template);
                    observed_template_enabled->assignProperty(ui->group_editor, "enabled", true);

                    observed_template_clean->addTransition(ui->templates, SIGNAL(itemClicked ( QListWidgetItem * )),observed_template_enabled);
                    observed_template_enabled->addTransition(this, SIGNAL(disable_templateeditor()),observed_template_clean);


                calibration_template->addTransition(this, SIGNAL(show_iterationvar_template()), iteration_template);
                calibration_template->addTransition(this, SIGNAL(show_observedvar_template()), observed_template);
                iteration_template->addTransition(this, SIGNAL(show_calvar_template()), calibration_template);
                iteration_template->addTransition(this, SIGNAL(show_observedvar_template()), observed_template);
                observed_template->addTransition(this, SIGNAL(show_calvar_template()), calibration_template);
                observed_template->addTransition(this, SIGNAL(show_iterationvar_template()), iteration_template);



        //start statemachine
        state_machine->addState(tab_states);
        state_machine->addState(init);
        state_machine->setInitialState(init);
        state_machine->start();
}

void MainWindow::init()
{
    Logger(Debug) << "init called";

    PyFunctionLoader::loadScripts("./");
    PyFunctionLoader::loadScripts("../../scripts/");
    FunctionLoader::loadNative("./");

    vector<string> ofunvec = CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getAvailableFunctions();

    QStringList ofunlist;
    //no function
    ofunlist.append(QString::fromStdString(""));
    BOOST_FOREACH(string name, ofunvec)
            ofunlist.append(QString::fromStdString(name));

    ui->ovarofun->addItems(ofunlist);

    Q_EMIT start_gui();
}

void MainWindow::setOFunction()
{
    if(!CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getSettingTypes(ui->ovarofun->currentText().toStdString()).size())
    {
        Q_EMIT show_ovar();
    }
    else
    {
        Q_EMIT show_ovar_advanced();
    }
}

void MainWindow::on_newvar_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Parameter name"),tr("Name:"), QLineEdit::Normal,"", &ok);

    if (ok && !text.isEmpty())
    {
        switch(ui->comboBox->currentIndex())
        {
        case 0:
            ok = CalibrationEnv::getInstance()->getCalibration()->addParameter(new CalibrationVariable(text.toStdString(),vector<double>(1)));
            if(ok)
                ui->vars->addItem(text);
            break;
        case 1:
            ok = CalibrationEnv::getInstance()->getCalibration()->addParameter(new Variable(text.toStdString(),vector<double>(1),ITERATIONVARIABLE));
            if(ok)
                ui->vars->addItem(text);
            break;
        case 2:
            ok = CalibrationEnv::getInstance()->getCalibration()->addParameter(new Variable(text.toStdString(),vector<double>(1),OBSERVEDVARIABLE));
            if(ok)
                ui->vars->addItem(text);
            show_var();
            break;
        case 3:
            ok = CalibrationEnv::getInstance()->getCalibration()->addParameter(new ObjectiveFunctionVariable(text.toStdString()));
            if(ok)
                ui->vars->addItem(text);
            break;
        }
    }
}

void MainWindow::on_comboBox_templates_currentIndexChanged(int index)
{
    ui->templates->clear();
    VARTYPE type;

    switch(index)
    {
    case 0:
        type = CALIBRATIONVARIABLE;
        Q_EMIT show_calvar_template();
        break;
    case 1:
        type = ITERATIONVARIABLE;
        Q_EMIT show_iterationvar_template();
        break;
    case 2:
        type = OBSERVEDVARIABLE;
        Q_EMIT show_observedvar_template();
        break;
    }

    vector<string> templatevec = CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->getTemplateNames(type);
    BOOST_FOREACH(string templatestring, templatevec)
            ui->templates->addItem(QString::fromStdString(templatestring));
}


void MainWindow::on_comboBox_currentIndexChanged ( int index )
{
    Domain *domain = CalibrationEnv::getInstance()->getCalibration()->getDomain();
    ui->vars->clear();
    VARTYPE type;

    switch(index)
    {
    case 0:
        type=CALIBRATIONVARIABLE;
        Q_EMIT show_calvar();
        break;
    case 1:
        type=ITERATIONVARIABLE;
        Q_EMIT show_var();
        break;
    case 2:
        type=OBSERVEDVARIABLE;
        Q_EMIT show_var();
        break;
    case 3:
        type=OBJECTIVEFUNCTIONVARIABLE;
        Q_EMIT show_ovar();
        break;
    }

    vector<Variable*> varvec = domain->getAllPars(type);
    BOOST_FOREACH(Variable* var, varvec)
            ui->vars->addItem(QString::fromStdString(var->getName()));
}

void MainWindow::on_vars_itemClicked ( QListWidgetItem * item )
{
    Calibration* calibration = CalibrationEnv::getInstance()->getCalibration();
    Variable* current = calibration->getDomain()->getPar(item->text().toStdString());

    ui->varname->setText(QString::fromStdString(current->getName()));

    switch(current->getType())
    {
    case CALIBRATIONVARIABLE:
        {
            CalibrationVariable* calvar = static_cast<CalibrationVariable*>(current);
            ui->calvarstep->setValue(calvar->getStep());
            ui->calvarmin->setValue(calvar->getMin());
            ui->calvarmax->setValue(calvar->getMax());
            ui->calvarinit->setValue(calvar->getInitValues()[0]);

            ui->groups->clear();
            vector<string> groups = calibration->getAllGroups();
            BOOST_FOREACH(string groupname, groups)
                    if(calibration->containsGroupMember(calvar->getName(),groupname))
                        ui->groups->addItem(QString::fromStdString(groupname));
            break;
        }
    case OBSERVEDVARIABLE:
        {
            ui->varvalues->clearContents();
            ui->varvalues->setRowCount(0);
            ui->vardelvalue->setEnabled(false);
            BOOST_FOREACH(double value, current->getValues())
            {
                ui->varvalues->setRowCount(ui->varvalues->rowCount()+1);
                QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(value));
                ui->varvalues->setItem(ui->varvalues->rowCount()-1, 0, newItem);
            }
            break;
        }
    case ITERATIONVARIABLE:
        {
            ui->varvalues->clearContents();
            ui->varvalues->setRowCount(0);
            ui->vardelvalue->setEnabled(false);
            BOOST_FOREACH(double value, current->getValues())
            {
                ui->varvalues->setRowCount(ui->varvalues->rowCount()+1);
                QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(value));
                ui->varvalues->setItem(ui->varvalues->rowCount()-1, 0, newItem);
            }
            break;
        }
    case OBJECTIVEFUNCTIONVARIABLE:
        {
            ObjectiveFunctionVariable* ovar = static_cast<ObjectiveFunctionVariable*>(current);
            ui->ovarmembers->clear();
            ui->ovardelmember->setEnabled(false);

            //insert iteration parameters
            BOOST_FOREACH(string name,ovar->getIterationParameters())
                    ui->ovarmembers->addItem(QString::fromStdString(name));

            //insert observed parameters
            BOOST_FOREACH(string name,ovar->getObservedParameters())
                    ui->ovarmembers->addItem(QString::fromStdString(name));

            //insert objective function parameters
            BOOST_FOREACH(string name,ovar->getObjectiveFunctionParameters())
                    ui->ovarmembers->addItem(QString::fromStdString(name));

            //set objective function;
            int index=0;
            while(ui->ovarofun->itemText(index)!=QString::fromStdString(ovar->getObjectiveFunction()))
                index++;

            ui->ovarofun->setCurrentIndex(index);

            //set current values
            uint i;
            QString values = "";
            vector<double> vec;

            try
            {
                 vec = ovar->getValues();
            }
            catch (PythonException e)
            {
                Logger(Error) << e.type;
                Logger(Error) << e.value;
            }



            for(i=0; i < vec.size(); i++)
            {
                    values += QString::number(vec[0]) + " ";
                    if(i==2)
                        break;
            }

            if(i < vec.size())
                values += "...";

            ui->ovarvalues->setText(values);
            break;
        }
    }
}

void MainWindow::on_vars_itemSelectionChanged ()
{
    if(!ui->vars->selectedItems().size())
    {
        ui->delvar->setEnabled(false);
        Q_EMIT disable_varsettings();
    }
    else
    {
        ui->delvar->setEnabled(true);
    }
}

void MainWindow::on_templates_itemSelectionChanged ()
{
    if(!ui->templates->selectedItems().size())
    {
        ui->deltemplate->setEnabled(false);
        ui->button_settemplatepath->setEnabled(false);
        Q_EMIT disable_templateeditor();
    }
    else
    {
        ui->deltemplate->setEnabled(true);
        ui->button_settemplatepath->setEnabled(true);
    }
}

void MainWindow::on_delvar_clicked()
{
    QList<QListWidgetItem *> list = ui->vars->selectedItems ();
    for(int index=0; index < list.size(); index++ )
    {
        bool ok = CalibrationEnv::getInstance()->getCalibration()->removeParameter(list.at(index)->text().toStdString());

        if(ok)
            delete list.at(index);
        else
            QMessageBox::warning(this,tr("Delete parameter"),tr("Cannot delete parameters which are members of a template\nParameter: ") + list.at(index)->text());
    }
}

void MainWindow::on_calvarmax_valueChanged(double v) {
    ui->calvarinit->setMaximum(v);
    ui->calvarmin->setMaximum(v);

    if(!ui->vars->selectedItems().size())
        return;

    QListWidgetItem *item = ui->vars->currentItem();
    CalibrationVariable* calvar = static_cast<CalibrationVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(item->text().toStdString()));
    calvar->setMax(v);
}

void MainWindow::on_calvarstep_valueChanged(double v)
{
    ui->calvarinit->setSingleStep(v);
    ui->calvarmax->setSingleStep(v);
    ui->calvarmin->setSingleStep(v);

    if(!ui->vars->selectedItems().size())
        return;

    QListWidgetItem *item = ui->vars->currentItem();
    CalibrationVariable* calvar = static_cast<CalibrationVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(item->text().toStdString()));
    calvar->setStep(v);
}

void MainWindow::on_calvarmin_valueChanged(double v )
{
    ui->calvarinit->setMinimum(v);
    ui->calvarmax->setMinimum(v);

    if(!ui->vars->selectedItems().size())
        return;

    QListWidgetItem *item = ui->vars->currentItem();
    CalibrationVariable* calvar = static_cast<CalibrationVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(item->text().toStdString()));
    calvar->setMin(v);
}

void MainWindow::on_calvarinit_valueChanged(double v)
{
    if(!ui->vars->selectedItems().size())
        return;

    QListWidgetItem *item = ui->vars->currentItem();
    CalibrationVariable* calvar = static_cast<CalibrationVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(item->text().toStdString()));
    vector<double> newvalue(1);
    newvalue[0]=v;
    calvar->setInitValues(newvalue);
}

void MainWindow::on_vardelvalue_clicked()
{
    QList<QTableWidgetItem *> items = ui->varvalues->selectedItems();
    for(int index=0; index < items.size(); index++)
        ui->varvalues->removeRow(items[index]->row());
}

void MainWindow::on_varaddvalue_clicked()
{
    ui->varvalues->setRowCount(ui->varvalues->rowCount()+1);
    QTableWidgetItem *newItem = new QTableWidgetItem("0.0");
    ui->varvalues->setItem(ui->varvalues->rowCount()-1, 0, newItem);
}

void MainWindow::on_varvalues_itemChanged ( QTableWidgetItem * item )
{
    bool ok;
    item->text().toDouble(&ok);
    if(!ok)
    {
        item->setData(Qt::EditRole,"0.0");
        return;
    }

    vector<double> newvector(item->tableWidget()->rowCount());
    for(int index=0; index < item->tableWidget()->rowCount(); index++)
        newvector[index]=item->tableWidget()->item(index,0)->text().toDouble();

    if(!ui->vars->selectedItems().size())
        return;

    QListWidgetItem *selecteditem = ui->vars->currentItem();
    Variable* var = CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(selecteditem->text().toStdString());
    var->setValues(newvector);
}

void MainWindow::on_varvalues_itemSelectionChanged ()
{
    if(!ui->varvalues->selectedItems().size())
        ui->vardelvalue->setEnabled(false);
    else
        ui->vardelvalue->setEnabled(true);
}

void MainWindow::on_ovarofun_currentIndexChanged(QString name)
{
    if(name=="")
    {
        ui->ovaradvanced->setEnabled(false);
        return;
    }

    if(!CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getSettingTypes(name.toStdString()).size())
        ui->ovaradvanced->setEnabled(false);
    else
        ui->ovaradvanced->setEnabled(true);

    QListWidgetItem *selecteditem = ui->vars->currentItem();
    ObjectiveFunctionVariable* ovar = static_cast<ObjectiveFunctionVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(selecteditem->text().toStdString()));
    IObjectiveFunction *fun = CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getFunction(name.toStdString());
    ovar->setObjectiveFunction(name.toStdString(), fun->getParameterValues());
    on_vars_itemClicked ( selecteditem );
    delete fun;
}

void MainWindow::on_ovardelmember_clicked()
{
    QListWidgetItem *selecteditem = ui->vars->currentItem();
    ObjectiveFunctionVariable* ovar = static_cast<ObjectiveFunctionVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(selecteditem->text().toStdString()));
    QList<QListWidgetItem *> list = ui->ovarmembers->selectedItems ();
    for(int index=0; index < list.size(); index++ )
    {
        bool ok = ovar->removeParameter(list.at(index)->text().toStdString());
        if(ok)
            delete list.at(index);
    }
    on_vars_itemClicked ( selecteditem );
}

void MainWindow::on_ovaraddmember_clicked()
{
    QListWidgetItem *selecteditem = ui->vars->currentItem();
    ObjectiveFunctionVariable* ovar = static_cast<ObjectiveFunctionVariable*>(CalibrationEnv::getInstance()->getCalibration()->getDomain()->getPar(selecteditem->text().toStdString()));
    bool ok;
    QStringList items;

    //iteration parameters
    vector<Variable*> available = CalibrationEnv::getInstance()->getCalibration()->getDomain()->getAllPars(ITERATIONVARIABLE);
    BOOST_FOREACH(Variable* var, available)
        if(!ovar->containsParameter(var->getName()))
            items.push_back(QString::fromStdString(var->getName()));

    //observed parameters
    available = CalibrationEnv::getInstance()->getCalibration()->getDomain()->getAllPars(OBSERVEDVARIABLE);
    BOOST_FOREACH(Variable* var, available)
        if(!ovar->containsParameter(var->getName()))
            items.push_back(QString::fromStdString(var->getName()));

    //objective function parameters
    available = CalibrationEnv::getInstance()->getCalibration()->getDomain()->getAllPars(OBJECTIVEFUNCTIONVARIABLE);
    BOOST_FOREACH(Variable* var, available)
        if(!ovar->containsParameter(var->getName()) && ovar->parameterCycleCheck(var->getName()))
            items.push_back(QString::fromStdString(var->getName()));

    QString text = QInputDialog::getItem ( this, "Members", "Available", items, 0, false, &ok);

    if (!ok || text.isEmpty())
        return;

    ok = ovar->addParameter(text.toStdString());
    if(ok)
        ui->ovarmembers->addItem(text);

     on_vars_itemClicked ( selecteditem );
}

void MainWindow::on_ovarmembers_itemSelectionChanged()
{
    if(!ui->ovarmembers->selectedItems().size())
        ui->ovardelmember->setEnabled(false);
    else
        ui->ovardelmember->setEnabled(true);
}

void MainWindow::on_newtemplate_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Template name"),
                                         tr("Name:"), QLineEdit::Normal,
                                         "", &ok);

    Calibration *calibration = CalibrationEnv::getInstance()->getCalibration();

    if (ok && !text.isEmpty())
    {
        switch(ui->comboBox_templates->currentIndex())
        {
        case 0:
            ok = CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->registerTemplate(text.toStdString(),"","",calibration,CALIBRATIONVARIABLE);
            if(ok)
                ui->templates->addItem(text);
            break;
        case 1:
            ok = CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->registerTemplate(text.toStdString(),"","",calibration, ITERATIONVARIABLE);
            if(ok)
                ui->templates->addItem(text);
            break;
        case 2:
            ok = CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->registerTemplate(text.toStdString(),"","",calibration, OBSERVEDVARIABLE);
            if(ok)
                ui->templates->addItem(text);
            break;
        }
    }
}

void MainWindow::on_templates_itemClicked ( QListWidgetItem * item )
{
    QString templatestring = QString::fromStdString(CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->getTemplate(item->text().toStdString()));
    ui->templateeditor->setPlainText(templatestring);
    ui->group_editor->setEnabled(true);
}

void MainWindow::on_deltemplate_clicked()
{
    QList<QListWidgetItem *> list = ui->templates->selectedItems ();
    for(int index=0; index < list.size(); index++ )
    {
        bool ok = CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->deleteTemplate(list[index]->text().toStdString());
        if(ok)
            delete list.at(index);
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Export template"), QDir::homePath());

    if(filename.isEmpty())
        return;

    QFile outputfile(filename);
    if(!outputfile.open(QIODevice::WriteOnly))
    {
        Logger(Debug) << "cannot create file";
        return;
    }

    QTextStream outputstream(&outputfile);
    outputstream << ui->templateeditor->toPlainText();
    outputfile.close();
}

void MainWindow::on_button_settemplatepath_clicked()
{
    QList<QListWidgetItem *> list = ui->templates->selectedItems ();
    if(!list.size())
        return;

    QListWidgetItem *item = list[0];
    QString currentpath = QString::fromStdString(CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->getPath(item->text().toStdString()));
    QString filename = QFileDialog::getSaveFileName(this, tr("Export template"), currentpath);

    if(filename.isEmpty())
        return;

    CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->updatePath(item->text().toStdString(),filename.toStdString());
}

void MainWindow::on_templateeditor_templatechanged(QString newtemplate, QString oldtemplate)
{
    QList<QListWidgetItem *> list = ui->templates->selectedItems ();
    if(!list.size())
        return;

    QListWidgetItem *item = list[0];
    bool ok = CalibrationEnv::getInstance()->getCalibration()->getExternalParameterRegistry()->updateTemplate(item->text().toStdString(),newtemplate.toStdString(),CalibrationEnv::getInstance()->getCalibration());

    if(!ok)
    {
        ui->templateeditor->setPlainText(oldtemplate);
        QMessageBox::warning(this,tr("Template error"),tr("Please check template for duplicates"));
        return;
    }

    //update gui
    on_comboBox_currentIndexChanged (ui->comboBox->currentIndex());
}

void MainWindow::on_button_load_values_clicked()
{
    QList<QListWidgetItem *> list = ui->templates->selectedItems ();
    if(!list.size())
        return;

    QListWidgetItem *item = list[0];

    QString fileName = QFileDialog::getOpenFileName(this,tr("Value file"), QDir::homePath(), tr("*.*"));
    if(!QFile::exists(fileName))
        return;

     QFile inputfile(fileName);

     if (!inputfile.open(QIODevice::ReadOnly))
          return;

     Calibration *calibration = CalibrationEnv::getInstance()->getCalibration();
     QString valuefile = "";
     QTextStream inputstream(&inputfile);
     valuefile+=inputstream.readAll();
     inputfile.close();

     if(!calibration->getExternalParameterRegistry()->updateParameters(calibration->getDomain(),item->text().toStdString(),valuefile.toStdString()))
     {
        QMessageBox::warning(this,tr("File error"),tr("Template not compatible"));
        return;
     }

     QList<QListWidgetItem *> varlist = ui->vars->selectedItems ();

     if(varlist.size() > 0)
        on_vars_itemClicked ( varlist[0] );
}

void MainWindow::on_groups_itemSelectionChanged()
{
    if(!ui->groups->selectedItems().size())
        ui->del_group->setEnabled(false);
    else
        ui->del_group->setEnabled(true);
}

void MainWindow::on_del_group_clicked()
{
    QList<QListWidgetItem *> varlist = ui->vars->selectedItems ();
    if(!varlist.size())
        return;

    QListWidgetItem* var = varlist[0];
    QList<QListWidgetItem *> list = ui->groups->selectedItems ();
    Calibration* calibration = CalibrationEnv::getInstance()->getCalibration();
    for(int index=0; index < list.size(); index++ )
    {
        bool ok = calibration->removeParameterFromGroup(var->text().toStdString(),list[index]->text().toStdString());
        if(ok)
            delete list.at(index);
        else
            QMessageBox::warning(this,tr("Error"),tr("Parameter must be a member of group: ") + list[index]->text());
    }
}

void MainWindow::on_add_group_clicked()
{
    QListWidgetItem *selecteditem = ui->vars->currentItem();
    Calibration *calibration = CalibrationEnv::getInstance()->getCalibration();
    CalibrationVariable* calvar = static_cast<CalibrationVariable*>(calibration->getDomain()->getPar(selecteditem->text().toStdString()));
    bool ok;
    QStringList items;

    //iteration parameters
    vector<string> available = calibration->getAllGroups();
    BOOST_FOREACH(string groupname, available)
        if(!calibration->containsGroupMember(calvar->getName(),groupname))
            items.push_back(QString::fromStdString(groupname));

    QString text = QInputDialog::getItem ( this, "Groups", "Available", items, 0, false, &ok);

    if (!ok || text.isEmpty())
        return;

    ok = calibration->addParameterToGroup(calvar->getName(),text.toStdString());
    if(ok)
        ui->groups->addItem(text);

     on_vars_itemClicked ( selecteditem );
}

void MainWindow::on_button_groupmanage_clicked()
{
    if(GroupManager::editGroups(CalibrationEnv::getInstance()->getCalibration(),this))
        if(ui->vars->selectedItems().size() > 0)
        {
            QListWidgetItem *item = ui->vars->currentItem();
            on_vars_itemClicked(item);
        }
}
