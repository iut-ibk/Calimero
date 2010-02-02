#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "../ui_mainwindow.h"
#include "../core/Model.h"

using namespace Ui;

class Gui : public QMainWindow, public MainWindow
{
        Q_OBJECT
    private:
        Model *model;


    public:
        QTranslator *translator;
        Gui(QWidget* parent = 0, Qt::WindowFlags flags = 0);
        virtual ~Gui();

        void update(Model* m)
        {
            qDebug("Gui::update()");

            if(model!=m)
            {
                model=m;
            }

            if(model==NULL)
                return;

            if(tab_template->isVisible())
                widget_4->update(model);

            if(widget->isVisible())
                widget->update(model);

            if(widget_2->isVisible())
                widget_2->update(model);

            if(widget_3->isVisible())
                widget_3->update(model);

            diagram_widget->update(model);
        }

    public slots:
        void update()
        {
            QMainWindow::update();
            qDebug() << "Gui::update()";
            if(model==NULL)
                return;

            update(model);
        }
    protected:
        virtual void changeEvent(QEvent *e);
};

#endif // MAINWINDOW_H_INCLUDED
