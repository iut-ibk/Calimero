/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
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
    ui->updatediagram->setChecked(settings.value("updatediagram",true).toBool());
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
    settings.setValue("updatediagram",ui->updatediagram->isChecked());
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
