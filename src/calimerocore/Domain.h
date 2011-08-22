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
#ifndef DOMAIN_H
#define DOMAIN_H

#include <map>
#include <string>
#include <CalimeroGlob.h>
#include <Variable.h>
#include <vector>

using namespace std;

class CALIMERO_PUBLIC Domain
{
private:
    map<string, Variable*> members;
public:
    Domain();
    Domain(const Domain &olddomain);
    ~Domain();
    Variable* getPar(const string &name);
    bool contains(string var);
    void setPar(Variable* var);
    bool removePar(const string &name);
    vector<Variable*> getAllPars(const VARTYPE &type);
};

#endif // DOMAIN_H
