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
// CLASS BSplineSurfaceNodeT
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_BSPLINESURFACENODET_HH
#define ACG_BSPLINESURFACENODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>

#include <QGLWidget>

#include <QImage>


#include <ACG/GL/globjects.hh>
#include <ACG/GL/VertexDeclaration.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================


/** \class BSPlineSurfaceNodeT BSplineSurfaceNodeT.hh <ACG/.../BSplineSurfaceNodeT.hh>
  Brief Description.

  A more elaborate description follows.
*/

template <class BSplineSurfaceType>
class BSplineSurfaceNodeT : public BaseNode
{
  public:

  // typedefs for easy access
  typedef typename BSplineSurfaceType::Point Point;

  /// Constructor
  BSplineSurfaceNodeT(BSplineSurfaceType& _bss,
                      BaseNode*    _parent=0,
                      std::string  _name="<BSplineSurfaceNode>" ) :
    BaseNode(_parent, _name),
    bsplineSurface_(_bss),
    bspline_draw_mode_(NORMAL),
    bspline_selection_draw_mode_(NONE),
    pick_radius_(1.0),
    resolution_(16),
    controlnet_color_( Vec4f(34.f/255.f, 139.f/255.f, 34.f/255.f, 1.f) ),
    controlnet_highlight_color_( Vec4f(1.0f, 1.0f, 1.0f, 1.0f) ),
    surface_color_( Vec4f(178.0f/255.0f, 34.0f/255.0f, 34.0f/255.0f, 1.0f) ),
    surface_highlight_color_( Vec4f(1.0f, 1.0f, 1.0f, 1.0f) ),
    render_control_net_(false),
    render_bspline_surface_(true),
    adaptive_sampling_(false),
    controlPointSelectionTexture_valid_(false),
    knotVectorSelectionTexture_valid_(false),
    pick_texture_idx_(0),
    pick_texture_res_(256),
    pick_texture_baseidx_(0),
    cp_selection_texture_idx_(0),
    cp_selection_texture_res_(256),
    knot_selection_texture_idx_(0),
    knot_selection_texture_res_(256),
    arb_texture_idx_(0),
    arb_texture_used_(false),
    arb_texture_repeat_(false),
    arb_texture_repeat_u_(1.0),
    arb_texture_repeat_v_(1.0),
    surfaceIndexCount_(0),
    invalidateSurfaceMesh_(true),
    controlNetSelIndices_(0),
    controlNetLineIndices_(0),
    invalidateControlNetMesh_(true),
    invalidateControlNetMeshSel_(true)
  {
    cylinder_    = new GLCylinder(16, 1, 1.0f, true, true);
    sphere_      = new GLSphere(5, 5);
    fancySphere_ = new GLSphere(16, 16);
  }

  /// Destructor
  ~BSplineSurfaceNodeT()
  {
    delete cylinder_;
    delete sphere_;
    delete fancySphere_;
  }

  enum BSplineDrawMode {
    NORMAL = 0,
    FANCY = 1
  };

  enum BSplineSelectionDrawMode {
    NONE = 0,
    CONTROLPOINT = 1,
    KNOTVECTOR = 2
  };


  BSplineSurfaceType& bsplineSurface() { return bsplineSurface_; }

  void set_pick_radius( double _pr) { pick_radius_ = _pr; }

  /// static name of this class
  ACG_CLASSNAME(BSplineSurfaceNodeT);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw lines and normals
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// create render objects
  void getRenderObjects(IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat);

  /// picking
  void pick(GLState& _state, PickTarget _target);

  /// update vertex buffer for rendering
  void updateGeometry();

  void set_rendering_resolution(int _res){resolution_ = _res;};

  void render_control_net(bool _render) {render_control_net_ = _render;};

  bool render_control_net() {return render_control_net_; };

  void render_bspline_surface(bool _render) {render_bspline_surface_ = _render;};

  bool render_bspline_surface() { return render_bspline_surface_; };

  void set_bspline_draw_mode(BSplineDrawMode _mode) {bspline_draw_mode_ = _mode;};
  
  void set_selection_draw_mode(BSplineSelectionDrawMode _mode) {bspline_selection_draw_mode_ = _mode;};

  BSplineSelectionDrawMode get_selection_draw_mode() const { return bspline_selection_draw_mode_; }
  
  void adaptive_sampling(bool _adaptive){adaptive_sampling_ = _adaptive;};
  
  void cpSelectionTextureValid  (bool _valid){controlPointSelectionTexture_valid_ = _valid;};
  void knotSelectionTextureValid(bool _valid){knotVectorSelectionTexture_valid_   = _valid;};
  
  
  //! Should be a power of 2
  int& pick_texture_res( ) { return pick_texture_res_; }

  /// use arbitrary texture (in SOLID_TEXTURED mode)
  void set_arb_texture( const QImage& _texture, bool _repeat = false, float _u_repeat = 1.0f, float _v_repeat = 1.0f);
  void set_arb_texture(const GLuint _texture)  { arb_texture_idx_ = _texture; }
  void set_repeat_arb_texture(bool _repeat = true, float _urep = 5.0f, float _vrep = 5.0f) { arb_texture_repeat_ = _repeat; arb_texture_repeat_u_ = _urep; arb_texture_repeat_v_ = _vrep; }
  bool get_repeat_arb_texture( ) { return arb_texture_repeat_; }



private:

  void pick_vertices( GLState& _state );
  void pick_spline( GLState& _state );
  void pick_surface( GLState& _state, unsigned int _offset );

  void draw_cylinder( const Point& _p0, const Point& _axis, double _r, GLState& _state);
  void draw_sphere  ( const Point& _p0, double _r, GLState& _state, GLSphere* _sphere);

  /// Copy constructor (not used)
  BSplineSurfaceNodeT(const BSplineSurfaceNodeT& _rhs);

  /// Assignment operator (not used)
  BSplineSurfaceNodeT& operator=(const BSplineSurfaceNodeT& _rhs);

  void render(GLState& _state, bool _fill);

  void drawSurface(GLState& _state, bool _fill = true);
  
  void drawTexturedSurface(GLState& _state, GLuint _texture_idx);

  void drawControlNet(GLState& _state);
  
  void drawFancyControlNet(GLState& _state);

  void updateControlPointSelectionTexture(GLState& _state);
  
  void updateKnotVectorSelectionTexture(GLState& _state);
  
  /** spline surface u,v-parameter picking */
  /// generate index and setup texture parameters
  void pick_init_texturing ( );
  /// create texture image
  void pick_create_texture( GLState& _state);

  /// draw textured nurbs patch
  void pick_draw_textured_nurbs( GLState& _state);
  
  /// generate index and setup texture parameters for selection visualization
  void selection_init_texturing(GLuint & _texture_idx);
  
  /// creates texture to put onto nurbs curve for visualization of control point selection
  void create_cp_selection_texture(GLState& _state);
  /// creates texture to put onto nurbs curve for visualization of knotvector selection
  void create_knot_selection_texture(GLState& _state);
  
  /// draw textured nurbs patch
  void draw_textured_nurbs( GLState& _state);
  
  ACG::Vec4f generateHighlightColor(ACG::Vec4f _color);
  
  /// update vertex + index buffer of surface mesh
  void updateSurfaceMesh(int _vertexCountU = 50, int _vertexCountV = 50);

  /// update vertex + index buffer of control net mesh
  void updateControlNetMesh();

  /// update index buffer of selected control points
  void updateControlNetMeshSel();

  /// update texture resources for gpu-based spline evaluation
  void updateTexBuffers();

private:

  BSplineSurface& bsplineSurface_;
  
  BSplineDrawMode bspline_draw_mode_;
  
  BSplineSelectionDrawMode bspline_selection_draw_mode_;
  
  double pick_radius_;

  int resolution_;

  Vec4f controlnet_color_;
  Vec4f controlnet_highlight_color_;

  Vec4f surface_color_;
  Vec4f surface_highlight_color_;

  bool render_control_net_;
  bool render_bspline_surface_;

  bool adaptive_sampling_;

  bool controlPointSelectionTexture_valid_;
  bool knotVectorSelectionTexture_valid_;
  
  QImage pick_texture_image_;
  GLuint pick_texture_idx_;
  int    pick_texture_res_;
  // used to only re-create pick_texture_image_ if picking indices changed...
  unsigned int pick_texture_baseidx_;
  
  // texturing stuff for control point selection highlighting
  QImage cp_selection_texture_image_;
  GLuint cp_selection_texture_idx_;
  int    cp_selection_texture_res_;

  // texturing stuff for knot vector selection highlighting
  QImage knot_selection_texture_image_;
  GLuint knot_selection_texture_idx_;
  int    knot_selection_texture_res_;
  
  // texturing stuff for using arbitrary textures
  QImage arb_texture_image_;
  GLuint arb_texture_idx_;
  bool   arb_texture_used_;
  bool   arb_texture_repeat_;
  float  arb_texture_repeat_u_;
  float  arb_texture_repeat_v_;
  
  GLCylinder* cylinder_;
  GLSphere* sphere_;
  GLSphere* fancySphere_;


  // surface mesh
  GeometryBuffer surfaceVBO_;
  IndexBuffer surfaceIBO_;
  VertexDeclaration surfaceDecl_;
  int surfaceIndexCount_;
  bool invalidateSurfaceMesh_;

  // control net mesh
  GeometryBuffer controlNetVBO_;
  IndexBuffer controlNetSelIBO_;
  int controlNetSelIndices_;
  IndexBuffer controlNetLineIBO_;
  int controlNetLineIndices_;
  VertexDeclaration controlNetDecl_;
  bool invalidateControlNetMesh_;
  bool invalidateControlNetMeshSel_;

  // GPU based evaluation
  TextureBuffer knotTexBufferU_;
  TextureBuffer knotTexBufferV_;
  Texture2D controlPointTex_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_BSPLINESURFACENODET_C)
#define ACG_BSPLINESURFACENODET_TEMPLATES
#include "BSplineSurfaceNodeT.cc"
#endif
//=============================================================================
#endif // ACG_BSPLINESURFACENODET_HH defined
//=============================================================================
