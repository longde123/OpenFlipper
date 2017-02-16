/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openflipper.org                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenFlipper.                                         *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
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
//  MyTypes
//
//=============================================================================

#define POLYLINEOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include "PolyLine.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** Constructor for PolyLine Objects. This object class gets a Separator Node giving
*  the root node to which it should be connected. The line is generated internally
*  and all nodes for visualization will be added below the scenegraph node.\n
*  You dont need to create an object of this type manually. Use
*  PluginFunctions::addPolyLine instead. ( see Types.hh::DataType )
*/
PolyLineObject::PolyLineObject() :
  BaseObjectData( ),
  line_(NULL),
  lineNode_(NULL)
{
  setDataType(DATA_POLY_LINE);
  init();
}

//=============================================================================


/**
 * Copy Constructor - generates a copy of the given object
 */
PolyLineObject::PolyLineObject(const PolyLineObject & _object) :
  BaseObjectData(_object)
{

    init(_object.line_);

    setName( name() );
}

/** Destructor for PolyLine Objects. The destructor deletes the Line and all
*  Scenegraph nodes associated with the Line or the object.
*/
PolyLineObject::~PolyLineObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the line below and
  // they do something with the polyline in the destructor of their
  // perObjectData.
  deleteData();

  // Delete the Mesh only, if this object contains a mesh
  if ( line_ != NULL)  {
    delete line_;
    line_ = NULL;
  } else {
    std::cerr << "Destructor error : Line already deleted" << std::endl;
  }

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  lineNode_    = NULL;
}

/** Cleanup Function for Line Objects. Deletes the contents of the whole object and
* calls PolyLineObject::init afterwards.
*/
void PolyLineObject::cleanup() {
  // Delete the Line only, if this object contains a line
  if ( line_ != NULL)  {
    delete line_;
    line_ = NULL;
  } else {
    std::cerr << "Cleanup error : Line already deleted" << std::endl;
  }

  BaseObjectData::cleanup();

  lineNode_   = NULL;

  setDataType(DATA_POLY_LINE);

  init();

}

/**
 * Generate a copy
 */
BaseObject* PolyLineObject::copy() {
    PolyLineObject* object = new PolyLineObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initalizes the line object. It creates the scenegraph nodes,
*  the line.
*/
void PolyLineObject::init(PolyLine* _line) {

  if (_line == 0)
    line_ = new PolyLine();
  else
    line_ = new PolyLine(*_line);

  // request default properties
  line()->request_vertex_selections();
  line()->request_edge_selections();
  line()->request_vertex_vhandles();
  line()->request_vertex_ehandles();
  line()->request_vertex_fhandles();


  if ( materialNode() == NULL)
    std::cerr << "Error when creating Line Object! materialNode is NULL!" << std::endl;

  lineNode_    = new ACG::SceneGraph::PolyLineNodeT< PolyLine >(*line_, materialNode() , "NEW LineNode");
  
  // Set default material of the polyLine
  materialNode()->set_random_color();
  materialNode()->set_line_width(3.0);
  materialNode()->set_point_size(8.0);
  materialNode()->set_round_points(true);

}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void PolyLineObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("LineNode for Line "     + _name.toUtf8() );
  lineNode_->name( nodename );
}

// ===============================================================================
// Content
// ===============================================================================

/** Get a pointer to the objects line.
* @return Pointer to the line
*/
PolyLine* PolyLineObject::line() {
  return line_;
}


void PolyLineObject::update(UpdateType _type) {
  lineNode_->update();
}

// ===============================================================================
// Visualization
// ===============================================================================

ACG::SceneGraph::PolyLineNodeT< PolyLine >* PolyLineObject::lineNode() {
  return lineNode_;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString PolyLineObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_POLY_LINE ) )
    output += "Object Contains PolyLine : ";

  output += QString::number( line()->n_vertices() ) + " vertices, ";
  output += QString::number( line()->n_edges() ) += " edges and is ";
  if ( line()->is_closed() )
    output += "closed.\n";
  else
    output += "open.\n";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the polyLineNode of the object has been picked.
*
* @param _node_idx Index of the picked line node
* @return bool if the lineNode of this object is the picking target.
*/
bool PolyLineObject::picked( uint _node_idx ) {
  return ( _node_idx == lineNode_->id() );
}

void PolyLineObject::enablePicking( bool _enable ) {
  lineNode_->enablePicking( _enable );
}

bool PolyLineObject::pickingEnabled() {
  return lineNode_->pickingEnabled();
}

/// Refine picking on triangle meshes
ACG::Vec3d PolyLineObject::refinePick(ACG::SceneGraph::PickTarget _pickTarget,
                             const ACG::Vec3d _hitPoint,
                             const ACG::Vec3d _start ,
                             const ACG::Vec3d _dir,
                             const unsigned int _targetIdx )
{
  if ( _pickTarget == ACG::SceneGraph::PICK_FACE) {
    //don't refine polyLine faces
    return _hitPoint;
  }

  if ( _pickTarget == ACG::SceneGraph::PICK_EDGE) {
    // get picked edge handle

    int eh;

      eh = _targetIdx;


    if(eh >= 0 && eh < (int)line()->n_edges())
    {
      //get vertices of the edge
      ACG::Vec3d edgeStart = line()->point((eh+1)%line()->n_vertices());
      ACG::Vec3d edgeEnd = line()->point(eh);

      //retrieve the point on the edge that is closest to the backprojected hitpoint
      ACG::Vec3d hitPointNew;
      ACG::Geometry::distPointLineSquared(_hitPoint,edgeStart,edgeEnd,&hitPointNew);


      return hitPointNew;
    }
  }

  if ( _pickTarget == ACG::SceneGraph::PICK_VERTEX) {
    // get picked vertex handle
    int vh = _targetIdx;
    if(vh>=0 && vh < (int)line()->n_vertices())
    {
      ACG::Vec3d hitpointNew = line()->point(vh);

      //just return the vertex position
      return hitpointNew;
    }
  }

  return _hitPoint;
}

//=============================================================================