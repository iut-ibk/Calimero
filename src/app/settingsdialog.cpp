#include <ui_settingsdialog.h>
#include <settingsdialog.h>
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>


SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    ui = new Ui::SettingsDialog();
    ui->setupUi(this);
    QSettings settings;
    ui->path->setText(settings.value("calimerohome").toString());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_b_save_accepted()
{
    QSettings settings;

    //save calimero home
    QFileInfo info(ui->path->text());
    if(info.dir().exists())
        settings.setValue("calimerohome",ui->path->text());
    QMessageBox::information(this, tr("Calimero options..."), tr("Please restart calimero."));
    this->close();
}

void SettingsDialog::on_b_save_rejected()
{
    this->close();
}

void SettingsDialog::on_b_path_clicked()
{
    QFileInfo info(ui->path->text());
    QString path = QDir::current().path();
    if (info.dir().exists())
            path = info.dir().path();

    QString name = QFileDialog::getExistingDirectory(this,tr("Choose Directory"),path);

    if (!name.isEmpty())
        ui->path->setText(name);
}
