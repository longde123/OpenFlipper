/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "RPCWrappersHelper.hh"

namespace RPC
{

RPCHelper::RPCHelper()
{
}

RPCHelper::~RPCHelper()
{
}

QScriptValue RPCHelper::callFunction(QScriptEngine* _engine, const QString& _plugin, const QString& _functionName)
{
  QString command = _plugin+"."+_functionName+ "()";

  QScriptValue returnValue = _engine->evaluate(command);
  if ( returnValue.isError() ) {
    QString error = returnValue.toString();
    std::cerr << "Error : " << error.toStdString() << std::endl;
  }
  return returnValue;
}


QScriptValue RPCHelper::callFunction(QScriptEngine* _engine, const QString& _plugin, const QString& _functionName , const std::vector< QScriptValue >& _parameters)
{
  QString command = _plugin+"."+_functionName+ "(";
  // Make the parameters available in the scripting environment
  for ( uint i = 0 ; i < _parameters.size(); ++i ) {
    _engine->globalObject().setProperty("ParameterData" + QString::number(i) , _parameters[i] );
    command += "ParameterData" + QString::number(i);
    if ( (i + 1) < _parameters.size() )
      command+=",";
  }
  command += ")";

  QScriptValue returnValue = _engine->evaluate(command);
  if ( returnValue.isError() ) {
    QString error = returnValue.toString();
    std::cerr << "Error : " << error.toStdString() << std::endl;
  }
  return returnValue;
}

}