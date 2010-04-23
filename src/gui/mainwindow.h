#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

namespace Ui {
	class MainWindow;
}

class QStateMachine;
class GuiLogSink;

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

Q_SIGNALS:
        void show_var();
        void show_calvar();
        void show_ovar();
        void start_gui();
        void show_ovar_advanced();
};

#endif // MAINWINDOW_H
