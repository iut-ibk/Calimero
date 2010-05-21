#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QListWidgetItem>
#include <QTimer>


#define DEFAULTLOGLEVEL Standard
#define UPDATETIME 1000

namespace Ui {
	class MainWindow;
}

class QStateMachine;
class Calibration;
class GuiLogSink;
class QTableWidgetItem;

class MainWindow : public QMainWindow {
Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();
        void setupStateMachine();
	Ui::MainWindow *ui;
        QStateMachine *state_machine;
        GuiLogSink *log_updater;
        QTimer updatetimer;

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
        void on_calvarinit_valueChanged(double );
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
        void updateDiagram(Calibration *calibration);
};

#endif // MAINWINDOW_H
