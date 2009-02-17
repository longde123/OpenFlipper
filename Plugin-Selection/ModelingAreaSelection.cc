//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#include "SelectionPlugin.hh"

#include <iostream>

#include <MeshTools/MeshSelectionT.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


//=========================================================
//==== Modeling Area selections
//=========================================================

void SelectionPlugin::selectModelingVertices( int objectId , idList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectModelingVertices : unable to get object" );
    return;
  }

  if ( _vertexList.size() == 0 )
    return;

  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH :
      MeshSelection::setArea(PluginFunctions::triMesh(object) , _vertexList , AREA, true);
      update_regions( PluginFunctions::triMesh(object) );
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , _vertexList , AREA, true);
      update_regions( PluginFunctions::polyMesh(object) );
      break;
    default:
      emit log(LOGERR,"selectModelingVertices : Unsupported object Type" );
      return;
  }

  QString selection = "selectModelingVertices( ObjectId , [ " + QString::number(_vertexList[0]);

  for ( uint i = 1 ; i < _vertexList.size(); ++i) {
    selection +=  " , " + QString::number(_vertexList[i]);
  }

  selection += " ] )";

  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::unselectModelingVertices( int objectId , idList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"unselectModelingVertices : unable to get object" );
    return;
  }

  if ( _vertexList.size() == 0 )
    return;

  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH :
      MeshSelection::setArea(PluginFunctions::triMesh(object) , _vertexList , AREA, false);
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , _vertexList , AREA, false);
      break;
    default:
      emit log(LOGERR,"unselectModelingVertices : Unsupported object Type" );
      return;
  }

  QString selection = "unselectModelingVertices( ObjectId , [ " + QString::number(_vertexList[0]);

  for ( uint i = 1 ; i < _vertexList.size(); ++i) {
    selection +=  " , " + QString::number(_vertexList[i]);
  }

  selection += " ] )";

  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::clearModelingVertices( int objectId ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"clearModelingVertices : unable to get object" );
    return;
  }

  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH :
      MeshSelection::setArea(PluginFunctions::triMesh(object) , AREA, false);
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , AREA, false);
      break;
    default:
      emit log(LOGERR,"clearModelingVertices : Unsupported object Type" );
      return;
  }

  object->update();
  emit scriptInfo( "clearModelingVertices( ObjectId )" );
}

//=========================================================

void SelectionPlugin::setAllModelingVertices( int objectId  ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"setAllModelingVertices : unable to get object" );
    return;
  }

  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH :
      MeshSelection::setArea(PluginFunctions::triMesh(object) , AREA, true);
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , AREA, true);
      break;
    default:
      emit log(LOGERR,"setAllModelingVertices : Unsupported object Type" );
      return;
  }

  object->update();
  emit scriptInfo( "setAllModelingVertices( ObjectId )" );
}

//=========================================================
idList SelectionPlugin::getModelingVertices( int objectId  ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"getModelingVertices : unable to get object" );
    return idList(0);
  }

  emit scriptInfo( "getModelingVertices( ObjectId )" );

  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH :
      return MeshSelection::getArea(PluginFunctions::triMesh(object) , AREA);
      break;
    case DATA_POLY_MESH :
      return MeshSelection::getArea(PluginFunctions::polyMesh(object) , AREA);
      break;
    default:
      emit log(LOGERR,"getModelingVertices : Unsupported object Type" );
      return idList(0);
  }

  return idList(0);
}

//=========================================================

void SelectionPlugin::loadFlipperModelingSelection( int _objectId , QString _filename ) {
  QFile file(_filename);

  if ( ! file.exists() ) {
    emit log(LOGERR,"Unable to find file : " + _filename );
    return;
  }

   if (file.open(QFile::ReadOnly)) {
    QTextStream input(&file);

    QString header = input.readLine();

    if ( !header.contains("Selection") ) {
       emit log(LOGERR,"Wrong file header! should be Selection but is " + header );
       return;
    }

    header = input.readLine();

    bool ok = false;

//     uint vertexCount = header.toUInt(&ok);

    if ( !ok ) {
       emit log(LOGERR,"Unable to parse header. Cant get vertex count from string : " + header );
       return;
    }

    idList handleVertices;
    idList modelingVertices;

    uint vertexId = 0;

    do {
      // Split into two substrings
      QStringList inputList = input.readLine().split(" ");

      if ( inputList.size() != 2 ) {
        emit log(LOGERR,"Unable to parse entry at vertex index " + QString::number( vertexId ) );
        return;
      }

      if ( inputList[0] == "1" )
        modelingVertices.push_back(vertexId);

      if ( inputList[1] == "1" )
        handleVertices.push_back(vertexId);

      ++vertexId;

    } while (!input.atEnd());

    clearModelingVertices(_objectId);
    selectModelingVertices(_objectId,modelingVertices);

    clearHandleVertices(_objectId);
    selectHandleVertices(_objectId,handleVertices);

  } else
    emit log(LOGERR,"Unable to open selection file!");


}
