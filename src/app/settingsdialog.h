#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
        class SettingsDialog;
}


class SettingsDialog : public QDialog
{
Q_OBJECT
private:
    Ui::SettingsDialog *ui;

public:
    SettingsDialog(QWidget *parent = 0);
    virtual ~SettingsDialog();

public Q_SLOTS:
    void on_add_path_clicked();
    void on_del_path_clicked();
    void on_paths_itemSelectionChanged ();
    void on_b_save_rejected();
    void on_b_save_accepted();
};

#endif // SETTINGSDIALOG_H
