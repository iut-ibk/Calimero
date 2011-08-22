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
#ifndef ITERATIONRESULT_H
#define ITERATIONRESULT_H

#include <vector>
#include <map>
#include <string>
#include <CalimeroGlob.h>
#include <Logger.h>
#include <boost/shared_ptr.hpp>
#include <QStringList>

class ObjectiveFunctionVariable;
class Variable;
class CalibrationVariable;
class Domain;

using namespace std;
using namespace boost;

class CALIMERO_PUBLIC IterationResult
{
private:
    int complete;
    int iterationnumber;
    QStringList calibrationparameters;
    QStringList objectivefucntionparameters;

public:
    ~IterationResult();
    IterationResult(int iterationnum);
    IterationResult(int iterationnum,
                    map<string, vector<double> > calibrationparameters,
                    map<string, vector<double> > objectivefunctionparameters);
    void setResults(Domain *dom);
    bool isComplete();
    int getIterationNumber();
    vector<double> getResults(string name);;
    vector<double> getCalibrationParameterResults(string name);
    vector<double> getObjectiveFunctionParameterResults(string name);
    vector<string> getNamesOfObjectiveFunctionParameters();
    vector<string> getNamesOfCalibrationParameters();
};

class CalimeroDB
{
private:
    static CalimeroDB* instance;
private:
    CalimeroDB();
public:
    ~CalimeroDB();
    static CalimeroDB* getInstance();
    bool saveVector(std::string name, std::vector<double> vector, int iteration);
    void beginTransaction();
    void endTransaction();
    bool removeVector(std::string name, int iteration);
    std::vector<double> getVector(std::string name, int iteration);
};

#endif // ITERATIONRESULT_H
