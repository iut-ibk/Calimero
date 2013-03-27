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
#include <IterationResult.h>
#include <boost/foreach.hpp>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <Logger.h>
#include <Variable.h>
#include <Domain.h>
#include <assert.h>
#include <CalibrationEnv.h>
#include <Calibration.h>
#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMutexLocker>

CalimeroDB* CalimeroDB::instance = 0;

CalimeroDB::CalimeroDB()
{
    dblock = new QMutex(QMutex::Recursive);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    if(!CalibrationEnv::getInstance()->inMemory())
    {
        QString id = QUuid::createUuid().toString();
        QFile::remove(QDir::tempPath()+"/calimerodb" + id + ".cdb");
        db.setDatabaseName(QDir::tempPath()+"/calimerodb" + id + ".cdb");
    }
    else
    {
        db.setDatabaseName(":memory:");
    }

    if (!db.open())
    {
        Logger(Error) << "Unable to establish a database connection.\nThis example needs SQLite support.\nPlease read the Qt SQL driver documentation for information how to build it.";
        return;
    }
    else
    {
        Logger(Standard) << "Database connection established";
    }

    QSqlQuery query;
    if(!query.exec("create table calimeroresults(id varchar(200), "
               "iterationnr int, "
               "val text, "
               "PRIMARY KEY (id,iterationnr))"))
         Logger(Error) << "Cannot create DB";
}

void CalimeroDB::beginTransaction()
{
    dblock->lock();
    QSqlQuery query;
    if(!query.exec("BEGIN TRANSACTION"))
         Logger(Error) << "DB error in \"BEGIN TRANSACTION\" statement";
}

void CalimeroDB::endTransaction()
{
    QMutexLocker lock(dblock);
    QSqlQuery query;
    if(!query.exec("END TRANSACTION"))
         Logger(Error) << "DB error in \"END TRANSACTION\" statement";
    dblock->unlock();
}

CalimeroDB::~CalimeroDB()
{
    if(!CalibrationEnv::getInstance()->inMemory())
        QFile::remove(QDir::tempPath()+"/calimerodb.cdb");

    delete dblock;
}

bool CalimeroDB::saveVector(std::string name, std::vector<double> vector, int iteration)
{
    QByteArray qba;
    QBuffer buffer(&qba);
    buffer.open(QIODevice::WriteOnly);
    QDataStream datastream(&buffer);
    datastream  << QVector<double>::fromStdVector(vector);
    //qba=qba.toHex();

    QMutexLocker lock(dblock);
    QSqlQuery query;
    query.prepare("INSERT INTO calimeroresults (id, iterationnr, val) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(QString::fromStdString(name));
    query.addBindValue(iteration);
    query.addBindValue(qba);
    if(!query.exec())
        Logger(Error) << "Cannot insert vector in calimero DB";
    return true;
}

std::vector<double> CalimeroDB::getVector(std::string name, int iteration)
{
    QMutexLocker lock(dblock);
    QString q = "SELECT val FROM calimeroresults WHERE id=\"" + QString::fromStdString(name) + "\" AND iterationnr=" + QString::number(iteration);
    QSqlQuery query(q);

    query.next();
    if(query.isValid())
    {
        QVector<double> result;
        QByteArray qba = query.value(0).toByteArray();
        //qba = QByteArray::fromHex(qba);
        QBuffer b(&qba);
        b.open(QBuffer::ReadOnly);
        QDataStream s(&b);
        s >> result;
        return result.toStdVector();
    }

    Logger(Error) << "Cannot find vector in calimero DB";
    return std::vector<double>(1,0);
}

bool CalimeroDB::removeVector(std::string name, int iteration)
{
    QMutexLocker lock(dblock);
    QString q = "DELETE FROM calimeroresults WHERE id=\"" + QString::fromStdString(name) + "\" AND iterationnr=" + QString::number(iteration);
    QSqlQuery query(q);
    if(!query.exec())
    {
        Logger(Error) << "Cannot delete vector in calimero DB";
        return false;
    }

    return true;
}

CalimeroDB* CalimeroDB::getInstance()
{
    if(!CalimeroDB::instance)
        CalimeroDB::instance = new CalimeroDB();
    return CalimeroDB::instance;
}

IterationResult::IterationResult(int iterationnum)
{
    if(iterationnum < 0)
    {
       Logger(Error) << "Iteration number invalid";
       abort();
    }

    iterationnumber=iterationnum;
    complete=0;
}

IterationResult::IterationResult(int iterationnum,
                map<string, vector<double> > calibrationparameters,
                map<string, vector<double> > objectivefunctionparameters)
{
    CalimeroDB* db = CalimeroDB::getInstance();

    if(iterationnum < 0)
    {
       Logger(Error) << "Iteration number invalid";
       abort();
    }

    this->iterationnumber=iterationnum;

    std::pair<string, vector<double> > pair;

    db->beginTransaction();
    BOOST_FOREACH(pair, objectivefunctionparameters)
    {
        db->saveVector(pair.first,pair.second,iterationnum);
        this->objectivefucntionparameters.append(QString::fromStdString(pair.first));
    }

    BOOST_FOREACH(pair, calibrationparameters)
    {
        db->saveVector(pair.first,pair.second,iterationnum);
        this->calibrationparameters.append(QString::fromStdString(pair.first));
    }
    db->endTransaction();
    complete = 1;
}

IterationResult::~IterationResult()
{
    CalimeroDB* db = CalimeroDB::getInstance();

    db->beginTransaction();
    for(int index=0; index < calibrationparameters.size(); index++)
        db->removeVector(calibrationparameters.at(index).toStdString(),iterationnumber);

    for(int index=0; index < objectivefucntionparameters.size(); index++)
        db->removeVector(objectivefucntionparameters.at(index).toStdString(),iterationnumber);
    db->endTransaction();
}

void IterationResult::setResults(Domain *dom)
{
    CalimeroDB* db = CalimeroDB::getInstance();
    assert(!complete);

    vector<Variable*> tmpvec;

    //save all calibration parameters
    tmpvec = dom->getAllPars(CALIBRATIONVARIABLE);
    BOOST_FOREACH(Variable *var, tmpvec)
    {
        db->saveVector(var->getName(),var->getValues(),iterationnumber);
        this->calibrationparameters.append(QString::fromStdString(var->getName()));
    }

    //save all objective function parameters
    tmpvec = dom->getAllPars(OBJECTIVEFUNCTIONVARIABLE);
    BOOST_FOREACH(Variable *var, tmpvec)
    {
        db->saveVector(var->getName(),var->getValues(),iterationnumber);
        this->objectivefucntionparameters.append(QString::fromStdString(var->getName()));
    }

    Logger(Debug) << "Iteration " << iterationnumber << "is complete";
    complete=1;
}

bool IterationResult::isComplete()
{
    return complete;
}

vector<double> IterationResult::getResults(string name)
{
    assert(complete);

    vector<double> result;

    if(calibrationparameters.contains(QString::fromStdString(name)))
    {
        result = getCalibrationParameterResults(name);
        return result;
    }

    if(objectivefucntionparameters.contains(QString::fromStdString(name)))
    {
        result = getObjectiveFunctionParameterResults(name);
        return result;
    }

    Logger(Error) << "Result container does not contain parameter [" << name << "]";
    return vector<double>();
}

vector<double> IterationResult::getCalibrationParameterResults(string name)
{
    assert(complete);

    if(!calibrationparameters.contains(QString::fromStdString(name)))
    {
        Logger(Error) << "Result container does not contain parameter [" << name << "]";
        return vector<double>();
    }

    CalimeroDB* db = CalimeroDB::getInstance();
    return db->getVector(name,iterationnumber);
}

vector<double> IterationResult::getObjectiveFunctionParameterResults(string name)
{
    assert(complete);

    if(!objectivefucntionparameters.contains(QString::fromStdString(name)))
    {
        Logger(Error) << "Result container does not contain parameter [" << name << "]";
        return vector<double>();
    }

    CalimeroDB* db = CalimeroDB::getInstance();
    return db->getVector(name,iterationnumber);
}

int IterationResult::getIterationNumber()
{
    return iterationnumber;
}

vector<string> IterationResult::getNamesOfObjectiveFunctionParameters()
{
    vector<string> result;

    for(int index=0; index < objectivefucntionparameters.size(); index++)
        result.push_back(objectivefucntionparameters.at(index).toStdString());

    return result;
}

vector<string> IterationResult::getNamesOfCalibrationParameters()
{
    vector<string> result;

    for(int index=0; index < calibrationparameters.size(); index++)
        result.push_back(calibrationparameters.at(index).toStdString());

    return result;;
}
