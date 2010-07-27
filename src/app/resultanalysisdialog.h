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
