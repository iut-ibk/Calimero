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
#ifndef PERSISTENCE_H_INCLUDED
#define PERSISTENCE_H_INCLUDED

#include <QtXml>
#include <CalimeroGlob.h>
#include <string>
#include <vector>
#include <map>

class Calibration;
class IterationResult;
class QStatusBar;
class CalibrationEnv;

#define CALIMERO_VERSION "Calimero_v1.10.05"

class CALIMERO_PUBLIC Persistence
{
    private:
        Calibration *calibration;
        QStatusBar *status;

    public:
        Persistence(Calibration *calibration);
        ~Persistence();
        bool saveCalibration(QString filename,QStatusBar *status=0);
        bool loadCalibration(QString filename,QStatusBar *status=0);
        static std::string vectorToString(std::vector<double> vec);
        static std::vector<double> stringToVector(std::string stringvalue, bool *ok=NULL);

    private:
        //save functions
        bool saveResultHandler(QTextStream *out);
        bool saveTemplates(QTextStream *out);
        bool saveIterationResults(QTextStream *out);
        bool saveGroups(QTextStream *out);
        bool saveEnabledObjectiveFunctionParameters(QTextStream *out);
        bool saveModelSimulator(QTextStream *out);
        bool saveCalibrationAlgorithm(QTextStream *out);
        bool saveCalibrationParameters(QTextStream *out);
        bool saveIterationParameters(QTextStream *out);
        bool saveObservedParameters(QTextStream *out);
        bool saveObjectiveFunctionParameters(QTextStream *out);
        bool saveFunction(QString functionname, std::map<std::string, std::string> parameters, QString *string);
};

class CalimeroXmlHandler : public QXmlDefaultHandler
{
    private:
        std::map<int, IterationResult*> results;
        std::map<std::string, std::vector<double> > objectiveparameters;
        std::map<std::string, std::vector<double> > calibrationparameters;
        int iterationnr;
        std::string currentname;
        std::string function;
        std::map<std::string, std::string> settings;
        std::string resulthandlername;
        bool reshandlerenabled;
        std::string currentgroup;
        std::vector<std::pair<std::string,std::string> > connections;
        std::string templatepath;
        std::string templatename;
        std::string templatestring;
        int loadediterations;
        QStatusBar *status;

    public:
        CalimeroXmlHandler(Calibration *calibration,QStatusBar *status=0);
        bool fatalError ( const QXmlParseException & exception );
        bool startDocument();
        bool characters(const QString &ch);
        bool endDocument();
        bool endElement( const QString&, const QString&, const QString &name );
        bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs );

    private:
        Calibration *calibration;

    private:
        bool loadCalibrationParameters(const QXmlAttributes &attrs );
        bool loadIterationParameters(const QXmlAttributes &attrs );
        bool loadObservedParameters(const QXmlAttributes &attrs );
        bool loadIterationResults(const QString &name,const QXmlAttributes &attrs );
        bool loadObjectiveFunctionParameters(const QXmlAttributes &attrs );
};

#endif // PERSISTENCE_H_INCLUDED
