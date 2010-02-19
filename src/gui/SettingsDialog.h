#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"

class SettingsDialog : public QDialog, private Ui::Settings {
    Q_OBJECT
    Q_DISABLE_COPY(SettingsDialog)
public:
    explicit SettingsDialog(QWidget *parent = 0);
    static bool checkScriptPath(QString dir);

public slots:
    void on_b_scriptpath_clicked();
    void on_bb_save_accepted();
    void on_bb_save_rejected();

protected:
    virtual void changeEvent(QEvent *e);
};

#endif // SETTINGSDIALOG_H
