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
