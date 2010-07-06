#include <groupmanager.h>
#include <ui_groupmanager.h>
#include <Calibration.h>
#include <QMessageBox>
#include <QInputDialog>


GroupManager* GroupManager::instance = 0;

GroupManager::GroupManager(Calibration *calibration, QWidget *parent) :QDialog(parent), ui(new Ui::GroupManager)
{
    ui->setupUi(this);

}

void GroupManager::on_del_group_clicked()
{
    QList<QListWidgetItem *> grouplist = ui->groups->selectedItems ();
    if(!grouplist.size())
        return;

    for(int index=0; index < grouplist.size(); index++ )
    {
        bool ok = calibration->removeGroup(grouplist[index]->text().toStdString());
        if(ok)
        {
            changed=true;
            delete grouplist.at(index);
        }
        else
            QMessageBox::warning(this,tr("Error"),tr("Not able to delete group: ") + grouplist[index]->text());
    }
}

void GroupManager::on_new_group_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Group name"),tr("Name:"), QLineEdit::Normal,"", &ok);

    if (ok && !text.isEmpty())
    {
        ok= calibration->addGroup(text.toStdString());
        if(ok)
        {
            changed=true;
            ui->groups->addItem(text);
        }
        else
            QMessageBox::warning(this,tr("Error"),tr("Not able to create group: ") + text);
    }
}

void GroupManager::on_button_ok_clicked()
{
    setVisible(false);
}

void GroupManager::on_groups_itemSelectionChanged()
{
    if(!ui->groups->selectedItems().size())
        ui->del_group->setEnabled(false);
    else
        ui->del_group->setEnabled(true);
}

bool GroupManager::editGroups(Calibration *calibration, QWidget *parent)
{
    if(!instance)
        instance = new GroupManager(calibration,parent);

    if(instance->isVisible())
        return instance->changed;

    instance->changed=false;
    instance->calibration=calibration;
    instance->setVisible(true);

    instance->ui->groups->clear();
    instance->ui->del_group->setEnabled(false);
    vector<string> groupnames = instance->calibration->getAllGroups();
    BOOST_FOREACH(string name, groupnames)
            instance->ui->groups->addItem(QString::fromStdString(name));

    instance->exec();

    return instance->changed;
}
