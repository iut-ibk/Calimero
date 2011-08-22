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
#ifndef RESULTANALYSISDIALOG_H
#define RESULTANALYSISDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
        class ResultAnalysisDialog;
}


class ResultAnalysisDialog : public QDialog
{
Q_OBJECT
private:
    Ui::ResultAnalysisDialog *ui;

public:
    ResultAnalysisDialog(QWidget *parent = 0);
    virtual ~ResultAnalysisDialog();
    void clear();
    void updateAll();

public Q_SLOTS:
    void on_handlerenabled_clicked(bool checked);
    void on_run_clicked();
    void on_handleradvanced_clicked();
    void on_newhandler_clicked();
    void on_delhandler_clicked();
    void on_run_all_clicked();
    void on_handlers_itemSelectionChanged();
};

#endif // RESULTANALYSISDIALOG_H
