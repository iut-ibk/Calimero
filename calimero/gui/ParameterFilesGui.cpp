#include "ParameterFilesGui.h"


ParameterFilesGui::ParameterFilesGui(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
}

ParameterFilesGui::~ParameterFilesGui()
{
}



void ParameterFilesGui::update(Model *m)
{
    qDebug("ParameterFilesGui::update()");
    if(model!=m)
    {
        model=m;
    }

    listWidget_input->clear();
    for(int counter=0; counter<model->numberOfInputFiles(); counter++)
        listWidget_input->addItem(model->getInputFileName(counter));

    listWidget_iteration->clear();
    for(int counter=0; counter<model->numberOfIterationFiles(); counter++)
        listWidget_iteration->addItem(model->getIterationFileName(counter));

    listWidget_result->clear();
    for(int counter=0; counter<model->numberOfResultFiles(); counter++)
        listWidget_result->addItem(model->getResultFileName(counter));
}

void ParameterFilesGui::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
