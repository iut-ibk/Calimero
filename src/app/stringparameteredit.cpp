#include <stringparameteredit.h>
#include <ui_stringparameteredit.h>
#include <QFileDialog>

StringParameterEdit::StringParameterEdit(QWidget *parent, PARAMETEREDITTYPE enabledir)
	: QWidget(parent) {
	ui = new Ui::StringParameterEdit();
	ui->setupUi(this);
        ui->fileButton->setVisible(false);
        ui->dirButton->setVisible(false);

        switch(enabledir)
        {
        case FILEDIALOG:
            {
                ui->fileButton->setVisible(true);
                break;
            }
        case DIRDIALOG:
            {
                ui->dirButton->setVisible(true);
                break;
            }
        }
}

void StringParameterEdit::on_fileButton_clicked() {
	QFileInfo info(ui->value->text());
	QString path = QDir::current().path();
	if (info.dir().exists()) {
		path = info.dir().path();
	}
        QString name = QFileDialog::getOpenFileName(this,tr("Choose File or FileName"),path);

	if (name != "") {
		ui->value->setText(name);
	}
}

void StringParameterEdit::on_dirButton_clicked() {
        QFileInfo info(ui->value->text());
        QString path = QDir::current().path();
        if (info.dir().exists()) {
                path = info.dir().path();
        }
        QString name = QFileDialog::getExistingDirectory(this,tr("Choose Directory"),path);

        if (name != "") {
                ui->value->setText(name);
        }
}

QString StringParameterEdit::value() const {
	return ui->value->text();
}

void StringParameterEdit::setValue(const QString &value) {
	ui->value->setText(value);
}

