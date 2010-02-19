#ifndef PROPERTIESGUI_H_INCLUDED
#define PROPERTIESGUI_H_INCLUDED

#include <QFileDialog>
#include "../core/Model.h"
#include <ui_parameterfiles.h>

using namespace Ui;

class ParameterFilesGui : public QDialog, public ParameterFilesDialog
 {
     Q_OBJECT

    public:
        Model *model;

        ParameterFilesGui(QWidget *parent = 0);
        ~ParameterFilesGui();

    public slots:
        void update(Model *m);
        void update()
        {
            update(model);
        }

    protected:
        virtual void changeEvent(QEvent *e);
};


#endif // PROPERTIESGUI_H_INCLUDED
