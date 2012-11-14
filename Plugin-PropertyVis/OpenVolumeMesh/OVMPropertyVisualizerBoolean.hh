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

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT

#ifndef OVM_PROPERTY_VISUALIZER_BOOLEAN_HH
#define OVM_PROPERTY_VISUALIZER_BOOLEAN_HH

#include "OVMPropertyVisualizer.hh"

#include "Widgets/BooleanWidget.hh"

#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>

#include <iostream>

template <typename MeshT>
class OVMPropertyVisualizerBoolean: public OVMPropertyVisualizer<MeshT>{

public:
    OVMPropertyVisualizerBoolean(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo);
    virtual ~OVMPropertyVisualizerBoolean(){}

protected:

    template <typename PropType, typename EntityIterator>
    void visualizeProp(PropType prop, EntityIterator e_begin, EntityIterator e_end);
    virtual void duplicateProperty();

    virtual void visualizeFaceProp();
    virtual void visualizeEdgeProp();
    virtual void visualizeHalfedgeProp();
    virtual void visualizeVertexProp();
    virtual void visualizeCellProp();
    virtual void visualizeHalffaceProp();

    virtual QString getCellPropertyText(unsigned int index);
    virtual QString getFacePropertyText(unsigned int index);
    virtual QString getHalffacePropertyText(unsigned int index);
    virtual QString getEdgePropertyText(unsigned int index);
    virtual QString getHalfedgePropertyText(unsigned int index);
    virtual QString getVertexPropertyText(unsigned int index);

    virtual void setCellPropertyFromText(unsigned int index, QString text);
    virtual void setFacePropertyFromText(unsigned int index, QString text);
    virtual void setHalffacePropertyFromText(unsigned int index, QString text);
    virtual void setEdgePropertyFromText(unsigned int index, QString text);
    virtual void setHalfedgePropertyFromText(unsigned int index, QString text);
    virtual void setVertexPropertyFromText(unsigned int index, QString text);


    virtual QString getPropertyText(unsigned int index);
};

#if defined(INCLUDE_TEMPLATES) && !defined(OVM_PROPERTY_VISUALIZER_BOOLEAN_CC)
#include "OVMPropertyVisualizerBooleanT.cc"
#endif

#endif /* OVM_PROPERTY_VISUALIZER_BOOLEAN_HH */

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
