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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QListWidgetItem>
#include <QTimer>
#include <QCloseEvent>

#define DEFAULTUPDATETIME 1000

namespace Ui {
	class MainWindow;
}

class QStateMachine;
class Calibration;
class GuiLogSink;
class QTableWidgetItem;
class ResultAnalysisDialog;
class Persistence;

class MainWindow : public QMainWindow {
Q_OBJECT
    public:
        MainWindow(QWidget *parent = 0, QString load_file = "");
	virtual ~MainWindow();
        void setupStateMachine();
	Ui::MainWindow *ui;
        ResultAnalysisDialog *resultanalysis;
        QStateMachine *state_machine;
        GuiLogSink *log_updater;
        QTimer updatetimer;
        Persistence *persistence;
        QString savefilepath;
        bool loading;
        int sp;
        int ls;
        int UPDATETIME;

    public Q_SLOTS:
        void on_comboBox_currentIndexChanged ( int index );
        void init();
        void setOFunction();
        void on_newvar_clicked();
        void on_vars_itemClicked ( QListWidgetItem * item );
        void on_delvar_clicked();
        void on_vars_itemSelectionChanged ();
        void on_calvarmax_valueChanged(double );
        void on_calvarmin_valueChanged(double );
        void on_calvarstep_valueChanged(double );
        void on_varaddvalue_clicked();
        void on_vardelvalue_clicked();
        void on_varvalues_itemChanged ( QTableWidgetItem * item );
        void on_varvalues_itemSelectionChanged();
        void on_ovarofun_currentIndexChanged(QString );
        void on_ovaraddmember_clicked();
        void on_ovardelmember_clicked();
        void on_ovarmembers_itemSelectionChanged();
        void on_comboBox_templates_currentIndexChanged(int index);
        void on_newtemplate_clicked();
        void on_templates_itemClicked ( QListWidgetItem * item );
        void on_templates_itemSelectionChanged ();
        void on_deltemplate_clicked();
        void on_pushButton_clicked();
        void on_button_settemplatepath_clicked();
        void on_templateeditor_templatechanged(QString newtemplate, QString oldtemplate);
        void on_button_load_values_clicked();
        void on_groups_itemSelectionChanged();
        void on_add_group_clicked();
        void on_del_group_clicked();
        void on_button_groupmanage_clicked();
        void on_add_ignored_groups_clicked();
        void on_del_ignored_groups_clicked();
        void on_add_monitored_groups_clicked();
        void on_del_monitored_groups_clicked();
        void on_ignored_groups_itemSelectionChanged();
        void on_monitored_groups_itemSelectionChanged();
        void on_button_add_cal_ofun_clicked();
        void on_button_del_cal_ofun_clicked();
        void on_cal_ofunction_itemSelectionChanged();
        void on_button_calfun_advanced_clicked();
        void on_calfun_currentIndexChanged(QString name);
        void on_button_calsimulation_advanced_clicked();
        void on_calsimulation_currentIndexChanged(QString name);
        void groups_visible_entered();
        void show_cal_ofun();
        void on_ovaradvanced_clicked();
        void on_calstart_clicked();
        void on_calstop_clicked();
        void updatetimer_timeout();
        void updateAll();
        void on_actionopen_triggered();
        void on_actionsaveas_triggered();
        void on_actionsave_triggered();
        void on_actionnew_triggered();
        void on_actionsettings_triggered();
        void on_actionLoad_Python_script_triggered();
        void on_resulthandler_clicked();
        void on_actionSchlie_en_triggered();
        void closeEvent(QCloseEvent *event);

    Q_SIGNALS:
        void show_var();
        void show_calvar();
        void show_ovar();
        void show_calvar_template();
        void show_observedvar_template();
        void show_iterationvar_template();
        void start_gui();
        void show_ovar_advanced();
        void disable_varsettings();       
        void disable_templateeditor();
        void enable_groups();
        void disable_groups();
        void clean_calofun();
        void updateDiagram(Calibration *calibration, QStatusBar *);
        void running();
        void notrunning();
};

#endif // MAINWINDOW_H
