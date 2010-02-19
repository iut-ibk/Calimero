#include "ParameterTextEdit.h"
#include <QRegExp>
#include <QStringList>
#include <QStyle>
#include "ProgressBar.h"
#include "QCoreApplication"

ParameterTextEdit::ParameterTextEdit ( QWidget * parent) : QPlainTextEdit ( parent )
{
    splitter=" ";
    isvector=false;
    enablecsv=false;
    startline=0;
    stopline=0;
    startcolumn=0;
    stopcolumn=0;
    init();
}

ParameterTextEdit::ParameterTextEdit ( const QString & text, QWidget * parent) : QPlainTextEdit ( text, parent )
{
    init();
}


void ParameterTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    bool isvalue;

    this->textCursor().selectedText().toDouble(&isvalue);

    if(isvalue)
    {
        if(!isvector)
        {
            newparam->setVisible(true);
            newvectorstart->setVisible(true);
            newvectorend->setVisible(false);
            canclevector->setVisible(false);
        }
        else
        {
            newvectorend->setVisible(true);
            newparam->setVisible(false);
            newvectorstart->setVisible(false);
            canclevector->setVisible(true);

        }
    }
    else
    {
            newparam->setVisible(false);
            newvectorstart->setVisible(false);
            newvectorend->setVisible(false);
            canclevector->setVisible(false);

            if(isvector)
                canclevector->setVisible(true);
    }

    menu->exec(event->globalPos());
}

void ParameterTextEdit::init()
{
    menu = new QMenu(this);

    newparam = new QAction("Create parameter", menu);
    newvectorstart = new QAction("Start vector", menu);
    newvectorend = new QAction("Finish vector", menu);
    canclevector = new QAction("Cancle Vector", menu);

    QObject::connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(editparameter(QAction*)));

    menu->addAction(newvectorstart);
    menu->addAction(newvectorend);
    menu->addAction(newparam);
    menu->addAction(canclevector);
}

void ParameterTextEdit::editparameter(QAction * action)
{
    if(action==newparam)
    {
        bool ok = false;
        QString name = QInputDialog::getText(this, tr("Please enter a parametername:"),tr("Parametername:"), QLineEdit::Normal,"", &ok);

        if (ok && !name.isEmpty())
        {
            values.clear();
            QTextCursor cursor = textCursor();
            values.append(cursor.selectedText().toDouble());
            cursor.removeSelectedText();
            cursor.insertText(QString("$") + name + QString("$"));
            emit newparameter(values, name);
        }
    }

    if(action==canclevector)
        isvector=false;

    if(action==newvectorstart || action==newvectorend)
    {

        int position=this->textCursor().columnNumber();
        int line=this->textCursor().blockNumber();

        QTextCursor oldcursor = this->textCursor();
        QTextCursor cursor = this->textCursor();
        cursor.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,position);
        this->setTextCursor(cursor);
            
        int column = this->textCursor().selectedText().split(splitter).size();
        this->setTextCursor(oldcursor);

        if(!isvector)
        {
            bool ok = false;
            QString name = QInputDialog::getText(this, tr("Please enter a string which splitts all columns:"),tr("Splitter:"), QLineEdit::Normal," ", &ok);

            if(!ok)
            {
                isvector=false;
                return;
            }

            if (name.isEmpty())
                QMessageBox::warning(this,tr("Warning"),tr("Wrong splitter"));

            splitter=name;

            isvector = !isvector;
            startline=line;
            startcolumn=column;
        }
        else
        {
            bool ok = false;
            QString name = QInputDialog::getText(this, tr("Please enter a parametername:"),tr("Parametername:"), QLineEdit::Normal,"", &ok);

            if(!ok)
            {
                isvector=false;
                return;
            }

            if (ok && !name.isEmpty())
            {
                values.clear();
                QString backup = this->document()->toPlainText();
                isvector = !isvector;
                stopline=line;
                stopcolumn=column;

                if(!checkVector(name))
                {
                    this->setPlainText(backup);
                    QMessageBox::warning(this,tr("Warning"),tr("Selected elements are not a vector"));
                    return;
                }
                emit newparameter(values, name);
            }
        }
    }
}

void ParameterTextEdit::setPlainText ( const QString & text )
{
    QPlainTextEdit::setPlainText(text);
}

bool ParameterTextEdit::checkVector(QString name)
{
    QTime time;
    time.start();
    values.clear();
    int update=0;
    QTextCharFormat format = currentCharFormat();
    ProgressBar progressbar(this);

    if(stopline!=startline && stopcolumn!=startcolumn)
        return false;

    int parameterindex = 0;
    QString newstring = "";
    QStringList list = this->document()->toPlainText().split(QRegExp("\n"));

    QCoreApplication::processEvents();

    for(int counter=0; counter<list.size(); counter++)
    {
        if(time.elapsed()>500)
        {
            progressbar.setVisible(true);
            progressbar.setRange(0,list.size());
        }

        if(counter%500==0)
            progressbar.setProgress(counter);

        if(counter>=startline && counter<=stopline)
        {
            QStringList line = list.at(counter).split(splitter);

            for(int tokenindex=0; tokenindex<line.size(); tokenindex++)
            {
                update++;

                if(tokenindex<startcolumn-1 || tokenindex>=stopcolumn)
                        newstring+=line.at(tokenindex);

                if(tokenindex>=startcolumn-1 && tokenindex<stopcolumn)
                {
                    QString tokenstring=line.at(tokenindex);
                    QString newtoken = "";
                    QString value = "";
                    bool founddouble=false;

                    for(int index=0; index<tokenstring.size(); index++)
                    {
                        if(tokenstring.at(index)==' ')
                        {
                            newtoken+=" ";
                        }
                        else
                        {
                            value+=tokenstring.at(index);
                            if(!founddouble)
                            {
                                if(parameterindex==0)
                                {
                                    newtoken+=QString("$") + name + QString("$");
                                }
                                else
                                {
                                    newtoken+=QString("$") + name + QString("_") + QString::number(parameterindex) + QString("$");
                                }
                                founddouble=true;
                                parameterindex++;
                            }
                        }
                    }

                    bool ok = false;
                    values.append(value.toDouble(&ok));
                    if(!ok)
                        return false;

                    newstring+=newtoken;
                }

                if(tokenindex!=line.size()-1)
                    newstring+=QString(splitter);
            }
        }

        if(counter>stopline || counter<startline)
            newstring+=list.at(counter);



        if(counter<list.size())
            newstring+=QString("\n");    
    }
    
    this->setPlainText(newstring);
    progressbar.setProgress(list.size());
    return true;
}

void ParameterTextEdit::changeEvent(QEvent *e)
{
    QPlainTextEdit::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        menu->removeAction(newvectorstart);
        menu->removeAction(newvectorend);
        menu->removeAction(newparam);
        menu->removeAction(canclevector);

        delete newvectorstart;
        delete newvectorend;
        delete newparam;
        delete canclevector;

        newparam = new QAction(tr("Create parameter"), menu);
        newvectorstart = new QAction(tr("Start vector"), menu);
        newvectorend = new QAction(tr("Finish vector"), menu);
        canclevector = new QAction(tr("Cancle Vector"), menu);

        menu->addAction(newvectorstart);
        menu->addAction(newvectorend);
        menu->addAction(newparam);
        menu->addAction(canclevector);
        break;
    default:
        break;
    }
}
