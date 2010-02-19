#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "../core/CoreEngine.h"
#include "Gui.h"
#include "ParameterFilesGui.h"
#include "ParameterEditGui.h"
#include <QtGui>
#include <QMutex>

class UpdateTimer;

class Controller: public QObject
{
    Q_OBJECT
    private:
        Gui *gui;
        QMutex mutex;
        CoreEngine *engine;
        bool needupdate;
        bool calibrationrunning;
        bool forcenoupdate;
        bool scriptimport;
        bool updateenabled;
        int interval;
        QTimer *timer;



    public:
        Controller(CoreEngine* e,int updateinterval);
        ~Controller();

    public slots:
        void newProject();
        void open();
        void openWithScript();
        void runBackground();
        void save();
        void settings();
        void saveas();
        void warningbox(QString w);
        void update(Model* m);
        void makeupdate();
        void buttonvisible();
        void sysInfo(QString info);
};

#endif // CONTROLLER_H_INCLUDED
