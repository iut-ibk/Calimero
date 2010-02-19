#ifndef SCRIPTEDITWIDGET_H
#define SCRIPTEDITWIDGET_H

#include <QtGui/QDialog>
#include <QtGui/QPlainTextEdit>
#include <QtScript/QScriptEngine>
#include <QScriptEngineDebugger>
#include "qscripthighlighter.h"
#include <QScriptEngineAgent>


using namespace SharedTools;

namespace Ui {
    class ScriptEditWidget;
    class ScriptEditWidgetAgent;
}

class ScriptEditWidgetAgent : public QScriptEngineAgent
{
    public:
        ScriptEditWidgetAgent(QScriptEngine *engine) : QScriptEngineAgent(engine){}
        virtual void exceptionThrow ( qint64 scriptId, const QScriptValue & exception, bool hasHandler );
};

class ScriptEditWidget : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ScriptEditWidget)

private:
    QScriptEngine *scriptengine;
    QScriptEngineDebugger *scriptdebugger;
    QScriptHighlighter *highlighter;
    QString scriptpathdir;
    QString namefilter;
    QString currentscriptname;

public:
    explicit ScriptEditWidget(QWidget *parent = 0);
    void setScriptEngine(QScriptEngine *engine);
    QScriptEngine* getScriptEngine();
    QString getScriptPathDir();
    void setScriptPathDir(QString path);
    void setNameFilter(QString namefilter);
    void saveScript(QString script, QString scriptname);
    void deleteScript(QString scriptname);
    void init();
    virtual ~ScriptEditWidget();

private slots:
    void on_b_deletescript_clicked();
    void on_b_save_clicked();
    void on_b_newscript_clicked();
    void on_b_start_clicked();
    void on_b_debug_clicked();
    void on_b_stop_clicked();
    void saveCurrentScript();
    void errorHandler();
    void loadFile();

protected:
    virtual void changeEvent(QEvent *e);

private :
    Ui::ScriptEditWidget *m_ui;
};

#endif // SCRIPTEDITWIDGET_H
