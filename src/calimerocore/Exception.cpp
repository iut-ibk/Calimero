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
#include <Exception.h>
#include <Logger.h>
#include <boost/algorithm/string.hpp>
#include <vector>

using namespace boost;

PythonException::PythonException(std::string error, std::string msg) : CalimeroException(msg){
        exceptiontype=BOOSTPYTHONEXCEPTION;
        std::vector<string> values;
	algorithm::split(values, error, algorithm::is_any_of("|"));
	assert(values.size() == 3);
	type = values[0];
	value = values[1];
	traceback = values[2];
}
