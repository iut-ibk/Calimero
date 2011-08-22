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
#ifndef CALIBRATIONPARAMETER_H_INCLUDED
#define CALIBRATIONPARAMETER_H_INCLUDED

#include <Variable.h>

class CALIMERO_PUBLIC CalibrationVariable : public Variable
{
    private:
        vector<double> initvalues;
        double min;
        double max;
        double step;

    private:
        bool checkrange(const vector<double> vec, double min, double max);

    public:
        CalibrationVariable(string Name, vector<double> value);
        CalibrationVariable(const CalibrationVariable &oldvar);
        virtual ~CalibrationVariable();
        vector<double> getInitValues();
        void setInitValues(vector<double> value);
        bool setValues( vector<double> vec);
        void setMax(double value);
        double getMax();
        void setMin(double value);
        double getMin();
        double getStep();
        void setStep(double step);
};



#endif // CALIBRATIONPARAMETER_H_INCLUDED
