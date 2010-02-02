#include "Controller.h"
#include "../core/Model.h"
#include "../persistence/Persistence.h"
#include <QtDebug>
#include "../gui/SettingsDialog.h"

Controller::Controller(CoreEngine* e, int updateinterval)
{
    gui = new Gui(0, Qt::Window);

    engine = e;
    needupdate=true;
    scriptimport=false;
    calibrationrunning=false;
    forcenoupdate=false;
    updateenabled=true;

    if(updateinterval<1000)
        updateinterval=1000;

    interval = updateinterval;

    QObject::connect(engine, SIGNAL(erroroccured(QString)), this, SLOT(warningbox(QString)));
    QObject::connect(engine, SIGNAL(modelchanged(Model*)), this, SLOT(update(Model*)));
    QObject::connect(engine, SIGNAL(sysInfo(QString)), this, SLOT(sysInfo(QString)));
    QObject::connect(gui->actionnew, SIGNAL(activated()), this, SLOT(newProject()));
    QObject::connect(gui->actionsave, SIGNAL(activated()), this, SLOT(save()));
    QObject::connect(gui->actionsaveas, SIGNAL(activated()), this, SLOT(saveas()));
    QObject::connect(gui->actionRun_in_Background, SIGNAL(activated()), this, SLOT(runBackground()));
    QObject::connect(gui->actionopen, SIGNAL(activated()), this, SLOT(open()));
    QObject::connect(gui->pushButton, SIGNAL(clicked()), this, SLOT(buttonvisible()),Qt::QueuedConnection);
    QObject::connect(gui->pushButton, SIGNAL(clicked()), engine, SLOT(startStopCalibration()),Qt::QueuedConnection);
    QObject::connect(gui->actionsettings, SIGNAL(activated()),this,SLOT(settings()));
    QObject::connect(gui->actionOpen_with_script_import, SIGNAL(activated()), this, SLOT(openWithScript()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(makeupdate()));
    timer->start(updateinterval);
    timer->start();

    gui->show();
    makeupdate();

    QSettings settings;
    if(!SettingsDialog::checkScriptPath(settings.value("scriptpath","").toString()))
    {
        warningbox(tr("This directory is not a valid Calimero Home directory"));
        this->settings();
    }
}

Controller::~Controller()
{
    timer->stop();
    QObject::disconnect(engine, SIGNAL(erroroccured(QString)), this, SLOT(warningbox(QString)));
    QObject::disconnect(engine, SIGNAL(modelchanged(Model*)), this, SLOT(update(Model*)));
    QObject::disconnect(engine, SIGNAL(sysInfo(QString)), this, SLOT(sysInfo(QString)));
    QObject::disconnect(gui->actionnew, SIGNAL(activated()), this, SLOT(newProject()));
    QObject::disconnect(gui->actionsave, SIGNAL(activated()), this, SLOT(save()));
    QObject::disconnect(gui->actionsaveas, SIGNAL(activated()), this, SLOT(saveas()));
    QObject::disconnect(gui->actionopen, SIGNAL(activated()), this, SLOT(open()));
    QObject::disconnect(gui->pushButton, SIGNAL(clicked()), engine, SLOT(startStopCalibration()));
    QObject::disconnect(gui->pushButton, SIGNAL(clicked()), this, SLOT(buttonvisible()));
    QObject::disconnect(gui->actionsettings, SIGNAL(activated()),this,SLOT(settings()));
    QObject::disconnect(gui->actionRun_in_Background, SIGNAL(activated()), this, SLOT(runBackground()));
    QObject::disconnect(gui->actionOpen_with_script_import, SIGNAL(activated()), this, SLOT(openWithScript()));
    timer->stop();
    delete timer;
    delete gui;
}

void Controller::update(Model* )
{
    needupdate=true;
}

void Controller::warningbox(QString w)
{
    QMessageBox::warning(gui,tr("Warning"),w);
}


void Controller::saveas()
{
    QString filename = QFileDialog::getSaveFileName(gui,tr("Save file"), "./", tr("*.cmp"));

    if(filename!="")
    {
        if(!filename.contains(".cmp"))
            filename+=".cmp";
        engine->save(filename);
    }
}

void Controller::save()
{
    if(engine->getPersistenceFileName()=="")
    {
        saveas();
    }
    else
    {
        engine->save();
    }
}

void Controller::open()
{
    bool script = scriptimport;
    scriptimport = false;
    QString filename = QFileDialog::getOpenFileName(gui,tr("Open file"), "./", tr("*.cmp"));
    if(filename!="")
    {
        gui->centralwidget->setEnabled(false);
        updateenabled=false;
        engine->getModel()->clear();
        updateenabled=true;
        makeupdate();
        updateenabled=false;

        if(engine->load(filename,script))
        {
            updateenabled=true;
            forcenoupdate=true;
            forcenoupdate=false;
            gui->centralwidget->setEnabled(true);
        }
        updateenabled=true;
        gui->centralwidget->setEnabled(true);
    }
}

void Controller::settings()
{
    SettingsDialog* settings = new SettingsDialog(gui);
    settings->show();
}

void Controller::newProject()
{
    QString filename = QFileDialog::getSaveFileName(gui,tr("Save file"), "./", "*.cmp");

    if(filename!="")
    {
        if(!filename.contains(".cmp"))
            filename+=".cmp";
        engine->getModel()->clear();
        engine->save(filename);
    }
}

void Controller::makeupdate()
{
    if(!gui->isVisible())
        return;

    if(!updateenabled)
        return;

    QMutexLocker locker(&mutex);
    if(!needupdate||forcenoupdate)
        return;

    if(engine->getModel()==NULL)
        return;

    timer->stop();
    needupdate=false;

    if(gui->isVisible())
        gui->update(engine->getModel());

    gui->actionnew->setEnabled(true);
    gui->actionsave->setEnabled(true);
    gui->actionsaveas->setEnabled(true);


    if(engine->calibrationRunning())
    {
        timer->setInterval(interval*2);
        gui->pushButton->setText(tr("Stop calibration"));
        gui->tab->setEnabled(false);
        gui->tab2->setEnabled(false);
        gui->tab3->setEnabled(false);
        gui->tab_template->setEnabled(false);
        gui->diagram_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gui->diagram_widget->enableFollow(true);
        gui->tab_calibrationedit->setCurrentWidget(gui->tab4);
        gui->tab_calibrationedit->setEnabled(false);
        gui->actionnew->setEnabled(false);
        gui->actionopen->setEnabled(false);
        gui->actionOpen_with_script_import->setEnabled(false);
        gui->actionsave->setEnabled(false);
        gui->actionsaveas->setEnabled(false);
        gui->actionsettings->setEnabled(false);
        gui->pushButton->setEnabled(true);
    }
    else
    {
        timer->setInterval(interval);
        gui->pushButton->setText(tr("Start calibration"));
        gui->tab->setEnabled(true);
        gui->tab2->setEnabled(true);
        gui->tab3->setEnabled(true);
        gui->tab_template->setEnabled(true);
        gui->diagram_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        gui->diagram_widget->enableFollow(false);
        gui->tab_calibrationedit->setEnabled(true);
        gui->actionnew->setEnabled(true);
        gui->actionopen->setEnabled(true);
        gui->actionOpen_with_script_import->setEnabled(true);
        gui->actionsave->setEnabled(true);
        gui->actionsaveas->setEnabled(true);
        gui->actionsettings->setEnabled(true);
        gui->pushButton->setEnabled(true);
    }

    timer->start();
}

void Controller::sysInfo(QString info)
{
    gui->statusBar()->showMessage(info);
}

void Controller::runBackground()
{
    engine->enabledGui(false);

   if(engine->calibrationRunning())
        engine->startStopCalibration();

   QApplication::processEvents();
   QApplication::exit();
}

void Controller::openWithScript()
{
    scriptimport=true;
    open();
}

void Controller::buttonvisible()
{
    gui->pushButton->setEnabled(false);
}
