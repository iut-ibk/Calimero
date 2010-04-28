#include <parametertextedit.h>
#include <QRegExp>
#include <QStringList>
#include <QStyle>
#include <QCoreApplication>
#include <Logger.h>
#include <QTimer>

ParameterTextEdit::ParameterTextEdit ( QWidget * parent) : QPlainTextEdit ( parent )
{
    splitter=" ";
    startline=0;
    stopline=0;
    startcolumn=0;
    stopcolumn=0;

    menu = new QMenu(this);
    menu->setObjectName("menu");

    newparam = new QAction("Create parameter", menu);
    newvectorstart = new QAction("Start vector", menu);
    newvectorend = new QAction("Finish vector", menu);
    canclevector = new QAction("Cancle Vector", menu);
    importfile = new QAction("Import file", menu);
    newparam->setObjectName(QString::fromUtf8("newparam"));
    newvectorstart->setObjectName(QString::fromUtf8("newvectorstart"));
    newvectorend->setObjectName(QString::fromUtf8("newvectorend"));
    importfile->setObjectName(QString::fromUtf8("importfile"));
    canclevector->setObjectName(QString::fromUtf8("canclevector"));


    menu->addAction(newvectorstart);
    menu->addAction(newvectorend);
    menu->addAction(newparam);
    menu->addAction(canclevector);
    menu->addAction(importfile);

    setupStateMachine();
    QObject::connect(this,SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    QMetaObject::connectSlotsByName(this);
}

ParameterTextEdit::~ParameterTextEdit()
{
    QObject::disconnect(this,SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

void ParameterTextEdit::setupStateMachine()
{
    state_machine = new QStateMachine();

    //states
    QState *init = new QState(QState::ParallelStates);

    QState *contextmenuestates = new QState(init);
    QState *vector = new QState(contextmenuestates);
    QState *value = new QState(contextmenuestates);
    QState *novalidvector = new QState(contextmenuestates);
    QState *novalidvalue = new QState(contextmenuestates);
    contextmenuestates->setInitialState(novalidvalue);

    //transitions
    value->addTransition(this,SIGNAL(valid_vector()),vector);
    vector->addTransition(this,SIGNAL(no_vector()),value);
    vector->addTransition(this, SIGNAL(no_valid_value()),novalidvector);
    novalidvector->addTransition(this, SIGNAL(valid_value()),vector);
    novalidvector->addTransition(this, SIGNAL(no_vector()),value);
    value->addTransition(this, SIGNAL(no_valid_value()),novalidvalue);
    novalidvalue->addTransition(this, SIGNAL(valid_value()),value);

    //assign properties
    value->assignProperty(importfile, "visible", true);
    value->assignProperty(newparam, "visible", true);
    value->assignProperty(newvectorstart, "visible", true);
    value->assignProperty(newvectorend, "visible", false);
    value->assignProperty(canclevector, "visible", false);

    vector->assignProperty(importfile, "visible", false);
    vector->assignProperty(newparam, "visible", false);
    vector->assignProperty(newvectorstart, "visible", false);
    vector->assignProperty(newvectorend, "visible", true);
    vector->assignProperty(canclevector, "visible", true);

    novalidvector->assignProperty(importfile, "visible", false);
    novalidvector->assignProperty(newparam, "visible", false);
    novalidvector->assignProperty(newvectorstart, "visible", false);
    novalidvector->assignProperty(newvectorend, "visible", false);
    novalidvector->assignProperty(canclevector, "visible", true);

    novalidvalue->assignProperty(importfile, "visible", true);
    novalidvalue->assignProperty(newparam, "visible", false);
    novalidvalue->assignProperty(newvectorstart, "visible", false);
    novalidvalue->assignProperty(newvectorend, "visible", false);
    novalidvalue->assignProperty(canclevector, "visible", false);

    //start state machine
    state_machine->addState(init);
    state_machine->setInitialState(init);
    state_machine->start();
}

void ParameterTextEdit::selectionChanged()
{
    bool isvalue;
    this->textCursor().selectedText().toDouble(&isvalue);

    if(isvalue)
        Q_EMIT valid_value();
    else
        Q_EMIT no_valid_value();
}

void ParameterTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    menu->exec(event->globalPos());
}

void ParameterTextEdit::on_newparam_triggered(bool checked)
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
        Q_EMIT no_valid_value();
    }
}

void ParameterTextEdit::on_importfile_triggered ( bool checked )
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Import file"), QDir::homePath(), tr("*.*"));
    if(!QFile::exists(fileName))
        return;

     QFile inputfile(fileName);

     if (!inputfile.open(QIODevice::ReadOnly))
          return;

     QString text = "";
     QTextStream inputstream(&inputfile);
     text+=inputstream.readAll();
     inputfile.close();
     this->setPlainText(text);
}

void ParameterTextEdit::on_canclevector_triggered ( bool checked )
{
    Q_EMIT no_vector();
}

void ParameterTextEdit::on_newvectorend_triggered ( bool checked )
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

    bool ok = false;
    QString name = QInputDialog::getText(this, tr("Please enter a parametername:"),tr("Parametername:"), QLineEdit::Normal,"", &ok);

    if(!ok)
        return;

    if (!name.isEmpty())
    {
        values.clear();
        QString backup = this->document()->toPlainText();
        stopline=line;
        stopcolumn=column;

        if(!checkVector(name))
        {
            this->setPlainText(backup);
            QMessageBox::warning(this,tr("Warning"),tr("Selected elements are not a vector"));
        }
        Q_EMIT no_vector();
    }
}

void ParameterTextEdit::on_newvectorstart_triggered ( bool checked )
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

    bool ok = false;
    QString name = QInputDialog::getText(this, tr("Please enter a string which splitts all columns:"),tr("Splitter:"), QLineEdit::Normal," ", &ok);

    if(!ok)
        return;

    if(name.isEmpty())
        QMessageBox::warning(this,tr("Warning"),tr("Wrong splitter"));

    splitter=name;

    startline=line;
    startcolumn=column;

    Q_EMIT valid_vector();
}

bool ParameterTextEdit::checkVector(QString name)
{
    values.clear();
    int update=0;
    QTextCharFormat format = currentCharFormat();


    if(stopline!=startline && stopcolumn!=startcolumn)
        return false;

    int parameterindex = 0;
    QString newstring = "";
    QStringList list = this->document()->toPlainText().split(QRegExp("\n"));
    QTime timer;
    timer.start();
    QProgressDialog progress("Extracting vector", "Cancle", 0, list.size()-1, this);
    progress.setWindowModality(Qt::WindowModal);

    for(int counter=0; counter<list.size(); counter++)
    {
        if(timer.elapsed()>50)
        {
            progress.setValue(counter);
            timer.restart();
        }
        if (progress.wasCanceled())
            return false;

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
    progress.setValue(list.size()-1);
    this->setPlainText(newstring);
    return true;
}

void ParameterTextEdit::changeEvent(QEvent *e)
{
    QPlainTextEdit::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        {
            newparam->setText(tr("Create parameter"));
            newvectorstart->setText(tr("Start vector"));
            newvectorend->setText(tr("Finish vector"));
            canclevector->setText(tr("Cancle Vector"));
            break;
        }
    default:
        break;
    }
}
