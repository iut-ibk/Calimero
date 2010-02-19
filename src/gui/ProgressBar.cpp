#include "ProgressBar.h"

ProgressBar::ProgressBar(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
}

void ProgressBar::setRange(int min, int max)
{
    progressBar->setMinimum(min);
    progressBar->setMaximum(max);
    maximum=max;
}

void ProgressBar::setProgress(int progress)
{
    if(maximum>100)
    {
        progressBar->setValue(progress);
        label->setText(QString(tr("Please wait: ")) + QString::number(progress) + QString(tr(" of ")) + QString::number(maximum));
    }
    else
    {
        progressBar->setValue(progress);
        label->setText(QString(tr("Please wait:")));
    }
    QCoreApplication::processEvents();
}

void ProgressBar::changeEvent(QEvent *e)
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
