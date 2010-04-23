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



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
	ui->setupUi(this);;
        setupStateMachine();

        log_updater = new GuiLogSink();
        Log::init(log_updater);
        ui->log_widget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::setupStateMachine() {
        state_machine = new QStateMachine();

        //init
        QState *init = new QState();
        QObject::connect(init,SIGNAL(entered()),this,SLOT(init()));


        //Parameter tab
        QState *tab_states = new QState(QState::ParallelStates);

            //variable group
            QState *var_group = new QState(tab_states);

                //Variable properties
                QState *var_prop = new QState(var_group);
                var_prop->assignProperty(ui->var_widget, "visible", true);
                var_prop->assignProperty(ui->calvar_widget, "visible", false);
                var_prop->assignProperty(ui->ovar_widget, "visible", false);
                var_group->setInitialState(var_prop);

                //Objective function variable properties
                QState *ovar_prop = new QState(var_group);
                ovar_prop->assignProperty(ui->var_widget, "visible", false);
                ovar_prop->assignProperty(ui->calvar_widget, "visible", false);
                ovar_prop->assignProperty(ui->ovar_widget, "visible", true);
                ovar_prop->assignProperty(ui->button_ofun_advanced, "enabled", false);

                //Objective function variable properties + advanced
                QState *ovar_prop_advanced = new QState(var_group);
                ovar_prop_advanced->assignProperty(ui->var_widget, "visible", false);
                ovar_prop_advanced->assignProperty(ui->calvar_widget, "visible", false);
                ovar_prop_advanced->assignProperty(ui->ovar_widget, "visible", true);
                ovar_prop_advanced->assignProperty(ui->button_ofun_advanced, "enabled", true);

                //Calibration variable properties
                QState *calvar_prop = new QState(var_group);
                calvar_prop->assignProperty(ui->var_widget, "visible", false);
                calvar_prop->assignProperty(ui->calvar_widget, "visible", true);
                calvar_prop->assignProperty(ui->ovar_widget, "visible", false);
                var_group->setInitialState(calvar_prop);

                //Choose ofun
                QState *choose_ofun = new QState(var_group);
                QObject::connect(choose_ofun,SIGNAL(entered()),this,SLOT(setOFunction()));


                var_prop->addTransition(this, SIGNAL(show_ovar()), ovar_prop);
                var_prop->addTransition(this, SIGNAL(show_calvar()), calvar_prop);
                ovar_prop->addTransition(this, SIGNAL(show_var()), var_prop);
                ovar_prop->addTransition(this, SIGNAL(show_calvar()), calvar_prop);
                calvar_prop->addTransition(this, SIGNAL(show_ovar()), ovar_prop);
                calvar_prop->addTransition(this, SIGNAL(show_var()), var_prop);

                ovar_prop->addTransition(ui->comboBox_ofun, SIGNAL(currentIndexChanged ( int )), choose_ofun);
                ovar_prop_advanced->addTransition(ui->comboBox_ofun, SIGNAL(currentIndexChanged ( int )), choose_ofun);
                choose_ofun->addTransition(this,SIGNAL(show_ovar()),ovar_prop);
                choose_ofun->addTransition(this,SIGNAL(show_ovar_advanced()),ovar_prop_advanced);
                ovar_prop_advanced->addTransition(this, SIGNAL(show_var()), var_prop);
                ovar_prop_advanced->addTransition(this, SIGNAL(show_calvar()), calvar_prop);

                init->addTransition(this, SIGNAL(start_gui()),tab_states);

        //start statemachine
        state_machine->addState(tab_states);
        state_machine->addState(init);
        state_machine->setInitialState(init);
        state_machine->start();
}

void MainWindow::init()
{
    Logger(Debug) << "init called";

    FunctionLoader::loadScripts("./");
    FunctionLoader::loadScripts("../../scripts/");
    FunctionLoader::loadNative("./");

    vector<string> ofunvec = CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getAvailableFunctions();

    QStringList ofunlist;
    BOOST_FOREACH(string name, ofunvec)
            ofunlist.append(QString::fromStdString(name));

    ui->comboBox_ofun->addItems(ofunlist);

    Q_EMIT start_gui();
}

void MainWindow::setOFunction()
{
    if(!CalibrationEnv::getInstance()->getObjectiveFunctionReg()->getSettingTypes(ui->comboBox_ofun->currentText().toStdString()).size())
    {
        Q_EMIT show_ovar();
    }
    else
    {
        Q_EMIT show_ovar_advanced();
    }
}

void MainWindow::on_comboBox_currentIndexChanged ( int index )
{
    switch(index)
    {
    case 0:
        show_calvar();
        break;
    case 1:
        show_var();
        break;
    case 2:
        show_var();
        break;
    case 3:
        show_ovar();
        break;
    }
}
