/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         * 
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
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *             
 *   $Revision: 137 $                                                         *
 *   $Date: 2009-06-04 10:46:29 +0200 (Do, 04. Jun 2009) $                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS StripProcessorT - IMPLEMENTATION
//
//=============================================================================

#define ACG_STRIP_PROCESSOR_C

//== INCLUDES =================================================================

#include <list>
#include <iostream>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {
    
  //== IMPLEMENTATION ==========================================================

template <class Mesh>
StripProcessorT<Mesh>::
StripProcessorT(Mesh& _mesh) :
    mesh_(_mesh)
{

}

template <class Mesh>
StripProcessorT<Mesh>::
~StripProcessorT() {

}

template <class Mesh>
unsigned int
StripProcessorT<Mesh>::
stripify()
{
  // preprocess:  add new properties
  mesh_.add_property( processed_ );
  mesh_.add_property( used_ );
  mesh_.request_face_status();

  // build strips
  clear();
  buildStrips();

  // postprocess:  remove properties
  mesh_.remove_property(processed_);
  mesh_.remove_property(used_);
  mesh_.release_face_status();

  return nStrips();
}


//-----------------------------------------------------------------------------

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStrips()
{
  if ( mesh_.is_trimesh() )
    buildStripsTriMesh();
  else
    buildStripsPolyMesh();
}

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripsPolyMesh()
{
  std::cerr << "Error! Strip processor not implemented for poly meshes!" << std::endl;
}

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripsTriMesh()
{
  Strip                           experiments[3];
  typename Mesh::HalfedgeHandle   h[3];
  unsigned int                    best_idx, best_length, length;
  FaceHandles                     faces[3];
  typename FaceHandles::iterator  fh_it, fh_end;
  typename Mesh::FaceIter         f_it, f_end=mesh_.faces_end();



  // init faces to be un-processed and un-used
  // deleted or hidden faces are marked processed
  if (mesh_.has_face_status())
  {
    for (f_it=mesh_.faces_begin(); f_it!=f_end; ++f_it)
      if (mesh_.status(f_it).hidden() || mesh_.status(f_it).deleted())
        processed(f_it) = used(f_it) = true;
      else
        processed(f_it) = used(f_it) = false;
  }
  else
  {
    for (f_it=mesh_.faces_begin(); f_it!=f_end; ++f_it)
      processed(f_it) = used(f_it) = false;
  }



  for (f_it=mesh_.faces_begin(); true; )
  {
    // find start face
    for (; f_it!=f_end; ++f_it)
      if (!processed(f_it))
        break;
    if (f_it==f_end) break; // stop if all have been processed


    // collect starting halfedges
    h[0] = mesh_.halfedge_handle(f_it.handle());
    h[1] = mesh_.next_halfedge_handle(h[0]);
    h[2] = mesh_.next_halfedge_handle(h[1]);


    // build 3 strips, take best one
    best_length = best_idx = 0;
    for (unsigned int i=0; i<3; ++i)
    {
      buildStrip(h[i], experiments[i], faces[i]);
      if ((length = experiments[i].indexArray.size()) > best_length)
      {
        best_length = length;
        best_idx    = i;
      }

      for (fh_it=faces[i].begin(), fh_end=faces[i].end();
           fh_it!=fh_end; ++fh_it)
        used(*fh_it) = false;
    }


    // update processed status
    fh_it  = faces[best_idx].begin();
    fh_end = faces[best_idx].end();
    for (; fh_it!=fh_end; ++fh_it)
      processed(*fh_it) = true;



    // add best strip to strip-list
    strips_.push_back(experiments[best_idx]);
  }
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
StripProcessorT<Mesh>::
buildStrip(typename Mesh::HalfedgeHandle _start_hh,
            Strip& _strip,
            FaceHandles& _faces)
{
  std::list<unsigned int>  strip;
  typename Mesh::HalfedgeHandle   hh;
  typename Mesh::FaceHandle       fh;


  // reset face list
  _faces.clear();


  // init strip
  strip.push_back(mesh_.from_vertex_handle(_start_hh).idx());
  strip.push_back(mesh_.to_vertex_handle(_start_hh).idx());


  // walk along the strip: 1st direction
  hh = mesh_.prev_halfedge_handle(mesh_.opposite_halfedge_handle(_start_hh));
  while (1)
  {
    // go right
    hh = mesh_.next_halfedge_handle(hh);
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_back(mesh_.to_vertex_handle(hh).idx());

    // go left
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_back(mesh_.to_vertex_handle(hh).idx());
  }


  // walk along the strip: 2nd direction
  bool flip(false);
  hh = mesh_.prev_halfedge_handle(_start_hh);
  while (1)
  {
    // go right
    hh = mesh_.next_halfedge_handle(hh);
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_front(mesh_.to_vertex_handle(hh).idx());
    flip = true;

    // go left
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break;
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_front(mesh_.to_vertex_handle(hh).idx());
    flip = false;
  }

  if (flip) strip.push_front(strip.front());



  // copy final strip to _strip
  _strip.indexArray.clear();
  _strip.indexArray.reserve(strip.size());
  std::copy(strip.begin(), strip.end(), std::back_inserter(_strip.indexArray));
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingVertices(ACG::GLState&  _state, uint _offset) {
  if ( mesh_.is_trimesh() )
    updatePickingVerticesTrimesh(_state, _offset );
  else
    updatePickingVerticesPolymesh(_state, _offset);
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingVerticesTrimesh(ACG::GLState& _state , uint _offset) {
  std::cerr << "StripProcessor updatePickingVerticesTrimesh with offset : " << _offset << std::endl;
  
  GLuint                         idx(0);
  
  // Adjust size of the color buffer to the number of vertices in the mesh
  pickVertexColorBuf_.resize( mesh_.n_vertices() );
  
  // Get the right picking colors from the gl state and add them per vertex to the color buffer
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()), v_end(mesh_.vertices_end());
  for (; v_it!=v_end; ++v_it, ++idx) 
    pickVertexColorBuf_[idx] = _state.pick_get_name_color(idx + _offset);
  
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingVerticesPolymesh(ACG::GLState& _state,  uint _offset) {
  std::cerr << "StripProcessor updatePickingVerticesPolymesh polymesh not yet implemented!" << std::endl;
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingEdges(ACG::GLState& _state,  uint _offset ) {
  if ( mesh_.is_trimesh() )
    updatePickingEdgesTrimesh(_state, _offset);
  else
    updatePickingEdgesPolymesh(_state, _offset);
}
    
template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingEdgesTrimesh(ACG::GLState& _state, uint _offset) {
  std::cerr << "StripProcessor updatePickingEdgesTrimesh with offset : " << _offset << std::endl;
  
  pickEdgeColorBuf_.resize(mesh_.n_edges() * 2);
  pickEdgeVertexBuf_.resize(mesh_.n_edges() * 2);
  
  int idx(0);
  
  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it)
  {
    pickEdgeColorBuf_[idx]    = _state.pick_get_name_color (e_it.handle().idx() + _offset);
    pickEdgeColorBuf_[idx+1]  = _state.pick_get_name_color (e_it.handle().idx() + _offset);
    pickEdgeVertexBuf_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0)));
    pickEdgeVertexBuf_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1)));
    idx += 2;
  }
  
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingEdgesPolymesh(ACG::GLState& _state , uint _offset) {
  std::cerr << "StripProcessor updatePickingEdgesPolymesh polymesh not yet implemented!" << std::endl;
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingFaces(ACG::GLState& _state ) {
  if ( mesh_.is_trimesh() )
    updatePickingFacesTrimesh(_state);
  else
    updatePickingFacesPolymesh(_state);
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingFacesTrimesh(ACG::GLState& _state ) {
  
  std::cerr << "StripProcessor updatePickingFacesTrimesh "<< std::endl;
  
  pickFaceColorBuf_.resize(mesh_.n_faces() * 3);
  pickFaceVertexBuf_.resize(mesh_.n_faces() * 3);
  
  int idx(0);
  
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()), f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;
  
  for (; f_it!=f_end; ++f_it)
  {
    pickFaceColorBuf_[idx]    = _state.pick_get_name_color (f_it.handle().idx());
    pickFaceColorBuf_[idx+1]  = _state.pick_get_name_color (f_it.handle().idx());
    pickFaceColorBuf_[idx+2]  = _state.pick_get_name_color (f_it.handle().idx());
    pickFaceVertexBuf_[idx]   = mesh_.point(fv_it=mesh_.cfv_iter(f_it));
    pickFaceVertexBuf_[idx+1] = mesh_.point(++fv_it);
    pickFaceVertexBuf_[idx+2] = mesh_.point(++fv_it);
    idx += 3;
  }
  
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingFacesPolymesh(ACG::GLState& _state ) {
  std::cerr << "StripProcessor updatePickingFacesPolymesh polymesh not yet implemented!" << std::endl;
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingAny(ACG::GLState& _state ) {
  if ( mesh_.is_trimesh() )
    updatePickingAnyTrimesh(_state);
  else
    updatePickingAnyPolymesh(_state);
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingAnyTrimesh(ACG::GLState& _state ) {
  std::cerr << "Update any lists" << std::endl;
  updatePickingFaces(_state);
  updatePickingEdges(_state,mesh_.n_faces());
  updatePickingVertices(_state,mesh_.n_faces() + mesh_.n_edges());
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingAnyPolymesh(ACG::GLState& _state ) {
  std::cerr << "StripProcessor updatePickingAnyPolymesh polymesh not yet implemented!" << std::endl;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
