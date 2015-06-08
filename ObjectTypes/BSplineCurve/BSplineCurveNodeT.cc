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


//=============================================================================
//
//  CLASS BSplineCurveNodeT - IMPLEMENTATION
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#define ACG_BSPLINECURVENODET_C

//== INCLUDES =================================================================

#include "BSplineCurveNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/GL/IRenderer.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>
#include <OpenMesh/Core/Utils/vector_cast.hh>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
  {
    _bbMin.minimize(bsplineCurve_.get_control_point(i));
    _bbMax.maximize(bsplineCurve_.get_control_point(i));
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
DrawModes::DrawMode
BSplineCurveNodeT<BSplineCurve>::
availableDrawModes() const
{
  /*
  DrawModes::DrawMode drawModes(0);

  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::WIREFRAME;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
  drawModes |= DrawModes::SOLID_FLAT_SHADED;
  drawModes |= DrawModes::SOLID_PHONG_SHADED;
  drawModes |= DrawModes::SOLID_SHADER;
  drawModes |= DrawModes::SOLID_TEXTURED;
  drawModes |= DrawModes::SOLID_1DTEXTURED;

  return drawModes;*/

  return (DrawModes::WIREFRAME | DrawModes::POINTS);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{  
  glPushAttrib(GL_ENABLE_BIT);
  
  // check if textures are still valid
  if (    bspline_selection_draw_mode_ == CONTROLPOINT
       && controlPointSelectionTexture_valid_ == false)
    updateControlPointSelectionTexture(_state);
  
  if (    bspline_selection_draw_mode_ == KNOTVECTOR
       && knotVectorSelectionTexture_valid_ == false)
    updateKnotVectorSelectionTexture(_state);
  
  
  if (_drawMode & DrawModes::WIREFRAME)
  {
    ACG::GLState::disable( GL_CULL_FACE );
    
    if (bspline_draw_mode_ == NORMAL)
    {
      ACG::GLState::disable(GL_LIGHTING);
    }
    else if (bspline_draw_mode_ == FANCY)
    {
//       ACG::GLState::enable(GL_AUTO_NORMAL);
//       ACG::GLState::enable(GL_NORMALIZE);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      ACG::GLState::enable( GL_COLOR_MATERIAL );
      ACG::GLState::enable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_SMOOTH);
    }

    render( _state, false, _drawMode);
  }
  else if (_drawMode & DrawModes::POINTS)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    
    render( _state, false, _drawMode);
  }

  
  glPopAttrib();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void 
BSplineCurveNodeT<BSplineCurve>::
getRenderObjects( IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat )
{
  // check if textures are still valid
  if (    bspline_selection_draw_mode_ == CONTROLPOINT
    && controlPointSelectionTexture_valid_ == false)
    updateControlPointSelectionTexture(_state);

  if (    bspline_selection_draw_mode_ == KNOTVECTOR
    && knotVectorSelectionTexture_valid_ == false)
    updateKnotVectorSelectionTexture(_state);

  // update vbo
  updateCurveBuffer();

  // init base object
  RenderObject ro;
  ro.initFromState(&_state);
  ro.depthTest = true;

  ro.vertexBuffer = curveLineVBO_.id();
  ro.vertexDecl = &curveLineDecl_;


  // create object for each layer
  for (size_t i = 0; i < _drawMode.getNumLayers(); ++i)
  {
    const DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    ro.setupShaderGenFromDrawmode(props);

    if (props->primitive() == DrawModes::PRIMITIVE_POINT)
    {
      ro.glDrawArrays(GL_POINTS, 0, curveLineVertices_);
      _renderer->addRenderObject(&ro);
    }
    else if (props->primitive() == DrawModes::PRIMITIVE_WIREFRAME)
    {
      ro.glDrawArrays(GL_LINE_STRIP, 0, curveLineVertices_);
      _renderer->addRenderObject(&ro);
    }
  }

  // create objects for the control polygon (includes selection on the polygon)
  if (render_control_polygon_)
  {
    updateControlPointBuffer();
    updateControlPointSelBuffer();
    updateControlEdgeSelBuffer();

    ro.vertexBuffer = controlPointVBO_.id();
    ro.vertexDecl = &controlPointDecl_;

    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
    ro.shaderDesc.clearTextures();
    ro.shaderDesc.vertexColors = false;


    Vec3f highlightColor = OpenMesh::vector_cast<Vec3f, Vec4f>(generateHighlightColor(polygon_color_));
    Vec3f polygonColor = OpenMesh::vector_cast<Vec3f, Vec4f>(polygon_color_);
    

    Vec2f screenSize = Vec2f(_state.viewport_width(), _state.viewport_height());

    // edge-selection
    if (controlEdgeSelCount_)
    {
      ro.emissive = highlightColor;
      ro.setupLineRendering(2.0f * _state.line_width(), screenSize);

      ro.indexBuffer = controlEdgeSelIBO_.id();
      ro.glDrawElements(GL_LINES, 2 * controlEdgeSelCount_, GL_UNSIGNED_INT, 0);
      _renderer->addRenderObject(&ro);
    }

    // all line segments
    ro.emissive = polygonColor;
    ro.setupLineRendering(_state.line_width(), screenSize);
    ro.glDrawArrays(GL_LINE_STRIP, 0, bsplineCurve_.n_control_points());
    _renderer->addRenderObject(&ro);

    ro.resetLineRendering();

    // point selection
    if (controlPointSelCount_)
    {
      ro.emissive = highlightColor;
      ro.setupPointRendering(10.0f, screenSize);

      ro.indexBuffer = controlPointSelIBO_.id();
      ro.glDrawElements(GL_POINTS, controlPointSelCount_, GL_UNSIGNED_INT, 0);
      _renderer->addRenderObject(&ro);
    }

    // all points
    ro.emissive = polygonColor;
    ro.setupPointRendering(_state.point_size() + 4.0f, screenSize);
    ro.glDrawElements(GL_POINTS, bsplineCurve_.n_control_points(), GL_UNSIGNED_INT, 0);
    _renderer->addRenderObject(&ro);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
render(GLState& _state, bool /*_fill*/, DrawModes::DrawMode _drawMode)
{
  // draw the control polygon (includes selection on the polygon)
  if (render_control_polygon_)
  {
    if (bspline_draw_mode_ == NORMAL)
      drawControlPolygon(_drawMode, _state);
    else if (bspline_draw_mode_ == FANCY)
      drawFancyControlPolygon(_drawMode, _state);
  }

  
  // draw the spline curve itself, depending on the type of visualization
  if ((_drawMode & DrawModes::WIREFRAME) && render_bspline_curve_)
  {
    if (bspline_selection_draw_mode_ == NONE)
    {
      if (bspline_draw_mode_ == NORMAL)
        drawCurve(_state);
      else
        drawFancyCurve(_state);
    }
    else
    {
      if (bspline_selection_draw_mode_ == CONTROLPOINT) {
        drawTexturedCurve(_state, cp_selection_texture_idx_);
      }
      else if (bspline_selection_draw_mode_ == KNOTVECTOR) {
        drawTexturedCurve(_state, knot_selection_texture_idx_);
      }
    }
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawCurve(GLState& /*_state*/)
{
  updateCurveBuffer();

  curveLineVBO_.bind();
  curveLineDecl_.activateFixedFunction();

  glDrawArrays(GL_LINE_STRIP, 0, curveLineVertices_);

  curveLineDecl_.deactivateFixedFunction();
  curveLineVBO_.unbind();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawFancyCurve(GLState& _state)
{
  // draw the curve
//  double cylinderRadius = _state.line_width() * 0.05;
  double cylinderRadius = _state.line_width() * 0.2;

  for (int i = 0; i < (int)curve_samples_.size() - 1; ++i)
  {
    Vec3d p      = curve_samples_[i].first;
    Vec3d p_next = curve_samples_[i+1].first;
    draw_cylinder(p, p_next - p, cylinderRadius, _state);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
ACG::Vec4f
BSplineCurveNodeT<BSplineCurve>::
generateHighlightColor(ACG::Vec4f _color)
{
  float c1 = _color[0]*1.5; 
  c1 = c1 > 1.0 ? 1.0 : c1;
  
  float c2 = _color[1]*1.5; 
  c2 = c2 > 1.0 ? 1.0 : c2;
  
  float c3 = _color[2]*1.5; 
  c3 = c3 > 1.0 ? 1.0 : c3;
  
  return Vec4f( c1, c2, c3, _color[3]);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawControlPolygon(DrawModes::DrawMode _drawMode, GLState& _state)
{
  updateControlPointBuffer();
  updateControlPointSelBuffer();
  updateControlEdgeSelBuffer();

  // remember current base color
  Vec4f base_color_old = _state.base_color();
  
  controlPointVBO_.bind();
  controlPointDecl_.activateFixedFunction();

  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME)
  {
    // draw selection
    if( bsplineCurve_.edge_selections_available())
    {
      // save old values
      float line_width_old = _state.line_width();

      glColor(generateHighlightColor(polygon_color_));
      glLineWidth(2*line_width_old);

      controlEdgeSelIBO_.bind();
      glDrawElements(GL_LINES, 2 * controlEdgeSelCount_, GL_UNSIGNED_INT, 0);
      controlEdgeSelIBO_.unbind();

      glLineWidth(line_width_old);
    }

    // draw all line segments
    glColor(polygon_color_);

//     float line_width_old = _state.line_width();
//     glLineWidth(line_width_old+2.0);


    // draw bspline control polygon
    glDrawArrays(GL_LINE_STRIP, 0, bsplineCurve_.n_control_points());
    
//     glLineWidth(line_width_old);
  }
  
  
  // draw points
  if ((_drawMode & DrawModes::POINTS) && render_control_polygon_)
  {
    // draw selection
    if (controlPointSelCount_)
    {
      // save old values
      float point_size_old = _state.point_size();

      glColor(generateHighlightColor(polygon_color_));
      glPointSize(10);

      controlPointSelIBO_.bind();
      glDrawElements(GL_POINTS, controlPointSelCount_, GL_UNSIGNED_INT, 0);
      controlPointSelIBO_.unbind();

      glPointSize(point_size_old);
    }

    // draw all points
    glColor(polygon_color_);
    float point_size_old = _state.point_size();
    glPointSize(point_size_old + 4);

    glDrawArrays(GL_POINTS, 0, bsplineCurve_.n_control_points());

    glPointSize(point_size_old);
  }
  

  controlPointDecl_.deactivateFixedFunction();
  controlPointVBO_.unbind();

  // reset olf color
  glColor( base_color_old );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawFancyControlPolygon(DrawModes::DrawMode _drawMode, GLState& _state)
{
  // save old base color
  Vec4f base_color_old = _state.base_color();
  
  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME)
  {
//    double cylinderRadius = _state.line_width() * 0.05;
    double cylinderRadius = _state.line_width() * 0.2;

    // draw selection
    if( bsplineCurve_.edge_selections_available())
    {
      glColor(generateHighlightColor(polygon_color_));

      // draw bspline control polygon
      for (int i = 0; i < (int)bsplineCurve_.n_control_points()-1; ++i) // #edges
      {
        if (bsplineCurve_.edge_selection(i))
        {
          Point p    = bsplineCurve_.get_control_point(i);
          Point axis = bsplineCurve_.get_control_point(i+1) - bsplineCurve_.get_control_point(i);
          draw_cylinder(p, axis, cylinderRadius, _state);
        }
      }
    }

    // draw all line segments
    glColor(polygon_color_);

    // draw bspline control polygon
    for (unsigned int i = 0; i < bsplineCurve_.n_control_points() - 1; ++i)
    {
      Point p    = bsplineCurve_.get_control_point(i);
      Point axis = bsplineCurve_.get_control_point(i+1) - bsplineCurve_.get_control_point(i);
      draw_cylinder(p, axis, cylinderRadius, _state);
    }
  } // end of if wireframe
  
  
  // draw points
  if ((_drawMode & DrawModes::POINTS) && render_control_polygon_)
  {
    if (bsplineCurve_.n_control_points() == 0)
      return;
    
    // radius of sphere
//    double sphereRadius = _state.point_size() * 0.05;
    double sphereRadius = _state.point_size() * 0.25;

    // draw selection
    if( bsplineCurve_.controlpoint_selections_available())
    {
      glColor(generateHighlightColor(polygon_color_));

      // draw control polygon
      for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
        if (bsplineCurve_.controlpoint_selection(i))
          draw_sphere(bsplineCurve_.get_control_point(i), sphereRadius, _state, fancySphere_);
    }

    // draw all points
    glColor(polygon_color_);
    
    for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
      draw_sphere(bsplineCurve_.get_control_point(i), sphereRadius, _state, fancySphere_);
  }
  
   // reset color
   glColor( base_color_old );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawTexturedCurve(GLState& _state, GLuint _texture_idx)
{   
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  ACG::GLState::enable( GL_COLOR_MATERIAL );
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  
  ACG::GLState::enable(GL_TEXTURE_2D);
  
  ACG::GLState::bindTexture( GL_TEXTURE_2D, _texture_idx);
  
  // blend colors (otherwise lighting does not affect the texture)
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_MODULATE to include lighting effects
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  float line_width_old = _state.line_width();
  draw_textured_nurbs( _state);
  glLineWidth(line_width_old);

  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
  ACG::GLState::disable(GL_TEXTURE_2D);
  ACG::GLState::disable( GL_COLOR_MATERIAL );
  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateGeometry()
{
  invalidateCurveLine_ = true;
  invalidateControlPointVBO_ = true;

  curve_samples_.clear();

  std::pair< Vec3d, Vec4f > sample;

  int d = bsplineCurve_.degree();
  int k = bsplineCurve_.n_knots();

  for ( int l = d; l < k - d - 1; ++l )
  {
    for ( int s = 0; s <= resolution_; ++s )
    {
      double step = s / (float) resolution_ ;
      double u = bsplineCurve_.get_knot( l ) + step  * ( bsplineCurve_.get_knot( l+1 ) - bsplineCurve_.get_knot( l ) );

        // check if highlighted
      if ( bsplineCurve_.get_knotvector_ref()->selections_available() )
      {
        if (    bsplineCurve_.get_knotvector_ref()->selection(l)
             && bsplineCurve_.get_knotvector_ref()->selection(l+1))
          sample.second = curve_highlight_color_;
        else
          sample.second = curve_color_;
      }
      else
        sample.second = curve_color_;

      sample.first = bsplineCurve_.curvePoint(u);

      curve_samples_.push_back(sample);
    } // end of resolution iter
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick(GLState& _state, PickTarget _target)
{
  switch (_target)
  {
    case PICK_VERTEX:
    {
      _state.pick_set_maximum (bsplineCurve_.n_control_points());
      pick_vertices(_state);
      break;
    }

    case PICK_SPLINE:
    {
      _state.pick_set_maximum (pick_texture_res_ );
      pick_spline(_state, 0);
      break;
    }


    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (bsplineCurve_.n_control_points() + pick_texture_res_);
      pick_vertices(_state);
      pick_spline(_state, bsplineCurve_.n_control_points());
      break;
    }

    default:
      _state.pick_set_maximum(1);
      _state.pick_set_name(0);
      break;
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_vertices( GLState& _state )
{
  // radius in pixels
  int psize = 7;

//   _state.pick_set_name (0);

  for (unsigned int i = 0; i < bsplineCurve_.n_control_points(); ++i)
  {
    _state.pick_set_name (i);

    // compute 3d radius of sphere
    Vec3d window_pos = _state.project( (Vec3d) bsplineCurve_.get_control_point(i));
    int px = round( window_pos[0]);
    int py = round( window_pos[1]);
    double angle = acos(_state.viewing_direction(px, py).normalize()|_state.viewing_direction(px+psize, py).normalize());
    double l = (_state.eye() - (Vec3d)bsplineCurve_.get_control_point(i)).norm();
    double r = l*tan(angle);

    // draw 3d sphere
    draw_sphere(bsplineCurve_.get_control_point(i), r, _state, sphere_);
  }
}

//----------------------------------------------------------------------------

template <class BSplineCurve >
void
BSplineCurveNodeT<BSplineCurve>::
pick_spline( GLState& _state, unsigned int _offset )
{
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  ACG::GLState::enable(GL_TEXTURE_2D);
//   ACG::GLState::enable(GL_TEXTURE_1D);
//   ACG::GLState::enable(GL_MAP1_TEXTURE_COORD_1);


  if( _state.pick_current_index () + _offset != pick_texture_baseidx_)
  {
    pick_texture_baseidx_ = _state.pick_current_index() + _offset;
    pick_create_texture( _state);
  }
  else
  {
    // do not blend colors (else color picking breaks!)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    // avoid aliasing at patch boundaries
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    // GL_REPLACE to avoid smearing colors (else color picking breaks!)
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
    ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_);
//     ACG::GLState::bindTexture( GL_TEXTURE_1D, pick_texture_idx_);
  }

  float line_width_old = _state.line_width();
  glLineWidth(10);
  draw_textured_nurbs( _state);
  glLineWidth(line_width_old);

//   ACG::GLState::bindTexture( GL_TEXTURE_1D, 0);
//   ACG::GLState::disable(GL_TEXTURE_1D);
//   ACG::GLState::disable(GL_MAP1_TEXTURE_COORD_1);

  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
  ACG::GLState::disable(GL_TEXTURE_2D);

  glPopAttrib( );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw_sphere( const Point& _p0, double _r, GLState& _state, GLSphere* _sphere)
{
  // draw 3d sphere
  _state.push_modelview_matrix();
  _state.translate( _p0[0], _p0[1], _p0[2]);

  _sphere->draw(_state,_r);

  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw_cylinder( const Point& _p0, const Point& _axis, double _r, GLState& _state)
{
  _state.push_modelview_matrix();
  _state.translate(_p0[0], _p0[1], _p0[2]);

  Point direction = _axis;
  Point z_axis(0,0,1);
  Point rot_normal;
  double rot_angle;

  direction.normalize();
  rot_angle  = acos((z_axis | direction))*180/M_PI;
  rot_normal = ((z_axis % direction).normalize());


  if( fabs( rot_angle ) > 0.0001 && fabs( 180 - rot_angle ) > 0.0001)
    _state.rotate(rot_angle,rot_normal[0], rot_normal[1], rot_normal[2]);
  else
    _state.rotate(rot_angle,1,0,0);

  cylinder_->setBottomRadius(_r);
  cylinder_->setTopRadius(_r);
  cylinder_->draw(_state,_axis.norm());

  _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateControlPointSelectionTexture(GLState& _state)
{
  create_cp_selection_texture(_state);
  controlPointSelectionTexture_valid_ = true;

  // vbo containing the control points needs updating
  invalidateControlPointSelIBO_ = true;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateKnotVectorSelectionTexture(GLState& _state)
{
  create_knot_selection_texture(_state);
  knotVectorSelectionTexture_valid_ = true;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
selection_init_texturing(GLuint & _texture_idx)
{
  // generate texture index
  glGenTextures( 1, &_texture_idx );
  // bind texture as current
  ACG::GLState::bindTexture( GL_TEXTURE_2D, _texture_idx );
  // blend colors (otherwise lighting does not affect the texture)
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_MODULATE to include lighting effects
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
create_cp_selection_texture(GLState& /*_state*/)
{
  if (bsplineCurve_.n_knots() == 0)
    return;
  
  if(cp_selection_texture_idx_ == 0)
      selection_init_texturing(cp_selection_texture_idx_);
  
  QImage b(cp_selection_texture_res_, 2, QImage::Format_ARGB32);
  
  int degree   = bsplineCurve_.degree();
  int numKnots = bsplineCurve_.n_knots();
  
  double minu  = bsplineCurve_.get_knot( degree );
  double maxu  = bsplineCurve_.get_knot( numKnots - degree -1 );
  double diffu = maxu - minu;
  if (diffu == 0.0) return;

  // get the colors to create the texture
//   Vec4f curveColor     = _state.base_color();
//   Vec4f highlightColor = generateHighlightColor(curveColor);
  Vec4f curveColor     = curve_color_;
  Vec4f highlightColor = curve_highlight_color_;
  

  int texelIdx = 0;
  for ( int m = 0; m < cp_selection_texture_res_; ++m)
  {
    double step_m = (double)m / (double)cp_selection_texture_res_;
    double u = step_m * diffu;
  
    // get the span and check which knots are selected
    ACG::Vec2i span = bsplineCurve_.span(u);
    // check for incomple spline 
    if (span[0] < 0 || span[1] < 0) 
      return; 

    float alpha = 0.0; // blends between curve and highlight colors
    for (int i = 0; i < degree+1; ++i) // degree+1 basis functions (those in the span) contribute
    {
      int idx = span[0] + i;
    
      // basis functions sum up to 1. hence, we only have to sum up those with selected control point to get the blending weight
      if (bsplineCurve_.controlpoint_selected(idx))
        alpha += bsplineCurve_.basisFunction(idx, degree, u);
    }
  
    // compute color
    Vec4f color =  curveColor * (1.0 - alpha) + highlightColor * alpha;

    // fill texture
    b.setPixel (texelIdx, 0, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    b.setPixel (texelIdx, 1, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    
    ++texelIdx;
  }
  
  // debug, output image
  //b.save("curveCPSelectionTexture.png", "PNG");
  
  cp_selection_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, cp_selection_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, cp_selection_texture_image_.width(), cp_selection_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, cp_selection_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
create_knot_selection_texture(GLState& /*_state*/)
{
  if (bsplineCurve_.n_knots() == 0)
    return;
  
  if(knot_selection_texture_idx_ == 0)
      selection_init_texturing(knot_selection_texture_idx_);
  
  QImage b(knot_selection_texture_res_, 2, QImage::Format_ARGB32);

  int degree   = bsplineCurve_.degree();
  int numKnots = bsplineCurve_.n_knots();
  
  double minu  = bsplineCurve_.get_knot( degree );
  double maxu  = bsplineCurve_.get_knot( numKnots - degree -1 );
  double diffu = maxu - minu;
  if (diffu == 0.0) return;

  int texelIdx = 0;
  
  // if a knot is selected, select all knots in the span of this knot, too
  std::vector<bool> selectedKnotSpans(numKnots, false);  
  for (int i = 0; i < numKnots; ++i)
  {
    if (bsplineCurve_.get_knotvector_ref()->selection(i))
    {
      // get the span and check which knots are selected
      ACG::Vec2i span = bsplineCurve_.span(bsplineCurve_.get_knot(i));
      // check for incomple spline
      if (span[0] < 0 || span[1] < 0)
        return;

      for(int j = span[0]; j <= span[1]+degree; ++j)
        selectedKnotSpans[j] = true;
    }
  }

//   Vec4f curveColor     = _state.base_color();
//   Vec4f highlightColor = generateHighlightColor(curveColor);
  Vec4f curveColor     = curve_color_;
  Vec4f highlightColor = curve_highlight_color_;

  for ( int m = 0; m < knot_selection_texture_res_; ++m)
  {
    double step_m = (double)m / (double)knot_selection_texture_res_;
    double u = step_m * diffu;
  
    Vec4f color;
    Vec2i interval = bsplineCurve_.interval(u);
    // check if highlighted
    if (selectedKnotSpans[interval[0]] && selectedKnotSpans[interval[1]])
      color = highlightColor;
    else
      color = curveColor;

    // fill texture
    b.setPixel (texelIdx, 0, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    b.setPixel (texelIdx, 1, qRgba((int)(color[0]*255.0), (int)(color[1]*255.0), (int)(color[2]*255.0), 255));
    
    ++texelIdx;
  }
  
  // debug, output image
  //b.save("curveKnotSelectionTexture.png", "PNG");
  
  knot_selection_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, knot_selection_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, knot_selection_texture_image_.width(), knot_selection_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, knot_selection_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_init_texturing( )
{
  pick_texture_res_     = 256;
  pick_texture_baseidx_ = 0;

  // generate texture index
  glGenTextures( 1, &pick_texture_idx_ );
  // bind texture as current
  ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  // do not blend colors (else color picking breaks!)
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  // GL_REPLACE to avoid smearing colors (else color picking breaks!)
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_2D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_create_texture( GLState& _state)
{
  if(pick_texture_idx_ == 0)
    pick_init_texturing();
    
  QImage b(pick_texture_res_, 2, QImage::Format_ARGB32);
    
  // fill with colors
  int cur_idx=0;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    Vec4uc cur_col( _state.pick_get_name_color (cur_idx) );
    b.setPixel (i, 0, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
    b.setPixel (i, 1, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
    cur_idx++;
  }

/*
  // create stripe or checkerboard texture
  bool odd_row = true;
  bool odd_col = true;
  bool green = true;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    if (i % 20 == 0)
      odd_row = !odd_row;
    
    odd_col = true;
    for( int j = 0; j < pick_texture_res_; ++j)
    {
      if (j % 20 == 0)
        odd_col = !odd_col;
      
//       green = (odd_row && odd_col) || (!odd_row && !odd_col);      // checkerboard texture
      green = odd_row; // stripe texture
      if (green)
        b.setPixel (i, j, qRgba(0, 255, 0, 255));
      else
        b.setPixel (i, j, qRgba(255, 0, 255, 255));
    }
  }
*/

  // debug, output image
//   b.save("curveTexture.png", "PNG");
  
  pick_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_2D, pick_texture_idx_ );
  glTexImage2D(  GL_TEXTURE_2D,
                 0, GL_RGBA, pick_texture_image_.width(), pick_texture_image_.height(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, pick_texture_image_.bits() );
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
draw_textured_nurbs( GLState& /*_state*/)
{
  updateCurveBuffer();

  curveLineVBO_.bind();
  curveLineDecl_.activateFixedFunction();

  glDrawArrays(GL_LINE_STRIP, 0, curveLineVertices_);

  curveLineDecl_.deactivateFixedFunction();
  curveLineVBO_.unbind();
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateCurveBuffer(int _numVertices)
{
  if (!invalidateCurveLine_)
    return;

  // create vertex declaration if uninitialized
  if (!curveLineDecl_.getNumElements())
  {
    curveLineDecl_.addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);
    curveLineDecl_.addElement(GL_FLOAT, 1, VERTEX_USAGE_TEXCOORD);
  }

  // vbo memory:
  //  float3 pos
  //  float  texcoord
  std::vector<float> vboData(_numVertices * 4);

  for (int i = 0; i < _numVertices; ++i)
  {
    // param in [0, 1]
    typename BSplineCurve::Scalar u01 = typename BSplineCurve::Scalar(i) / typename BSplineCurve::Scalar(_numVertices - 1);

    // map to actual range
    typename BSplineCurve::Scalar u = (1 - u01) * bsplineCurve_.lower() + u01 * bsplineCurve_.upper();

    // evaluate curve point
    typename BSplineCurve::Point pos = bsplineCurve_.curvePoint(u);

    // store pos
    for (int k = 0; k < 3; ++k)
      vboData[i*4 + k] = pos[k];

    // store texcoord
    vboData[i*4 + 3] = u01;
  }


  curveLineVBO_.del();
  if (_numVertices)
    curveLineVBO_.upload(vboData.size() * 4, &vboData[0], GL_STATIC_DRAW);


  curveLineVertices_ = _numVertices;

  invalidateCurveLine_ = false;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateControlPointBuffer()
{
  if (!invalidateControlPointVBO_)
    return;

  // create vertex declaration if uninitialized
  if (!controlPointDecl_.getNumElements())
    controlPointDecl_.addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);

  int numCP = bsplineCurve_.n_control_points();

  // vbo memory:
  //  float3 pos
  std::vector<float> vboData(numCP * 3);

  for (int i = 0; i < numCP; ++i)
  {
    typename BSplineCurve::Point pos = bsplineCurve_.get_control_point(i);
    for (int k = 0; k < 3; ++k)
      vboData[i*3 + k] = pos[k];
  }

  controlPointVBO_.del();
  if (numCP)
    controlPointVBO_.upload(vboData.size() * 4, &vboData[0], GL_STATIC_DRAW);

  invalidateControlPointVBO_ = false;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateControlPointSelBuffer()
{
  if (!invalidateControlPointSelIBO_)
    return;

  controlPointSelIBO_.del();

  if (bsplineCurve_.controlpoint_selections_available())
  {
    int numCP = bsplineCurve_.n_control_points();

    // count # selected points
    int numSel = 0;
    for (int i = 0; i < numCP; ++i)
    {
      if (bsplineCurve_.controlpoint_selection(i))
        ++numSel;
    }

    // save count for draw call
    controlPointSelCount_ = numSel;


    if (numSel)
    {
      // create array
      std::vector<int> iboData(numSel);
      numSel = 0;
      for (int i = 0; i < numCP; ++i)
      {
        if (bsplineCurve_.controlpoint_selection(i))
          iboData[numSel++] = i;
      }

      controlPointSelIBO_.upload(numSel * 4, &iboData[0], GL_STATIC_DRAW);
    }
  }

  invalidateControlPointSelIBO_ = false;
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
updateControlEdgeSelBuffer()
{
  if (!invalidateControlEdgeSelIBO_)
    return;

  controlEdgeSelIBO_.del();

  if (bsplineCurve_.edge_selections_available())
  {
    int numCP = bsplineCurve_.n_control_points();
    int numE = numCP - 1;

    // count # selected edges
    int numSel = 0;
    for (int i = 0; i < numE; ++i)
    {
      if (bsplineCurve_.edge_selection(i))
        ++numSel;
    }

    // save count for draw call
    controlEdgeSelCount_ = numSel;

    if (numSel)
    {
      // create array
      std::vector<int> iboData(numSel * 2);
      numSel = 0;
      for (int i = 0; i < numE; ++i)
      {
        if (bsplineCurve_.edge_selection(i))
        {
          iboData[numSel++] = i;
          iboData[numSel++] = (i+1)%numCP;
        }
      }

      controlEdgeSelIBO_.upload(numSel * 4, &iboData[0], GL_STATIC_DRAW);
    }
  }

  invalidateControlEdgeSelIBO_ = false;
}

//----------------------------------------------------------------------------

/*
template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_init_texturing( )
{
  pick_texture_res_     = 256;
  pick_texture_baseidx_ = 0;

  // generate texture index
  glGenTextures( 1, &pick_texture_idx_ );
  // bind texture as current
  ACG::GLState::bindTexture( GL_TEXTURE_1D, pick_texture_idx_ );
  // do not blend colors (else color picking breaks!)
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  // avoid aliasing at patch boundaries
  glTexParameterf( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  // GL_REPLACE to avoid smearing colors (else color picking breaks!)
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  // unbind current texture
  ACG::GLState::bindTexture( GL_TEXTURE_1D, 0);
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_create_texture( GLState& _state)
{
  std::cout << "[BSplineCurveNodeT] pick_create_texture" << std::endl;
  
//   QImage b(pick_texture_res_, pick_texture_res_, QImage::Format_ARGB32);
  QImage b(pick_texture_res_, 1, QImage::Format_ARGB32);
  
  std::cout << "texture of size " << b.width() << " x " << b.height() << std::endl;
    
  // fill with colors
  int cur_idx = 0;
  bool green = false;
  for( int i = 0; i < pick_texture_res_; ++i)
  {
    Vec4uc cur_col( _state.pick_get_name_color (cur_idx) );
//     b.setPixel (i, 0, qRgba((int)cur_col[0], (int)cur_col[1], (int)cur_col[2], (int)cur_col[3]));
    
    if (i % 10 == 0)
      green = !green;
    
    if (green)
      b.setPixel (i, 0, qRgba(0, 255, 0, 255));
    else
      b.setPixel (i, 0, qRgba(255, 0, 255, 255));
    
    cur_idx++;
  }

  // debug, output image (usually does not look as expected :\ )
  b.save("1Dcurvetexture.png", "PNG");
  
  pick_texture_image_ = QGLWidget::convertToGLFormat( b );

  // bind texture 
  ACG::GLState::bindTexture( GL_TEXTURE_1D, pick_texture_idx_ );
  glTexImage1D(  GL_TEXTURE_1D,
                 0, GL_RGBA, pick_texture_image_.width(), 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 
                 pick_texture_image_.bits() );
                 
}

//----------------------------------------------------------------------------

template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
pick_draw_textured_nurbs( GLState& _state)
{
  std::cout << "[BSplineCurveNodeT] pick_draw_textured_nurbs" << std::endl;
  
  int numKnots     = bsplineCurve_.n_knots();
  const int numCPs = bsplineCurve_.n_control_points();
  int order        = bsplineCurve_.degree() + 1;

  // get kntvector
  std::cout << "knots: " << std::flush;
  GLfloat *knots = new GLfloat[numKnots];
  for (int i = 0; i < numKnots; ++i)
  {
    knots[i] = bsplineCurve_.get_knot(i);
    std::cout << bsplineCurve_.get_knot(i) << ", " << std::flush;
  }
  std::cout << std::endl;
 
  // get control points
  GLfloat *ctlpoints = new GLfloat[numCPs * 3];
  for (int i = 0; i < numCPs; ++i)
  {
    Vec3d p = bsplineCurve_.get_control_point(i);
    ctlpoints[i * 3 + 0] = (GLfloat)p[0];
    ctlpoints[i * 3 + 1] = (GLfloat)p[1];
    ctlpoints[i * 3 + 2] = (GLfloat)p[2];
  }


  glLineWidth(5);

  GLUnurbsObj *theNurb;
  theNurb = gluNewNurbsRenderer();

  #ifdef WIN32
    gluNurbsCallback(theNurb, GLU_ERROR, (void (__stdcall *)(void))(&nurbsErrorCallback) );
  #else
    gluNurbsCallback(theNurb, GLU_ERROR, (GLvoid (*)()) (&nurbsErrorCallback) );  
  #endif

  // draw filled
  gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

   #ifdef GLU_OBJECT_PARAMETRIC_ERROR
    // object space -> fixed (non-adaptive) sampling
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_OBJECT_PARAMETRIC_ERROR);
  #else
    gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD,   GLU_PARAMETRIC_ERROR);
  #endif

  gluNurbsProperty(theNurb, GLU_PARAMETRIC_TOLERANCE, 0.2);

  // get min/max knots of domain defining patch (partition of unity)
  float  minu( knots[bsplineCurve_.degree()]);
  float  maxu( knots[numKnots - order]);
  std::cout << "minu = " << minu << ", maxu = " << maxu << std::endl;

  // control points of 1d texture (0, 1)
  GLfloat   tcoords[2] = {0.0, 1.0};

  // knots of domain, over which tcoords shall be linearly interpolated
//   GLfloat   tknots[2] = {minu, maxu};
  GLfloat   tknots[4] = {minu, minu, maxu, maxu};
//   GLfloat   tknots[4] = {minu/(maxu - minu), minu/(maxu - minu), maxu/(maxu - minu), maxu/(maxu - minu)};

  // begin drawing nurbs
  gluBeginCurve(theNurb);

  // first enable texture coordinate mapping
  gluNurbsCurve(theNurb, 4, tknots, 1, tcoords, 2, GL_MAP1_TEXTURE_COORD_1);
//   gluNurbsCurve(theNurb, 4, tknots, 1, &tcoords[0], 2, GL_MAP1_TEXTURE_COORD_1);
//   gluNurbsCurve(theNurb, numKnots, knots, 3, ctlpoints, order, GL_MAP1_TEXTURE_COORD_1);

  // draw surface
  gluNurbsCurve(theNurb, numKnots, knots, 3, ctlpoints, order, GL_MAP1_VERTEX_3);
  gluEndCurve(theNurb);

  gluDeleteNurbsRenderer(theNurb);

  delete[] knots;
  delete[] ctlpoints;
}
*/

//----------------------------------------------------------------------------

/*
template <class BSplineCurve>
void
BSplineCurveNodeT<BSplineCurve>::
drawDirectMode(DrawModes::DrawMode _drawMode, GLState& _state)
{
  // draw the curve
  if ((_drawMode & DrawModes::WIREFRAME) && render_bspline_curve_)
  {
//     float line_width_old = _state.line_width();
//     glLineWidth(line_width_old + 2.0);

    glBegin(GL_LINE_STRIP);
    for (unsigned int i = 0; i < curve_samples_.size(); ++i)
    {
      Vec3d pos = curve_samples_[i].first;
      Vec4f col = curve_samples_[i].second;

      glColor(col);
      glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();

//     glLineWidth(line_width_old);
  }
}
*/

//----------------------------------------------------------------------------

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
