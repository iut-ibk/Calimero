#ifndef PARAMETERTEXTEDIT_H_INCLUDED
#define PARAMETERTEXTEDIT_H_INCLUDED

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
        int numberoflines;
        QString textstring;
        QString splitter;

        QVector<double> values;
        bool isvector;
        int startline, stopline, startcolumn, stopcolumn;
        bool enablecsv;
        QStringList list;

        ParameterTextEdit ( QWidget * parent = 0 );
        ParameterTextEdit ( const QString & text, QWidget * parent = 0 );
        void contextMenuEvent(QContextMenuEvent *event);

    private:
        void init();
        bool checkVector(QString name);

    public slots:
        void editparameter(QAction * action);
        void setPlainText ( const QString & text );



    signals:
        void newparameter(QVector<double> values, QString name);

    protected:
        virtual void changeEvent(QEvent *e);

};

#endif // PARAMETERTEXTEDIT_H_INCLUDED
