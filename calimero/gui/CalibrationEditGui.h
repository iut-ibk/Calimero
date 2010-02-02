#ifndef CALIBRATIONEDITGUI_H_INCLUDED
#define CALIBRATIONEDITGUI_H_INCLUDED

#include "../ui_calibrationedit.h"
#include "../core/Model.h"
#include "../qtscript/ScriptLibary.h"
#include "./qtscript/scripteditwidget.h"

#include <QtGui>

using namespace Ui;

class CalibrationEditGui : public QWidget, public CalibrationEdit
{
    Q_OBJECT

    private:
        Model *model;
        ::ScriptEditWidget *scriptedit;


    public:
        CalibrationEditGui(QWidget *parent = 0);
        ~CalibrationEditGui();

    public slots:
        void selectionChanged();
        void plusGroup( QListWidgetItem * item );
        void minusGroup( QListWidgetItem * item );
        void update(Model *m);
        void textEdited ( const QString & text );
        void newScript();
        void editScript();
        void setScript(const QString & text);
        void checkBoxStateChanged ();
        void calulationparameter(QListWidgetItem * item);
        void on_lineEdit_max_error_returnPressed();
        void on_line_preexec_returnPressed();
        void on_line_preexecargument_returnPressed();
        void on_line_preexec_w_returnPressed();
        void on_b_pre_w_clicked();
        void on_b_pre_clicked();
        void on_line_iterationexec_w_returnPressed();
        void on_line_iterationexec_returnPressed();
        void on_line_iterationexecargument_returnPressed();
        void on_b_exec_w_clicked();
        void on_b_exec_clicked();
        void on_b_post_w_clicked();
        void on_line_postexec_w_returnPressed();
        void on_b_post_clicked();
        void on_line_postexec_returnPressed();
        void on_line_postexecargument_returnPressed();
        void on_spinBox_cpus_valueChanged ( int i );

    protected:
        virtual void changeEvent(QEvent *e);
};
#endif // CALIBRATIONEDITGUI_H_INCLUDED
