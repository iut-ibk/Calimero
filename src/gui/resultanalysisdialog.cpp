#include <resultanalysisdialog.h>
#include <ui_resultanalysisdialog.h>
#include <IResultHandler.h>
#include <QInputDialog>
#include <CalibrationEnv.h>
#include <vector>
#include <IFunction.h>
#include <functionparametersdialog.h>
#include <QMessageBox>

using namespace std;

ResultAnalysisDialog::ResultAnalysisDialog(QWidget *parent)
{
    ui = new Ui::ResultAnalysisDialog();
    ui->setupUi(this);
    ui->handlername->setText("");
    ui->handlerfunction->setText("");
    ui->delhandler->setEnabled(false);
    ui->settingsbox->setEnabled(false);
}

ResultAnalysisDialog::~ResultAnalysisDialog()
{
}

void ResultAnalysisDialog::on_run_all_clicked()
{
    CalibrationEnv::getInstance()->runAllEnabledResultHandler();
}

void ResultAnalysisDialog::on_delhandler_clicked()
{
    Calibration *cal = CalibrationEnv::getInstance()->getCalibration();

    QList<QListWidgetItem *> list = ui->handlers->selectedItems ();
    for(int index=0; index < list.size(); index++ )
    {
        bool ok = cal->removeResultHandler(list.at(index)->text().toStdString());

        if(ok)
            delete list.at(index);
        else
            QMessageBox::warning(this,tr("Delete analysis function"),tr("Cannot delete function: ") + list.at(index)->text());
    }
}

void ResultAnalysisDialog::on_newhandler_clicked()
{
    CalibrationEnv *env = CalibrationEnv::getInstance();
    bool ok;
    QString funname = QInputDialog::getText(this, tr("Analysis name"),tr("Name:"), QLineEdit::Normal,"", &ok);

    if (ok && !funname.isEmpty())
    {
        map<string, string> handlers = env->getCalibration()->getResultHandlers();
        if(handlers.find(funname.toStdString())!=handlers.end())
        {
            QMessageBox::warning(this,tr("New analysis function"),tr("Name already exists: ") + funname);
            return;
        }

        QStringList items;
        vector<string> available = env->getAvailableResultHandlers();
        BOOST_FOREACH(string name, available)
                items.push_back(QString::fromStdString(name));

        QString text = QInputDialog::getItem ( this, "Analyse functions", "Available", items, 0, false, &ok);

        if (!ok || text.isEmpty())
            return;

        ok = env->getCalibration()->addResultHandler(funname.toStdString(),
                                                                               text.toStdString(),
                                                                               env->getResultHandlerReg()->getFunction(text.toStdString())->getParameterValues(),
                                                                               false);

        if(ok)
            ui->handlers->addItem(funname);
    }
}

void ResultAnalysisDialog::on_handleradvanced_clicked()
{
    Calibration* calibration = CalibrationEnv::getInstance()->getCalibration();
    string functionname = calibration->getResultHandlers()[ui->handlers->currentItem()->text().toStdString()];

    IResultHandler *function = CalibrationEnv::getInstance()->getResultHandlerReg()->getFunction(functionname);
    function->setValues(calibration->getResultHandlerSettings(ui->handlers->currentItem()->text().toStdString()));
    FunctionParametersDialog np(function);

    if(np.exec())
    {
        np.updateFunctionParameters();
        calibration->addResultHandler(ui->handlers->currentItem()->text().toStdString(),
                                      functionname,
                                      function->getParameterValues(),
                                      calibration->isResultHandlerEnabled(ui->handlers->currentItem()->text().toStdString()));
    }

    delete function;
}

void ResultAnalysisDialog::on_run_clicked()
{
    CalibrationEnv::getInstance()->runResultHandler(ui->handlers->currentItem()->text().toStdString());
}

void ResultAnalysisDialog::on_handlerenabled_clicked(bool checked)
{
    Calibration* calibration = CalibrationEnv::getInstance()->getCalibration();
    string functionname = calibration->getResultHandlers()[ui->handlers->currentItem()->text().toStdString()];

    calibration->addResultHandler(  ui->handlers->currentItem()->text().toStdString(),
                                    functionname,
                                    calibration->getResultHandlerSettings(ui->handlers->currentItem()->text().toStdString()),
                                    ui->handlerenabled->isChecked());

}

void ResultAnalysisDialog::on_handlers_itemSelectionChanged()
{
    if(!ui->handlers->selectedItems().size())
    {
        ui->handlername->setText("");
        ui->handlerfunction->setText("");
        ui->settingsbox->setEnabled(false);
        ui->delhandler->setEnabled(false);
        ui->handlerenabled->setChecked(false);
    }
    else
    {
        ui->delhandler->setEnabled(true);
        ui->settingsbox->setEnabled(true);

        QString currentText = ui->handlers->currentItem()->text();

        CalibrationEnv *env = CalibrationEnv::getInstance();
        ui->settingsbox->setEnabled(true);
        ui->handlername->setText(currentText);
        string functionname = env->getCalibration()->getResultHandlers()[currentText.toStdString()];
        ui->handlerfunction->setText(QString::fromStdString(functionname));

        if(env->getCalibration()->isResultHandlerEnabled(currentText.toStdString()))
            ui->handlerenabled->setChecked(true);
        else
            ui->handlerenabled->setChecked(false);

        if(!env->getResultHandlerReg()->getSettingTypes(functionname).size())
            ui->handleradvanced->setEnabled(false);
        else
            ui->handleradvanced->setEnabled(true);
    }
}

void ResultAnalysisDialog::clear()
{
    ui->handlers->clear();
    ui->handlername->setText("");
    ui->handlerfunction->setText("");
    ui->settingsbox->setEnabled(false);
}

void ResultAnalysisDialog::updateAll()
{
    clear();
    map<string, string> names = CalibrationEnv::getInstance()->getCalibration()->getResultHandlers();
    std::pair<string, string> p;

    BOOST_FOREACH(p, names)
            ui->handlers->addItem(QString::fromStdString(p.first));
}
