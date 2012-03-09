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
    bsplineSurface_(_bss)
  {
    resolution_ = 16;
    render_control_net_     = false;
    render_bspline_surface_ = true;
    adaptive_sampling_      = false;

    controlnet_color_           = Vec4f(34.0/255.0, 139.0/255.0, 34.0/255.0, 1.0);
    controlnet_highlight_color_ = Vec4f(1.0, 1.0, 1.0, 1.0);
    surface_color_             = Vec4f(178.0/255.0, 34.0/255.0, 34.0/255.0, 1.0);
    surface_highlight_color_   = Vec4f(1.0, 1.0, 1.0, 1.0);
    
    bspline_draw_mode_           = NORMAL;
    bspline_selection_draw_mode_ = NONE;
    
    cp_selection_texture_res_   = 256;
    knot_selection_texture_res_ = 256;
    
    cp_selection_texture_idx_ = 0;
    knot_selection_texture_idx_ = 0;
    pick_texture_idx_ = 0;  
    arb_texture_idx_ = 0;

    cylinder_ = new GLCylinder(16, 1, 1.0f, true, true);
    sphere_ = new GLSphere(5, 5);
    fancySphere_ = new GLSphere(16, 16);
  }

  /// Destructor
  ~BSplineSurfaceNodeT()
  {
    if (cylinder_)
      delete cylinder_;

    if (sphere_)
      delete sphere_;

    if (fancySphere_)
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

  /// picking
  void pick(GLState& _state, PickTarget _target);

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
  void set_repeat_arb_texture( bool _repeat = true, float _urep = 5.0f, float _vrep = 5.0f) { arb_texture_repeat_ = _repeat; arb_texture_repeat_u_ = _urep; arb_texture_repeat_v_ = _vrep; }
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

  void drawSurface(GLState& _state, bool _fill);
  
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

