#include <ui_settingsdialog.h>
#include <settingsdialog.h>
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>


SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    ui = new Ui::SettingsDialog();
    ui->setupUi(this);
    ui->del_path->setEnabled(false);
    QSettings settings;
    QStringList pathlist = settings.value("calimerohome","").toString().replace("\\","/").split(",");

    for (int index = 0; index < pathlist.size(); index++)
        ui->paths->addItem(pathlist.at(index));

    ui->memory->setChecked(settings.value("inmemory",true).toBool());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_b_save_accepted()
{
    QSettings settings;

    //save calimero paths
    QStringList pathstringlist;
    for(int index=0; index < ui->paths->count(); index++ )
        pathstringlist.append(ui->paths->item(index)->text());


    settings.setValue("calimerohome",pathstringlist.join(","));

    settings.setValue("inmemory",ui->memory->isChecked());
    QMessageBox::information(this, tr("Calimero options..."), tr("Please restart calimero."));
    this->close();
}

void SettingsDialog::on_b_save_rejected()
{
    this->close();
}

void SettingsDialog::on_add_path_clicked()
{
    QString path = QDir::current().path();

    QString name = QFileDialog::getExistingDirectory(this,tr("Choose Directory"),path);

    if (!name.isEmpty())
    {
        QFileInfo info(name);
        if(info.dir().exists())
            ui->paths->addItem(name);
    }
}

void SettingsDialog::on_del_path_clicked()
{
    QList<QListWidgetItem *> list = ui->paths->selectedItems ();
    for(int index=0; index < list.size(); index++ )
        delete list.at(index);
}

void SettingsDialog::on_paths_itemSelectionChanged ()
{
    if(!ui->paths->selectedItems().size())
        ui->del_path->setEnabled(false);
    else
        ui->del_path->setEnabled(true);
}
