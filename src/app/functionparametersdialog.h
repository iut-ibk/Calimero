#ifndef FUNCTIONPARAMETERSDIALOG_H
#define FUNCTIONPARAMETERSDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
        class FunctionParametersDialog;
}

class IFunction;
class QGridLayout;
class QLineEdit;

class FunctionParametersDialog : public QDialog
{
public:
        FunctionParametersDialog(IFunction *function, QWidget *parent = 0);
        virtual ~FunctionParametersDialog();

        bool updateFunctionParameters();

private:
        QWidget *widgetForParameter(std::string name);
        Ui::FunctionParametersDialog *ui;
        IFunction *function;
	QMap<std::string, QWidget *> widgets;
};

#endif // FUNCTIONPARAMETERSDIALOG_H
