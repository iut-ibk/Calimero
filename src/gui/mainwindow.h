#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QListWidgetItem>

namespace Ui {
	class MainWindow;
}

class QStateMachine;
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
        void on_calvarvalue_valueChanged(double );
        void on_varaddvalue_clicked();
        void on_vardelvalue_clicked();
        void on_varvalues_itemChanged ( QTableWidgetItem * item );
        void on_varvalues_itemSelectionChanged();
        void on_ovarofun_currentIndexChanged(QString );
        void on_ovaraddmember_clicked();
        void on_ovardelmember_clicked();
        void on_ovarmembers_itemSelectionChanged();

Q_SIGNALS:
        void show_var();
        void show_calvar();
        void show_ovar();
        void start_gui();
        void show_ovar_advanced();
        void disable_varsettings();  
};

#endif // MAINWINDOW_H
