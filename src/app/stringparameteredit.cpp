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
        case NOTYPEDIALOG:
            {
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

