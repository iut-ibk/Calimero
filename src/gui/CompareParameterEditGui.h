#ifndef COMPAREPARAMETEREDITGUI_H_INCLUDED
#define COMPAREPARAMETEREDITGUI_H_INCLUDED

#include "../ui_compareparameteredit.h"
#include "ParameterChooserGui.h"
#include "../core/Model.h"
#include "../qtscript/scripteditwidget.h"

class CompareParameterEditGui : public QWidget, public CompareParameterEditWidget
{
    Q_OBJECT

    private:


        ::ScriptEditWidget *scriptedit;
        Model *model;
        QString currentparametername;
        CalculationVariable *currentparameter;
        ParameterChooserGui *comparechooser;
        ParameterChooserGui *resultchooser;
        ParameterChooserGui *iterationchooser;

    public:
        CompareParameterEditGui(QWidget *parent = 0);
        ~CompareParameterEditGui();



    public slots:
        void newCompareParameter();
        void deleteCompareParameter();

        //parameterchooser slots
        void compareParameterOk();
        void iterationParameterOk();
        void resultParameterOk();
        void parameterChooserRejected();

        //removeparameters
        void removeCompareParameter();
        void removeResultParameter();
        void removeIterationParameter();

        //addparameters
        void addCompareParameter();
        void addResultParameter();
        void addIterationParameter();
        void newScript();
        void editScript();
        void selectionChanged( QListWidgetItem * item);
        void update(Model *m);
        void setScript(const QString & text);

    protected:
        virtual void changeEvent(QEvent *e);

};

#endif // COMPAREPARAMETEREDITGUI_H_INCLUDED
