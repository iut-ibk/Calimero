#ifndef PARAMETERGROUPEDITGUI_H_INCLUDED
#define PARAMETERGROUPEDITGUI_H_INCLUDED

#include "../ui_inputparameteredit.h"
#include "ParameterChooserGui.h"

class Model;
class ParameterChooserGui;

using namespace Ui;

class ParameterGroupEditGui : public QWidget, public InputParameterEditWidget
{
    Q_OBJECT

    private:
        Model *model;
        QString currentgroup;
        QString currentparameter;
        ParameterChooserGui *parameterchooser;


    public:
        ParameterGroupEditGui(QWidget *parent = 0);
        ~ParameterGroupEditGui();

    public slots:
        void addParameterToGroup();
        void addSelectedParameters();
        void removeParameterFromGroup();
        void newGroup();
        void deleteGroup();
        void currentParameterChange( const QString & currentText );
        void currentGroupChange( const QString & currentText );
        void update(Model* m);
        void parameterChooserRejected();
        void on_lineEdit_max_valueChanged(double value);
        void on_lineEdit_min_valueChanged(double value );
        void on_lineEdit_initvalue_editingFinished();

    protected:
        virtual void changeEvent(QEvent *e);
};

#endif // PARAMETERGROUPEDITGUI_H_INCLUDED
