/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 7874 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-12-14 16:08:50 +0100 (Mo, 14. Dez 2009) $                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  Wrapper for IdList ( std::vector< int > ) - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "DataTypeWrapper.hh"

//== IMPLEMENTATION ==========================================================

QScriptValue toScriptValueDataType(QScriptEngine *engine, const DataType &s)
{
  QScriptValue obj = engine->newObject();
  obj.setProperty("type", QScriptValue(engine, s ));
  std::cerr << "toScriptValueDataType " << s << std::endl;
  return obj;
}

void fromScriptValueDataType(const QScriptValue &obj, DataType &s)
{
  s = obj.property("type").toNumber();

  std::cerr << "fromScriptValueDataType " << s << std::endl;
}

QScriptValue createDataType(QScriptContext *context, QScriptEngine *engine)
{
  std::cerr << "createDataType " << std::endl;
  
  DataType s;
  
  QScriptValue callee = context->callee();
  
  // If arguments are given, use them for initialization otherwise
  // initialize with 0
  if (context->argumentCount() == 1) {     
    s = context->argument(0).toNumber();
  } else {
    s = DATA_UNKNOWN;
  }


  std::cerr << "s set to " << s << std::endl;
  
  return engine->toScriptValue( s );
}

QScriptValue DataTypeToString(QScriptContext *context, QScriptEngine *engine)
{
  DataType s = context->thisObject().property("type").toNumber();

  std::cerr << "DataTypeToString " << s << std::endl;
  std::cerr << "DataTypeToString " << typeName(s).toStdString() << std::endl;
  
  return QScriptValue(engine, typeName(s) );
}

//=============================================================================

