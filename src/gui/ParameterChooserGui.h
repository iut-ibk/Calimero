#ifndef PARAMETERCHOOSERGUI_H_INCLUDED
#define PARAMETERCHOOSERGUI_H_INCLUDED

#include "../ui_parameterchooser.h"

using namespace Ui;

class ParameterChooserGui : public QDialog, public ParameterChooser
{
    Q_OBJECT
    
    public:
        ParameterChooserGui(QWidget *parent = 0);
        ~ParameterChooserGui();

    protected:
        virtual void changeEvent(QEvent *e);
};
#endif // PARAMETERCHOOSERGUI_H_INCLUDED
