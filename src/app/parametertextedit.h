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
