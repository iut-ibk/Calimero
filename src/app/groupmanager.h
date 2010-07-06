#ifndef GROUPDIALOGMANAGER_H
#define GROUPDIALOGMANAGER_H

#include <QDialog>
#include <QObject>

namespace Ui {
        class GroupManager;
}

class Calibration;

class GroupManager : public QDialog
{
    Q_OBJECT

private:
    Ui::GroupManager *ui;
    Calibration *calibration;
    static GroupManager* instance;
    GroupManager(Calibration *calibration, QWidget *parent = 0);
    bool changed;

public:
    static bool editGroups(Calibration *calibration, QWidget *parent = 0);

public Q_SLOTS:
    void on_groups_itemSelectionChanged();
    void on_button_ok_clicked();
    void on_new_group_clicked();
    void on_del_group_clicked();
};

#endif // GROUPDIALOGMANAGER_H
