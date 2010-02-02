#include "ParameterEditGui.h"
#include "../persistence/Persistence.h"
#include "../core/CalibrationVariable.h"
#include <iostream>
#include <QVector>
#include <QTextEdit>
#include "ProgressBar.h"
#include <QTime>


ParameterEditGui::ParameterEditGui(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    position=0;
    currentfilename="";
    model=NULL;
    loadnewtemplate=true;
    QObject::connect(cb_parametertype, SIGNAL(currentIndexChanged ( const QString)), this, SLOT(changeCurrentParameterType()));
    QObject::connect(cb_filename, SIGNAL(activated ( const QString)), this, SLOT(changeCurrentFile(const QString)));
    QObject::connect(textEdit, SIGNAL(newparameter(QVector<double>, QString)), this, SLOT(newparameter(QVector<double>, QString)));
    QObject::connect(b_delete, SIGNAL(clicked()), this, SLOT(deleteparameter()));
    QObject::connect(b_import, SIGNAL(clicked()), this, SLOT(importparameter()));
    QObject::connect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(jumptoparameter(QListWidgetItem*)));
    setMouseTracking(true);

}

ParameterEditGui::~ParameterEditGui()
{
    QObject::disconnect(cb_parametertype, SIGNAL(currentIndexChanged ( const QString)), this, SLOT(changeCurrentParameterType()));
    QObject::disconnect(cb_filename, SIGNAL(currentIndexChanged ( const QString)), this, SLOT(changeCurrentFile(const QString)));
    QObject::disconnect(textEdit, SIGNAL(newparameter(QVector<double>, QString)), this, SLOT(newparameter(QVector<double>, QString)));
    QObject::disconnect(b_delete, SIGNAL(clicked()), this, SLOT(deleteparameter()));
    QObject::disconnect(b_import, SIGNAL(clicked()), this, SLOT(importparameter()));
    QObject::disconnect(listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(jumptoparameter(QListWidgetItem*)));
}

void ParameterEditGui::changeCurrentFile( const QString & text )
{
    currentfilename=text;
    loadnewtemplate = true;
    position=0;
    update(model);
}

void ParameterEditGui::changeCurrentParameterType()
{
    loadnewtemplate=true;
    currentfilename="";
    position=0;
    update(model);
}

void ParameterEditGui::importparameter()
{
    loadnewtemplate=true;
    QString savecurrentfilename=currentfilename;
    QString filename = QFileDialog::getOpenFileName(this,tr("Choose file"), "./", tr("*.*"));
    QString parameterstring = "";
    bool ok=true;

    QTime timer;
    timer.start();
    int elapsed = 500;
    ::ProgressBar progressbar(this);
    progressbar.setRange(0,4);
    progressbar.setVisible(true);
    progressbar.setProgress(1);

    if(Persistence::loadFileAsString(filename,&parameterstring))
    {
            progressbar.setProgress(2);
            if(timer.elapsed() > elapsed)
                progressbar.setVisible(true);

            if(cb_parametertype->currentIndex()==0)
            {
                ok=model->importInputParameters(currentfilename,filename);
            }

            if(cb_parametertype->currentIndex()==1)
            {
                ok=model->importIterationParameters(currentfilename,filename);
            }

            if(cb_parametertype->currentIndex()==2)
            {
                ok=model->importResultParameters(currentfilename,filename);
            }
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("Not able to open file"));
    }

    progressbar.setProgress(3);
    progressbar.setVisible(false);
    loadnewtemplate=true;
    currentfilename=savecurrentfilename;
    QCoreApplication::processEvents();
    progressbar.setProgress(4);
}

void ParameterEditGui::newparameter(QVector<double> values, QString name)
{
    QCoreApplication::processEvents();
    bool varok = false;


    if (!name.isEmpty())
    {
        if(model->isFreeParameterName(name))
        { 
            Variable *newvar = NULL;
            QVector<double> vec = values;

            if(cb_parametertype->currentIndex()==0)
            {
                newvar = new CalibrationVariable(name, vec);
                varok = model->addInputParameter(static_cast<CalibrationVariable*>(newvar), textEdit->document()->toPlainText(), currentfilename);
            }

            if(cb_parametertype->currentIndex()==1)
            {
                newvar = new Variable(name, vec,Variable::ITERATIONVARIABLE);
                varok = model->addIterationParameter(newvar, textEdit->toPlainText(), currentfilename);
            }

            if(cb_parametertype->currentIndex()==2)
            {
                newvar = new Variable(name, vec,Variable::RESULTVARIABLE);
                varok = model->addResultParameter(newvar, textEdit->toPlainText(), currentfilename);
            }

            if(!varok)
            {          
                delete newvar;
                QMessageBox::critical(this,tr("Error"),tr("Not able to create parameter"));
            }
        }
        else
        {
            update(model);
            QMessageBox::warning(this,tr("Warning"),tr("Parametername already exists"));
        }
    }
    loadnewtemplate=true;
    update(model);
    QCoreApplication::processEvents();
}

void ParameterEditGui::deleteparameter()
{
    ::ProgressBar progressbar(this);
    progressbar.setRange(0,3);

    if(listWidget->selectedItems().size()==0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("Please choose a parameter"));
    }
    else
    {
        progressbar.setVisible(true);
        progressbar.setProgress(1);
        if(cb_parametertype->currentIndex()==0)
        {
            model->deleteInputParameter(listWidget->selectedItems().at(0)->text(),currentfilename);
        }

        if(cb_parametertype->currentIndex()==1)
        {
            model->deleteIterationParameter(listWidget->selectedItems().at(0)->text(),currentfilename);
        }

        if(cb_parametertype->currentIndex()==2)
        {
            model->deleteResultParameter(listWidget->selectedItems().at(0)->text(),currentfilename);
        }
        progressbar.setProgress(2);
    }
    loadnewtemplate=true;
    update(model);
    QCoreApplication::processEvents();
    progressbar.setProgress(3);
}

void ParameterEditGui::update(Model* m)
{
    qDebug("ParameterEditGui::update()");
    if(model!=m)
    {
        model=m;
    }

    if(model==NULL)
        return;

    cb_filename->clear();
    listWidget->clear();
    
    if(loadnewtemplate)
        textEdit->setPlainText("");

    //setting iterationparameters
    line_iterationexec->setText(model->getIterationExec());
    line_iterationexecargument->setText(model->getIterationExecArgument());
    line_iterationexec_w->setText(model->getIterationWorkspace());

    //setting correct textfile
    if(cb_parametertype->currentIndex()==0)
    {
        if((currentfilename!="")&&(!model->existsInputFileName(currentfilename)))
        {
            position=0;
            currentfilename="";
        }
        else
        {
            if(currentfilename!="")
                cb_filename->addItem(currentfilename);
        }

        for(int counter=0; counter<model->numberOfInputFiles(); counter++)
        {
            if(counter==0&&currentfilename=="")
            {
                loadnewtemplate=true;
                currentfilename=model->getInputFileName(counter);
                cb_filename->addItem(currentfilename);
            }

            QString currentItem=model->getInputFileName(counter);

            if(currentItem!=currentfilename)
            {
                cb_filename->addItem(currentItem);
            }
        }

        if(currentfilename=="")
        {
            textEdit->setPlainText("");
        }
        else
        {
            if(loadnewtemplate)
                textEdit->setPlainText(model->getInputTemplate(currentfilename));

            for(int counter = 0; counter<model->numberOfInputParameters(currentfilename); counter++)
            {
                listWidget->addItem(model->getInputParameter(counter, currentfilename)->getName());
            }
        }
    }

    if(cb_parametertype->currentIndex()==1)
    {
        if((currentfilename!="")&&(!model->existsIterationFileName(currentfilename)))
        {
            position=0;
            currentfilename="";
        }
        else
        {
            if(currentfilename!="")
                cb_filename->addItem(currentfilename);
        }

        for(int counter=0; counter<model->numberOfIterationFiles(); counter++)
        {
            if(counter==0&&currentfilename=="")
            {
                currentfilename=model->getIterationFileName(counter);
                cb_filename->addItem(currentfilename);
            }

            QString currentItem=model->getIterationFileName(counter);

            if(currentItem!=currentfilename)
            {
                cb_filename->addItem(currentItem);
            }
        }

        if(currentfilename=="")
        {
           textEdit->setPlainText("");
        }
        else
        {
            if(loadnewtemplate)
                textEdit->setPlainText(model->getIterationTemplate(currentfilename));

            for(int counter = 0; counter<model->numberOfIterationParameters(currentfilename); counter++)
            {
                listWidget->addItem(model->getIterationParameter(counter, currentfilename)->getName());
            }
        }
    }

    if(cb_parametertype->currentIndex()==2)
    {
        if((currentfilename!="")&&(!model->existsResultFileName(currentfilename)))
        {
            position=0;
            currentfilename="";
        }
        else
        {
            if(currentfilename!="")
                cb_filename->addItem(currentfilename);
        }

        for(int counter=0; counter<model->numberOfResultFiles(); counter++)
        {
            if(counter==0&&currentfilename=="")
            {
                currentfilename=model->getResultFileName(counter);
                cb_filename->addItem(currentfilename);
            }

            QString currentItem=model->getResultFileName(counter);

            if(currentItem!=currentfilename)
            {
                cb_filename->addItem(currentItem);
            }
        }

        if(currentfilename=="")
        {
            textEdit->setPlainText("");
        }
        else
        {
            if(loadnewtemplate)
                textEdit->setPlainText(model->getResultTemplate(currentfilename));

            for(int counter = 0; counter<model->numberOfResultParameters(currentfilename); counter++)
            {
                listWidget->addItem(model->getResultParameter(counter, currentfilename)->getName());
            }
        }
    }

    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(position,QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    textEdit->setTextCursor(cursor);
    loadnewtemplate=false;
}

void ParameterEditGui::jumptoparameter( QListWidgetItem * item)
{
    int position = textEdit->toPlainText().indexOf(item->text(),0);
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(position,QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    textEdit->setTextCursor(cursor);
    this->position=position;
}

void ParameterEditGui::on_b_editpath_clicked()
{
    loadnewtemplate=true;
    bool ok;
    QString path = QInputDialog::getText(this, tr("Please enter a path:"),tr("Path:"), QLineEdit::Normal,currentfilename, &ok);

    if (ok && !path.isEmpty())
    {
        if(cb_parametertype->currentIndex()==0)
        {
            ok = model->changeInputFileName(currentfilename,path);
        }

        if(cb_parametertype->currentIndex()==1)
        {
            ok = model->changeIterationFileName(currentfilename,path);
        }

        if(cb_parametertype->currentIndex()==2)
        {
            ok = model->changeResultFileName(currentfilename,path);
        }
        
        if(!ok)
        {
            QMessageBox::warning(this,tr("Warning"),tr("Path already exists"));
            return;
        }

        currentfilename = path;
        update(model);
    }
}

void ParameterEditGui::on_b_newtemplate_clicked()
{
    loadnewtemplate=true;
    QString filename = QFileDialog::getOpenFileName(this,tr("Choose file"), "./", tr("*.*"));
    QString filestring = "";


    if(filename!="")
    {
        bool ok = false;

        if(!Persistence::loadFileAsString(filename,&filestring))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Not able to open file"));
            return;
        }

        if(cb_parametertype->currentIndex()==0)
        {
            ok=model->addInputFileName(filename,filestring);
        }

        if(cb_parametertype->currentIndex()==1)
        {
            ok=model->addIterationFileName(filename,filestring);
        }

        if(cb_parametertype->currentIndex()==2)
        {
            ok=model->addResultFileName(filename,filestring);
        }

        if(!ok)
            return;

        currentfilename = filename;
    }
}

void ParameterEditGui::on_b_deletetemplate_clicked()
{
    loadnewtemplate=true;
    if(currentfilename=="")
    {
        QMessageBox::warning(this,tr("Warning"),tr("Please choose a template"));
        return;
    }

    if(!model->removeFileName(currentfilename,model->getType(currentfilename)))
            qFatal("ParameterEditGui::on_b_deletetemplate_clicked()");

    update(model);
}

void ParameterEditGui::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void ParameterEditGui::on_line_iterationexec_returnPressed()
{
    model->setIterationExec(line_iterationexec->text());
}

void ParameterEditGui::on_line_iterationexecargument_returnPressed()
{
    model->setIterationExecArgument(line_iterationexecargument->text());
}

void ParameterEditGui::on_b_exec_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Choose a program"),model->getIterationWorkspace());

    if(file!="")
    {
        model->setIterationExec(file);
    }
}

void ParameterEditGui::on_b_exec_w_clicked()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Choose a directory"), "",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(dirname!="")
        model->setIterationWorkspace(dirname);
}

void ParameterEditGui::on_line_iterationexec_w_returnPressed()
{
    model->setIterationWorkspace(line_iterationexec_w->text());
}

