/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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




//=============================================================================
//
//  Plugin Functions for Skeleton
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "Skeleton.hh"

namespace PluginFunctions {

// ===============================================================================
// Get objects
// ===============================================================================

bool getObject(  int _identifier , SkeletonObject*& _object ) {
  if  (_identifier == -1)
    return false;

  // Get object by using the map accelerated plugin function
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_identifier,object);

  _object = dynamic_cast< SkeletonObject* >(object);
  return ( _object != 0 );
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

Skeleton* skeleton( BaseObjectData* _object ) {
  if ( _object && _object->dataType(DATA_SKELETON) ) {
    SkeletonObject* object = dynamic_cast< SkeletonObject* >(_object);
    return object->skeleton();
  } else
    return NULL;
}

Skeleton* skeleton(  int _identifier ) {

  SkeletonObject* object = skeletonObject(_identifier);

  if ( object  ) {
    return object->skeleton();
  } else
    return NULL;
}


SkeletonObject* skeletonObject( BaseObjectData* _object ) {
  if ( !_object || ! _object->dataType(DATA_SKELETON) )
    return NULL;
  return dynamic_cast< SkeletonObject* >( _object );
}


SkeletonObject* skeletonObject( int _identifier ) {
  SkeletonObject* object;
  if ( ! getObject( _identifier, object ) )
    return 0;

  if ( !object || ! object->dataType(DATA_SKELETON) )
    return NULL;
  return dynamic_cast< SkeletonObject* >( object );
}


}
