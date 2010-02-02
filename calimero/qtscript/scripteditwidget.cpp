#include "scripteditwidget.h"
#include "../ui_scripteditwidget.h"
#include <QtGui>
#include <iostream>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QList>
#include "ScriptLibary.h"

void ScriptEditWidgetAgent::exceptionThrow ( qint64 scriptId, const QScriptValue & exception, bool hasHandler )
{
    Q_UNUSED(scriptId);
    Q_UNUSED(hasHandler);
    this->engine()->evaluate("print(\"" + exception.toString() + "\");");
}

ScriptEditWidget::ScriptEditWidget(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ScriptEditWidget)
{
    m_ui->setupUi(this);
    init();
}

void ScriptEditWidget::init()
{
    scriptengine = NULL;
    scriptdebugger = NULL;
    highlighter = NULL;
    scriptpathdir="";
    namefilter = "*";
    currentscriptname = "";
    QObject::connect(m_ui->w_scripts, SIGNAL(itemSelectionChanged()),this, SLOT(loadFile()));

}

ScriptEditWidget::~ScriptEditWidget()
{
    on_b_stop_clicked();
    QObject::disconnect(m_ui->w_scripts, SIGNAL(itemSelectionChanged()),this, SLOT(loadFile()));


    delete m_ui;
    delete scriptdebugger;
    delete scriptengine;
    delete highlighter;
}

void ScriptEditWidget::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ScriptEditWidget::setScriptEngine(QScriptEngine *engine)
{
    if(engine==NULL)
        return;

    if(engine->isEvaluating())
        engine->abortEvaluation();

    if(scriptengine!=NULL)
    {
        delete scriptdebugger;
        delete scriptengine;
        delete highlighter;
    }

    scriptengine=engine;

    scriptengine->setProcessEventsInterval(100);

    scriptdebugger = new QScriptEngineDebugger(QApplication::activeWindow());
    scriptdebugger->setParent(this);
    scriptdebugger->attachTo(scriptengine);


    highlighter = new QScriptHighlighter(m_ui->w_codewidget->document());
    QObject::connect(scriptengine, SIGNAL(signalHandlerException(const QScriptValue)), this, SLOT(on_b_newscript_clicked()));

    QScriptValue include = scriptengine->newFunction(ScriptLibary::include);
    include.setProperty("WORKINGDIR", scriptpathdir);
    scriptengine->globalObject().setProperty("include", include);

}

QScriptEngine* ScriptEditWidget::getScriptEngine()
{
    return scriptengine;
}

QString ScriptEditWidget::getScriptPathDir()
{
    return scriptpathdir;
}

void ScriptEditWidget::setScriptPathDir(QString path)
{
    if(path==NULL)
        return;

    if(!QFile::exists(path))
        return;

    scriptpathdir = path;
    m_ui->w_scripts->clear();
    
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList(namefilter));
    
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        m_ui->w_scripts->addItem(fileInfo.fileName());
    }

    if(!currentscriptname.isEmpty())
    {
        if(!m_ui->w_scripts->findItems(currentscriptname,Qt::MatchExactly).isEmpty())
            m_ui->w_scripts->setCurrentItem(m_ui->w_scripts->findItems(currentscriptname,Qt::MatchExactly).at(0));
    }

    if(scriptengine != NULL)
    {
        QScriptValue include = scriptengine->newFunction(ScriptLibary::include);
        include.setProperty("WORKINGDIR", scriptpathdir);
        scriptengine->globalObject().setProperty("include", include);
    }
}

void ScriptEditWidget::loadFile()
{
    saveCurrentScript();
    QString script = "";

    if(!Persistence::loadFileAsString(scriptpathdir + "/" + m_ui->w_scripts->currentItem()->text(),&script))
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Could not load file"));
        return;
    }

    currentscriptname = m_ui->w_scripts->currentItem()->text();

    if(script == "")
        script = "/* " + currentscriptname + " */";

    m_ui->w_codewidget->document()->setPlainText(script);

}

void ScriptEditWidget::setNameFilter(QString namefilter)
{
    if(namefilter == NULL)
        return;

    this->namefilter = namefilter;

    setScriptPathDir(scriptpathdir);
}

void ScriptEditWidget::saveScript(QString script, QString scriptname)
{
    qDebug() << "ScriptEditWidget::saveScript()";
    if(!Persistence::saveStringAsFile(scriptpathdir + "/" + scriptname,script))
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Could not save file"));
        return;
    }
    qDebug() << "ScriptEditWidget::saveScript() Script save DONE: " << scriptpathdir + "/" + scriptname ;
}

void ScriptEditWidget::deleteScript(QString scriptname)
{
    m_ui->w_scripts->clear();
    m_ui->w_codewidget->document()->setPlainText("");
    QFile::remove(scriptpathdir + "/" + scriptname);
    currentscriptname = "";
    setScriptPathDir(getScriptPathDir());
}


void ScriptEditWidget::on_b_deletescript_clicked()
{
    if(m_ui->w_scripts->selectedItems().size()<1)
        return;

    QString selectedString = m_ui->w_scripts->currentItem()->text();

    if(selectedString.isEmpty())
        return;

    currentscriptname = "";
    deleteScript(selectedString);
}

void ScriptEditWidget::saveCurrentScript()
{
    qDebug() << "ScriptEditWidget::saveCurrentScript()";
    if(!currentscriptname.isEmpty())
    {
        saveScript(m_ui->w_codewidget->document()->toPlainText(),currentscriptname);
    }
    else
    {
        qWarning() << "ScriptEditWidget::saveCurrentScript() No script name set";
    }
}

void ScriptEditWidget::on_b_save_clicked()
{
    qDebug() << "ScriptEditWidget::on_b_save_clicked()";
    saveCurrentScript();
}

void ScriptEditWidget::on_b_newscript_clicked()
{
     bool ok;
     QString text = QInputDialog::getText(this, tr("Script"), tr("Scriptname:"), QLineEdit::Normal, "", &ok);

     if (ok && !text.isEmpty())
     {
         text += ".qs";

         if(QFile::exists(scriptpathdir + "/" + text))
         {
             QMessageBox::warning(this, tr("ERROR"), tr("Could not create file"));
             return;
         }

         saveScript("",text);
         currentscriptname = "";
         setScriptPathDir(getScriptPathDir());
         m_ui->w_scripts->setCurrentItem(m_ui->w_scripts->findItems(text,Qt::MatchExactly).at(0));
     }
}

void ScriptEditWidget::on_b_start_clicked()
{
    scriptengine->clearExceptions();
    QObject::connect(scriptengine, SIGNAL(signalHandlerException(const QScriptValue)), this, SLOT(on_b_newscript_clicked()));
    if(scriptengine->isEvaluating())
    {
        on_b_stop_clicked();
        QMessageBox::warning(this, tr("ERROR"), tr("Already running script"));
        return;
    }

    saveCurrentScript();
    scriptdebugger->setAutoShowStandardWindow(false);
    QScriptValue print = scriptengine->newFunction(ScriptLibary::printconsole);
    print.setData(scriptengine->newQObject(m_ui->w_console));
    scriptengine->globalObject().setProperty("print", print);

    if(m_ui->w_scripts->currentItem()==NULL)
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Choose a script"));
        return;
    }

    m_ui->w_console->setPlainText("=== Script Start ===\n");
    QString fileName = scriptpathdir + "/" + m_ui->w_scripts->currentItem()->text();
    QString contents = "";

    if(!Persistence::loadFileAsString(fileName,&contents))
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Could not load file"));
        return;
    }

    scriptdebugger->setAutoShowStandardWindow(false);
    ScriptEditWidgetAgent agent(scriptengine);
    scriptengine->setAgent(&agent);

    if(QScriptEngine::checkSyntax(contents).state()==QScriptSyntaxCheckResult::Valid && scriptengine->canEvaluate(contents))
    {
        scriptengine->evaluate(contents, m_ui->w_scripts->currentItem()->text());
    }
    else
    {
        QScriptSyntaxCheckResult result = QScriptEngine::checkSyntax(contents);

        if(result.state()!=QScriptSyntaxCheckResult::Valid)
        {
            m_ui->w_console->setPlainText("Syntax Error on line: " + QString::number(result.errorLineNumber()));
            QTextCursor tmpcursor = m_ui->w_codewidget->textCursor();
            tmpcursor.setPosition(0,QTextCursor::MoveAnchor);
            tmpcursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,result.errorLineNumber()-1);
            tmpcursor.select(QTextCursor::LineUnderCursor);
            m_ui->w_codewidget->setTextCursor(tmpcursor);
        }
        else
        {
            m_ui->w_console->setPlainText("Syntax Error");
        }
        return;
    }
    QString output = m_ui->w_console->toPlainText();
    output += "=== Script DONE ===\n";
    m_ui->w_console->setPlainText(output);
    QTextCursor cursor =  m_ui->w_console->textCursor();
    cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
     m_ui->w_console->setTextCursor(cursor);
}

void ScriptEditWidget::on_b_debug_clicked()
{
    scriptengine->clearExceptions();
    QObject::connect(scriptengine, SIGNAL(signalHandlerException(const QScriptValue)), this, SLOT(on_b_newscript_clicked()));
    if(scriptengine->isEvaluating())
    {
        on_b_stop_clicked();
        QMessageBox::warning(this, tr("ERROR"), tr("Already running script"));
        return;
    }

    if(m_ui->w_scripts->currentItem()==NULL)
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Choose a script"));
        return;
    }

    saveCurrentScript();



    if(m_ui->w_scripts->currentItem()==NULL)
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Choose a script"));
        return;
    }


    m_ui->w_console->setPlainText("=== Debug Script Start ===\n");
    QString fileName = scriptpathdir + "/" + m_ui->w_scripts->currentItem()->text();
    QString contents = "debugger;\n";

    if(!Persistence::loadFileAsString(fileName,&contents))
    {
        QMessageBox::warning(this, tr("ERROR"), tr("Could not load file"));
        return;
    }

    scriptdebugger->setAutoShowStandardWindow(true);

    if(QScriptEngine::checkSyntax(contents).state()==QScriptSyntaxCheckResult::Valid && scriptengine->canEvaluate(contents))
    {
        if(scriptdebugger!=NULL)
        {

            scriptdebugger->detach();
            delete scriptdebugger;
        }

        scriptdebugger = new QScriptEngineDebugger(QApplication::activeWindow());
        scriptdebugger->attachTo(scriptengine);
        scriptdebugger->setAutoShowStandardWindow(true);
        m_ui->tabWidget->addTab(scriptdebugger->standardWindow(),"Debugger");
        m_ui->tabWidget->setCurrentWidget(scriptdebugger->standardWindow());
        scriptengine->evaluate(contents, m_ui->w_scripts->currentItem()->text());
    }
    else
    {
        QScriptSyntaxCheckResult result = QScriptEngine::checkSyntax(contents);

        if(result.state()!=QScriptSyntaxCheckResult::Valid)
        {
            m_ui->w_console->setPlainText("Syntax Error on line: " + QString::number(result.errorLineNumber()-1));
            QTextCursor tmpcursor = m_ui->w_codewidget->textCursor();
            tmpcursor.setPosition(0,QTextCursor::MoveAnchor);
            tmpcursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,result.errorLineNumber()-2);
            tmpcursor.select(QTextCursor::LineUnderCursor);
            m_ui->w_codewidget->setTextCursor(tmpcursor);
        }
        else
        {
            m_ui->w_console->setPlainText("Syntax Error");
        }
        return;
    }


    QString output = m_ui->w_console->toPlainText();
    output += "=== Debug Script DONE ===\n";
    m_ui->tabWidget->removeTab(m_ui->tabWidget->indexOf(scriptdebugger->standardWindow()));
    m_ui->w_console->setPlainText(output);
    QTextCursor cursor =  m_ui->w_console->textCursor();
    cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
    m_ui->w_console->setTextCursor(cursor);
}

void ScriptEditWidget::on_b_stop_clicked()
{
    if(scriptengine!=NULL)
        scriptengine->abortEvaluation();
}

void ScriptEditWidget::errorHandler()
{
    m_ui->w_codewidget->setPlainText("Syntax ERROR");
}
