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
