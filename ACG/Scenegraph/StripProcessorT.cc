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
mesh_(_mesh),
triangulated_(false) {

}

template <class Mesh>
StripProcessorT<Mesh>::
~StripProcessorT() {

}

/*template <class Mesh>
uint
StripProcessorT<Mesh>::getPolyDim() {
    
    typename Mesh::FaceIter f_it, f_end = mesh_.faces_end();
    typename Mesh::FaceVertexIter fv_it;
    uint n = 0;
    
    for(f_it = mesh_.faces_begin(); f_it != f_end; ++f_it) {
        uint count = 0;
        for ( fv_it = mesh_.fv_iter(f_it); fv_it; ++fv_it ) {
            ++count;
        }
        
        if(count > n) n = count;
    }
    
    return n;
}

template <class Mesh>
uint
StripProcessorT<Mesh>::getFaceDimension(FaceHandle _fh) {
    uint count = 0;
    for ( typename Mesh::FaceVertexIter fv_it = mesh_.fv_iter(_fh); fv_it; ++fv_it )
        ++count;
    return count;
}*/

template <class Mesh>
void
StripProcessorT<Mesh>::convexityTest(FaceHandle /*_fh*/) {
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
  if ( mesh_.is_trimesh() ) {
    buildStripsTriMesh();
  } else {
    buildStripsPolyMesh();
  }
}

template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripsPolyMesh() {
    
    // Note: Limiting number of strips to be built to three, even for n-gons.
    
    std::vector<Strip>                            experiments; // Three strips to be built. Take longest one after all.
    std::vector< typename Mesh::HalfedgeHandle >  h;           // Three halfedges to start from.
    unsigned int                                  best_idx, best_length, length;
    std::vector< FaceHandles >                    faces;       // Lists of faces.
    typename FaceHandles::iterator                fh_it, fh_end;
    typename Mesh::FaceIter                       f_it, f_end=mesh_.faces_end();
    std::vector<FaceMap>                          faceMap;
    
    faceMaps_.clear();
    
    // init faces to be un-processed and un-used
    // deleted or hidden faces are marked processed
    if (mesh_.has_face_status()) {
      for (f_it=mesh_.faces_begin(); f_it!=f_end; ++f_it)
        if (mesh_.status(f_it).hidden() || mesh_.status(f_it).deleted())
          processed(f_it) = used(f_it) = true;
        else
          processed(f_it) = used(f_it) = false;
    } else {
      for (f_it=mesh_.faces_begin(); f_it!=f_end; ++f_it)
        processed(f_it) = used(f_it) = false;
    }

    uint k = 0;
    for (f_it=mesh_.faces_begin(); true; ) {

      // find start face
      for (; f_it != f_end; ++f_it)
          if (!processed(f_it)) break;
      
      // stop if all have been processed
      if (f_it == f_end) 
        break; 
              
      // Number of experiments equals number of edges of the face
      experiments.clear();
      experiments.resize( mesh_.valence(f_it) );
      faceMap.clear();
      faceMap.resize(mesh_.valence(f_it));
      faces.clear();
      faces.resize(mesh_.valence(f_it));
      
      // Store all adjacent halfedges
      h.clear();
      for ( typename Mesh::FaceHalfedgeIter fhalfedge_it(mesh_,f_it); fhalfedge_it; ++fhalfedge_it ) 
        h.push_back( fhalfedge_it.handle() );
      
      // build all strips, take best one
      best_length = best_idx = 0;
      for (unsigned int i = 0; i < mesh_.valence(f_it) ; ++i)
      {
        buildStripPolyMesh(h[i], experiments[i], faces[i], faceMap[i]);
        if ((length = experiments[i].indexArray.size()) > best_length) {
            best_length = length;
            best_idx    = i;
        }
        
        for (fh_it=faces[i].begin(), fh_end=faces[i].end(); fh_it!=fh_end; ++fh_it)
            used(*fh_it) = false;
      }
      
      // update processed status
      // The processed flag indicates that the whole face has been rendered
      fh_it  = faces[best_idx].begin();
      fh_end = faces[best_idx].end();
      for (; fh_it!=fh_end; ++fh_it)
          processed(*fh_it) = true;
      
      // add best strip to strip-list
      strips_.push_back(experiments[best_idx]);
      faceMaps_.push_back(faceMap[best_idx]);
    }
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
      buildStripTriMesh(h[i], experiments[i], faces[i]);
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
buildStripPolyMesh(typename Mesh::HalfedgeHandle _start_hh,
                  Strip& _strip,
                  FaceHandles& _faces,
                  FaceMap&    _faceMap) {
    
    std::list<unsigned int>               strip;
    std::list<typename Mesh::FaceHandle > faceMap;
    typename Mesh::FaceHandle             fh;
    typename Mesh::HalfedgeHandle         hh_left, hh_right; // Keep the hh of the halfedge where we started
                      
    // reset list
    _faces.clear();

    // Init strip
    strip.push_back(mesh_.from_vertex_handle(_start_hh).idx());
    strip.push_back(mesh_.to_vertex_handle(_start_hh).idx());
    
    // Don't update face map here! See below why

    // Walk along the strip: 1st direction
    // We construct the strip by using alternating vertices
    // of each side.
    hh_left = hh_right = _start_hh;
    
    while(true) {
      
      // Go right
      hh_right = mesh_.prev_halfedge_handle(hh_right);
      
      // Add vertex to triangle strip
      strip.push_back(mesh_.from_vertex_handle(hh_right).idx());
      faceMap.push_back(mesh_.face_handle(hh_right));
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
          
          // Mark face as processed and used
          fh = mesh_.face_handle(hh_left);
          _faces.push_back(fh);
          used(fh) = true;
      
          // Go over to next face via the exit halfedge
          hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
          
          if(mesh_.is_boundary(hh_left)) break;
          fh = mesh_.face_handle(hh_left);
          if (processed(fh) || used(fh)) break;
          
          // Test if polygon is convex (only for testing purposes a.t.m.)
          convexityTest(fh);
          
          continue;
      } 
      
      // Go left
      hh_left = mesh_.next_halfedge_handle(hh_left);
      
      // Add vertex to triangle strip
      strip.push_back(mesh_.to_vertex_handle(hh_left).idx());
      faceMap.push_back(mesh_.face_handle(hh_left));
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
        // Mark face as processed and used
        fh = mesh_.face_handle(hh_left);
        _faces.push_back(fh);
        used(fh) = true;
        
        // Go over to next face via the exit halfedge
        hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
        
        if(mesh_.is_boundary(hh_left)) break;
        fh = mesh_.face_handle(hh_left);
        if (processed(fh) || used(fh)) break;
        
        // Test if polygon is convex (only for testing purposes a.t.m.)
        convexityTest(fh);
        
        continue;
      }
       
    }
    
    // Be carefull with the map. As we run in the opposite direction now,
    // we have to finish the map at the end of the strip
    
    // Walk along the strip: 2nd direction
    // We construct the strip by using alternating vertices
    // of each side.
    hh_left = hh_right = mesh_.opposite_halfedge_handle(_start_hh);
    
    while(true) {
      // Boundary check as the first might be at the boundary
      if(mesh_.is_boundary(hh_left)) break;

      // Go right
      hh_right = mesh_.prev_halfedge_handle(hh_right);
      
      // Add vertex to triangle strip
      strip.push_front(mesh_.from_vertex_handle(hh_right).idx());
      faceMap.push_front(mesh_.face_handle(hh_right));
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
          
        // Mark face as processed and used
        fh = mesh_.face_handle(hh_right);
        _faces.push_back(fh);
        used(fh) = true;
    
        // Go over to next face via the exit halfedge
        hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
    
        if(mesh_.is_boundary(hh_left)) break;
        fh = mesh_.face_handle(hh_left);
        if (processed(fh) || used(fh)) break;
        
        // Test if polygon is convex (only for testing purposes a.t.m.)
        convexityTest(fh);
        
        continue;
        
      }
      
      // Go left
      hh_left = mesh_.next_halfedge_handle(hh_left);
      
      // Add vertex to triangle strip
      strip.push_front( mesh_.to_vertex_handle(hh_left).idx() );
      faceMap.push_front( mesh_.face_handle(hh_left) );
      
      // Test if we're at the very last halfedge of the polygon
      if(mesh_.to_vertex_handle(mesh_.next_halfedge_handle(hh_left)) == mesh_.from_vertex_handle(hh_right)) {
        
        // Mark face as processed and used
        fh = mesh_.face_handle(hh_right);
        _faces.push_back(fh);
        used(fh) = true;
        
        // Go over to next face via the exit halfedge
        hh_left = hh_right = mesh_.opposite_halfedge_handle(mesh_.next_halfedge_handle(hh_left));
        
        if(mesh_.is_boundary(hh_left)) break;
        fh = mesh_.face_handle(hh_left);
        if (processed(fh) || used(fh)) break;
        
        // Test if polygon is convex (only for testing purposes a.t.m.)
        convexityTest(fh);
        
        continue;
      }
     
    }
    
    // Finish the map ( As this is the start of the strip now, this data will be ignored!
    faceMap.push_front(mesh_.face_handle(0));
    faceMap.push_front(mesh_.face_handle(0));    
    
    // copy final strip to _strip
    _strip.indexArray.clear();
    _strip.indexArray.reserve(strip.size());
    std::copy(strip.begin(), strip.end(), std::back_inserter(_strip.indexArray));
    
    
    _faceMap.clear();
    _faceMap.reserve(strip.size());
    std::copy(faceMap.begin(), faceMap.end(), std::back_inserter(_faceMap));
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
StripProcessorT<Mesh>::
buildStripTriMesh(typename Mesh::HalfedgeHandle _start_hh,
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
  
  unsigned int idx = 0;
  
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
updatePickingEdges(ACG::GLState& _state,  uint _offset ) {
  pickEdgeColorBuf_.resize(mesh_.n_edges() * 2);
  pickEdgeVertexBuf_.resize(mesh_.n_edges() * 2);
  
  int idx = 0;
  
  typename Mesh::ConstEdgeIter  e_it(mesh_.edges_sbegin()), e_end(mesh_.edges_end());
  for (; e_it!=e_end; ++e_it) {
    
    const Vec4uc pickColor =  _state.pick_get_name_color (e_it.handle().idx() + _offset);
    
    pickEdgeColorBuf_[idx]    = pickColor;
    pickEdgeColorBuf_[idx+1]  = pickColor;
    
    pickEdgeVertexBuf_[idx]   = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 0)));
    pickEdgeVertexBuf_[idx+1] = mesh_.point(mesh_.to_vertex_handle(mesh_.halfedge_handle(e_it, 1)));
    idx += 2;
  }
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

  pickFaceColorBuf_.resize(mesh_.n_faces() * 3);
  pickFaceVertexBuf_.resize(mesh_.n_faces() * 3);
  
  int idx = 0;
  
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()), f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;
  
  for (; f_it!=f_end; ++f_it) {
    const Vec4uc pickColor =  _state.pick_get_name_color (f_it.handle().idx());
    
    pickFaceColorBuf_[idx]    = pickColor;
    pickFaceColorBuf_[idx+1]  = pickColor;
    pickFaceColorBuf_[idx+2]  = pickColor;
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

    // Get total number of triangles
    // Each strip has two vertices more than triangles
    unsigned int n_faces = 0;
    for(StripsIterator it = strips_.begin(); it != strips_.end(); ++it) 
        n_faces += (*it).indexArray.size() - 2;
    
    // 3 vertices per face.
    pickFaceColorBuf_.resize(n_faces * 3);
    
    // Index to the current buffer position
    unsigned int bufferIndex = 0;
    
    // Process all strips
    for ( unsigned int i = 0 ; i < strips_.size() ; ++i ) {
      
        // process all faces in the strip
        // The strip contains 2 faces less then number of vertices in the strip.
        // As we need seperate faces during rendering, the strips are splitted into triangles
        // The last vertex of each triangle defines the picking color for the last face.
        // The handles and indices are collected during the strip generation.
        for (unsigned int stripIndex = 2 ; stripIndex <  strips_[ i ].indexArray.size() ; ++stripIndex) {
          
          // We have to provide a vertex color for each of the vertices as we need flat shading!
          const Vec4uc pickColor = _state.pick_get_name_color ( faceMaps_[i][ stripIndex ].idx() );
          pickFaceColorBuf_[ bufferIndex + 0 ] = pickColor;
          pickFaceColorBuf_[ bufferIndex + 1 ] = pickColor;
          pickFaceColorBuf_[ bufferIndex + 2 ] = pickColor;
          
          bufferIndex += 3;
        }
    }
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePickingAny(ACG::GLState& _state ) {
  // Update the per Face buffer
  updatePerFaceVertexBuffer();
  updatePickingFaces(_state);
  updatePickingEdges(_state,mesh_.n_faces());
  updatePickingVertices(_state,mesh_.n_faces() + mesh_.n_edges());
  
}

template <class Mesh>
void
StripProcessorT<Mesh>::
updatePerFaceVertexBuffer() {
  // Get total number of triangles
  // Each strip has two vertices more than triangles
  unsigned int n_faces = 0;
  for(StripsIterator it = strips_.begin(); it != strips_.end(); ++it) 
    n_faces += (*it).indexArray.size() - 2;
  
  // 3 vertices per face.
  perFaceVertexBuffer_.resize(n_faces * 3);
  
  // Index to the current buffer position
  unsigned int bufferIndex = 0;
  
  // Process all strips
  for ( unsigned int i = 0 ; i < strips_.size() ; ++i ) {
    
    // process all faces in the strip
    // The strip contains 2 faces less then number of vertices in the strip.
    // As we need seperate faces during rendering, the strips are splitted into triangles
    // The last vertex of each triangle defines the picking color for the last face.
    // The handles and indices are collected during the strip generation.
    for (unsigned int stripIndex = 2 ; stripIndex <  strips_[ i ].indexArray.size() ; ++stripIndex) {

      // Cant render triangle strips as we need one color per face and this means duplicating vertices
      perFaceVertexBuffer_[ bufferIndex + 0 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 2 ] ));
      perFaceVertexBuffer_[ bufferIndex + 1 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 1 ] ));
      perFaceVertexBuffer_[ bufferIndex + 2 ] = mesh_.point(mesh_.vertex_handle( strips_[ i ].indexArray[ stripIndex - 0 ] ));
      
      bufferIndex += 3;
    }
  } 
  
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
