#include <functionparametersdialog.h>
#include <ui_functionparametersdialog.h>

#include <boost/foreach.hpp>
#include <limits>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QDirModel>
#include <QCompleter>
#include <QDebug>
#include <IFunction.h>
#include <stringparameteredit.h>
#include <QCheckBox>

typedef std::pair<std::string, DATATYPE> ptype;

FunctionParametersDialog::~FunctionParametersDialog() {
}

FunctionParametersDialog::FunctionParametersDialog(IFunction *function, QWidget *parent) : QDialog(parent), function(function) {
        ui = new Ui::FunctionParametersDialog();
	ui->setupUi(this);

	int row = 0;

        BOOST_FOREACH(ptype p, function->getDataTypes()) {
                QString name = QString::fromStdString(p.first);
		QLabel *label = new QLabel(this);
                label->setText(QString("%1:").arg(name));
                QWidget *param_widget = widgetForParameter(name.toStdString());
		ui->gridLayout->addWidget(label, row, 0, Qt::AlignRight);
		ui->gridLayout->addWidget(param_widget, row, 1);
		row ++;
                widgets[name.toStdString()] = param_widget;
	}

	this->adjustSize();
}

QWidget *FunctionParametersDialog::widgetForParameter(std::string name)
{
    switch(function->getDataTypes()[name])
    {
    case BOOL:
        {
            QCheckBox *widget = new QCheckBox(this);
            widget->setChecked(QString::fromStdString(function->getValueOfParameter(name)).toInt());
            return widget;
            break;
        }
    case STRING:
        {
            StringParameterEdit *widget = new StringParameterEdit(this);
            std::string value = (std::string) function->getValueOfParameter(name);
            widget->setValue(QString::fromStdString(value));
            return widget;
            break;
        }
    case DOUBLE:
        {
            QDoubleSpinBox *widget = new QDoubleSpinBox(this);
            widget->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
            widget->setDecimals(10);
            double value = QString::fromStdString(function->getValueOfParameter(name)).toDouble();
            widget->setValue(value);
            Logger(Debug) << "set " << value;
            return widget;
            break;
        }
    case INT:
        {
            QSpinBox *widget = new QSpinBox(this);
            widget->setRange(-std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
            double value = QString::fromStdString(function->getValueOfParameter(name)).toDouble();
            widget->setValue(value);
            Logger(Debug) << "set " << value;
            return widget;
            break;
        }
    case UINT:
        {
            QSpinBox *widget = new QSpinBox(this);
            widget->setRange(0, std::numeric_limits<int>::max());
            double value = QString::fromStdString(function->getValueOfParameter(name)).toInt();
            widget->setValue(value);
            Logger(Debug) << "set " << value;
            return widget;
            break;
        }
    default:
        {
            Logger() << "UNKNOWN  type of parameter " << name;
        }
    }
    return new QLineEdit(this);
}

bool FunctionParametersDialog::updateFunctionParameters() {
        Q_FOREACH(std::string p, widgets.keys())
        {
            switch(function->getDataTypes()[p])
            {
            case STRING:
                {
                StringParameterEdit *widget = (StringParameterEdit *) widgets[p];
                function->setValueOfParameter(p,widget->value().toStdString());
                continue;
            }
            case UINT:
                {
                QSpinBox *widget = (QSpinBox *) widgets[p];
                function->setValueOfParameter(p, QString::number(widget->value()).toStdString());
                continue;
            }
            case INT:
                {
                QSpinBox *widget = (QSpinBox *) widgets[p];
                function->setValueOfParameter(p, QString::number(widget->value()).toStdString());
                continue;
            }
            case BOOL:
                {
                QCheckBox *widget = (QCheckBox *) widgets[p];
                function->setValueOfParameter(p, QString::number(widget->isChecked()).toStdString());
                continue;
            }
            case DOUBLE:
                {
                QDoubleSpinBox *widget = (QDoubleSpinBox *) widgets[p];
                function->setValueOfParameter(p, QString::number(widget->value()).toStdString());
                continue;
            }
            default:
                {
                Logger() << "cannot update node parameter " << p;
                continue;
            }
            }
	}
	return true;
}
