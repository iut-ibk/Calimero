#ifndef STRINGPARAMETEREDIT_H
#define STRINGPARAMETEREDIT_H

#include <QWidget>

namespace Ui {
	class StringParameterEdit;
}

enum PARAMETEREDITTYPE {NOTYPEDIALOG = -1,
                        FILEDIALOG = 0,
                        DIRDIALOG = 1
                    };

class StringParameterEdit : public QWidget {
Q_OBJECT
public:
        StringParameterEdit(QWidget *parent = 0, PARAMETEREDITTYPE enabledir=NOTYPEDIALOG);

	QString value() const;
	void setValue(const QString &value);
private Q_SLOTS:
	void on_fileButton_clicked();
        void on_dirButton_clicked();
private:
	Ui::StringParameterEdit *ui;
};

#endif // STRINGPARAMETEREDIT_H
