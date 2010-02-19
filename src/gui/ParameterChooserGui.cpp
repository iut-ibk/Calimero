#include "ParameterChooserGui.h"

ParameterChooserGui::ParameterChooserGui(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

}

ParameterChooserGui::~ParameterChooserGui()
{
}

void ParameterChooserGui::changeEvent(QEvent *e)
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
