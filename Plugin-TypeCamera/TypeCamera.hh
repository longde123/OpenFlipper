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
 *   $Revision: 7683 $                                                       *
 *   $Author: kremer $                                                       *
 *   $Date: 2009-11-30 13:51:21 +0100 (Mo, 30. Nov 2009) $                   *
 *                                                                           *
\*===========================================================================*/


#ifndef TYPECAMERAPLUGIN_HH
#define TYPECAMERAPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>

#include <ObjectTypes/Camera/Camera.hh>

class TypeCameraPlugin : public QObject, BaseInterface, LoadSaveInterface, LoggingInterface, TypeInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(TypeInterface)

  signals:
    // Logging interface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // LoadSave Interface
    void emptyObjectAdded( int _id );

  private slots:

    void noguiSupported( ) {} ;

  public :

     ~TypeCameraPlugin() {};
     TypeCameraPlugin();

     QString name() { return (QString("TypeCamera")); };
     QString description( ) { return (QString(tr("Register Camera type"))); };
     
     bool registerType();

  public slots:

    // Base Interface
    QString version() { return QString("1.0"); };
    
    // Type Interface
    int addEmpty();
    DataType supportedType() { return DATA_CAMERA; }; 
    
  private:
    
    /// Return unique name for object
    QString get_unique_name(CameraObject* _object);

};

#endif //TYPECAMERAPLUGIN_HH
