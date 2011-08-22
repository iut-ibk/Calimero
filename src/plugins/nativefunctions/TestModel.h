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
#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <vector>
#include <IModelSimulator.h>

CALIMERO_DECLARE_MODELSIMULATOR(TestModel)
public:
    TestModel();
    bool exec(Domain *dom);
};

CALIMERO_DECLARE_MODELSIMULATOR(VectorModel)
public:
    VectorModel();
    bool exec(Domain *dom);
};

double fac(int value);

CALIMERO_DECLARE_MODELSIMULATOR(Schmutzstoffmodell)
public:
    Schmutzstoffmodell();
    bool exec(Domain *dom);
};


#endif // TESTMODEL_H
