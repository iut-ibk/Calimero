#ifndef PARAMETERTEXTEDIT_H_INCLUDED
#define PARAMETERTEXTEDIT_H_INCLUDED

#include <QPlainTextEdit>
#include <QContextMenuEvent>
#include <QtGui>
#include <iostream>
#include <QVector>

class ParameterTextEdit : public QPlainTextEdit
{
    Q_OBJECT

    public:
        QMenu *menu;
        QAction *newparam;
        QAction *newvectorstart;
        QAction *newvectorend;
        QAction *canclevector;
        QAction *importfile;
        int numberoflines;
        QString textstring;
        QString splitter;
        QStateMachine *state_machine;

        QVector<double> values;
        int startline, stopline, startcolumn, stopcolumn;
        QStringList list;
        ParameterTextEdit ( QWidget * parent = 0 );
        ~ParameterTextEdit();

    private:
        bool checkVector(QString name);
        void setupStateMachine();

    public Q_SLOTS:
        void contextMenuEvent(QContextMenuEvent *event);
        void selectionChanged();
        void on_importfile_triggered ( bool checked );
        void on_canclevector_triggered ( bool checked );
        void on_newvectorend_triggered ( bool checked );
        void on_newvectorstart_triggered ( bool checked );
        void on_newparam_triggered(bool checked);

    Q_SIGNALS:
        void valid_value();
        void no_valid_value();
        void valid_vector();
        void no_vector();
        void templatechanged(QString newtemplate, QString oldtemplate);

    protected:
        virtual void changeEvent(QEvent *e);

};

#endif // PARAMETERTEXTEDIT_H_INCLUDED
