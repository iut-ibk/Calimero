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
#ifndef EXTERNALPARAMETERREGISTRY_H
#define EXTERNALPARAMETERREGISTRY_H

#include <string>
#include <map>
#include <CalimeroGlob.h>
#include <Variable.h>

class Domain;
class Calibration;

using namespace std;

class CALIMERO_PUBLIC ExternalParameterRegistry
{
private:
    //[templatename][template]
    map<string,string> regtemplates;
    //[templatename][path]
    map<string,string> templatepaths;
    //[parametername][templatename]
    map<string, string> regparameters;
    //[templatename][VariableType]
    map<string, VARTYPE> types;

private:
    void destroy(map<string, vector<double>* > extracted);
public:
    ExternalParameterRegistry();
    ExternalParameterRegistry(const ExternalParameterRegistry &oldreg);
    bool registerTemplate(const string &name, const string &templatepath, const string &templatestring, Calibration* calibration, VARTYPE type);
    bool registerParameter(const string &parametername, const string &templatename, Domain *domain);
    bool containsParameter(const string &paramtername);
    bool containsParameter(const string &parametername, const string &templatename);
    bool updateTemplate(const string &templatename, string templatestring, Calibration* calibration);
    bool updatePath(const string &templatename, string path);
    bool updateParameters(Domain *domain, int iteration);
    bool updateParameters(Domain *domain, const string &templatename, const string &values);
    bool deleteTemplate(const string &templatename);
    bool deleteParameter(const string &parametername);
    bool createValueFile(const string &templatename, Domain *domain, int iteration);
    bool createValueFile(const string &templatename, Domain *domain, const string &filepath, int iteration);
    bool createValueFiles(Domain *dom, int iteration);
    string getPath(const string &templatename);
    string getTemplate(const string &templatename);
    vector<string> getAllTemplateNames();
    vector<string> getTemplateNames(VARTYPE type);
};

#endif // EXTERNALPARAMETERREGISTRY_H
