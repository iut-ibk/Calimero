#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "ui_progressbar.h"

class ProgressBar : public QDialog, private Ui::ProgressBar {
    Q_OBJECT
    Q_DISABLE_COPY(ProgressBar)

public:
    double maximum;

public:
    explicit ProgressBar(QWidget *parent = 0);
    virtual void setProgress(int progress);
    virtual void setRange(int min, int max);

protected:
    virtual void changeEvent(QEvent *e);
};

#endif // PROGRESSBAR_H
