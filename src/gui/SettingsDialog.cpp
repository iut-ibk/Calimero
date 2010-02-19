#include "SettingsDialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include "../core/Language.h"
#include "Gui.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QSettings settings;
    this->lineEditScriptPath->setText(settings.value("scriptpath","../testdaten/script/").toString());
    this->comboBox_language->setCurrentIndex(settings.value("language",0).toInt());
    settings.sync();
}

void SettingsDialog::on_b_scriptpath_clicked()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "./",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(checkScriptPath(dirname))
    {
        this->lineEditScriptPath->setText(dirname);
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("This directory is not a valid Calimero Home directory"));
    }

}

void SettingsDialog::on_bb_save_accepted()
{
    if(!checkScriptPath(this->lineEditScriptPath->text()))
    {
        QMessageBox::critical(this,tr("Error"),tr("Settings not valid"));
        return;
    }

    QSettings settings;
    settings.setValue("scriptpath",this->lineEditScriptPath->text());
    settings.setValue("language",this->comboBox_language->currentIndex());

    settings.sync();
    ScriptLibary::getInstance()->update();
    LanguageSelector::setLanguage(settings.value("language",0).toInt(),static_cast<Gui*>(this->parent())->translator);
    this->accept();
}

void SettingsDialog::on_bb_save_rejected()
{
    if(!checkScriptPath(this->lineEditScriptPath->text()))
    {
        QMessageBox::critical(this,tr("Error"),tr("Settings not valid"));
        return;
    }

    this->reject();
}

void SettingsDialog::changeEvent(QEvent *e)
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

bool SettingsDialog::checkScriptPath(QString dirname)
{
    if(dirname!="")
    {
        QDir dir(dirname);

        if(dir.exists("comparescripts") && dir.exists("calibrationscripts") && dir.exists("language"))
        {
            return true;
        }
    }
    return false;
}
