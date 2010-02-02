#ifndef PARAMETERGUI_H_INCLUDED
#define PARAMETERGUI_H_INCLUDED

#include <QtCore>
#include <QtGui>
#include <QAction>
#include "../ui_parameteredit.h"
#include "../core/Model.h"

using namespace Ui;

class ParameterEditGui : public QWidget, public ParameterEditWidget
 {
    Q_OBJECT

    public:
        Model *model;
        int position;
        QString currentfilename;
        bool loadnewtemplate;

        ParameterEditGui(QWidget *parent = 0);
        ~ParameterEditGui();


    public slots:
        void changeCurrentFile( const QString & text );
        void changeCurrentParameterType();
        void importparameter();
        void newparameter(QVector<double> values, QString name);
        void deleteparameter();
        void update(Model* m);
        void jumptoparameter( QListWidgetItem * item);
        void on_b_editpath_clicked();
        void on_b_newtemplate_clicked();
        void on_b_deletetemplate_clicked();

        void on_line_iterationexec_w_returnPressed();
        void on_line_iterationexec_returnPressed();
        void on_line_iterationexecargument_returnPressed();
        void on_b_exec_w_clicked();
        void on_b_exec_clicked();
        void update()
        {
            update(model);
        }

     protected:
        virtual void changeEvent(QEvent *e);
};


#endif // PARAMETERGUI_H_INCLUDED
