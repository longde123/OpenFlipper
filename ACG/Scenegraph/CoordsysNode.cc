/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS CoordsysNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "CoordsysNode.hh"
#include <ACG/GL/gl.hh>

#include <iostream>
#include <math.h>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


CoordsysNode::CoordsysNode(BaseNode* _parent, std::string _name, CoordsysMode _mode, ProjectionMode _projectionMode) :
        BaseNode(_parent, _name),
        mode_(_mode),
        projectionMode_(_projectionMode)
{
  const double bodyRadius = 0.004;
  const double topRadius = 0.01;
  const int slices = 10;
  const int stacks = 10;

  sphere_   = new ACG::GLSphere(slices,stacks);
  cylinder_ = new ACG::GLCylinder(slices, stacks, bodyRadius,false,false);
  cone_     = new ACG::GLCone(slices, stacks, 0, topRadius , false,true);
}


void
CoordsysNode::
boundingBox(Vec3d& /*_bbMin*/, Vec3d& /*_bbMax*/)
{
	//_bbMin.minimize( Vect3f  )
}


//----------------------------------------------------------------------------


DrawModes::DrawMode
CoordsysNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS |
	        DrawModes::POINTS_SHADED |
	        DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------

void
CoordsysNode::
drawCoordsys( GLState&  _state) {

  const double arrowLength  = 0.03;
  const double bodyLength   = 0.06;
  const double sphereRadius = 0.01;


  ACG::Vec4f matCol(0.5f, 0.5f, 0.5f, 1.0f);

  // Origin
  glColor4f(0.5, 0.5, 0.5 , 1.0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (matCol * 0.5f).data());
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matCol.data());
  sphere_->draw(_state,sphereRadius);
  
  // X-Axis
  glColor4f(1.0, 0.0, 0.0, 1.0);
  matCol[0] = 1.0f;  matCol[1] = 0.0f; matCol[2] = 0.0f;
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (matCol * 0.5f).data());
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matCol.data());
  _state.push_modelview_matrix ();
  _state.rotate (-90, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  cylinder_->draw(_state,bodyLength);
  _state.translate ( 0, 0, -arrowLength );
  cone_->draw(_state,arrowLength);
  _state.pop_modelview_matrix ();

  // Y-Axis
  glColor4f(0.0, 1.0, 0.0, 1.0);
  matCol[0] = 0.0f;  matCol[1] = 1.0f; matCol[2] = 0.0f;
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (matCol * 0.2f).data());
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matCol.data());
  _state.push_modelview_matrix ();
  _state.rotate (90, 1, 0, 0);
  _state.translate ( 0, 0, -bodyLength );
  cylinder_->draw(_state,bodyLength);
  _state.translate ( 0, 0, -arrowLength );
  cone_->draw(_state,arrowLength);
  _state.pop_modelview_matrix ();

  // Z-Axis
  glColor4f(0.0, 0.0, 1.0, 1.0);
  matCol[0] = 0.0f;  matCol[1] = 0.0f; matCol[2] = 1.0f;
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (matCol * 0.5f).data());
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matCol.data());
  _state.push_modelview_matrix ();
  _state.rotate (180, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  cylinder_->draw(_state,bodyLength);
  _state.translate ( 0, 0, -arrowLength );
  cone_->draw(_state,arrowLength);
  _state.pop_modelview_matrix ();

}

//============================================================================

void
CoordsysNode::drawCoordsysPick( GLState&  _state) {

  const double arrowLength  = 0.03;
  const double bodyLength   = 0.06;
  const double sphereRadius = 0.01;

  // Origin
  _state.pick_set_name (1);
  sphere_->draw(_state,sphereRadius);

  // X-Axis
  _state.pick_set_name (2);
  _state.push_modelview_matrix ();
  _state.rotate (-90, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  cylinder_->draw(_state,bodyLength);
  _state.translate ( 0, 0, -arrowLength );
  cone_->draw(_state,arrowLength);
  _state.pop_modelview_matrix ();


  // Y-Axis
  _state.pick_set_name (3);
  _state.push_modelview_matrix ();
  _state.rotate (90, 1, 0, 0);
  _state.translate ( 0, 0, -bodyLength );
  cylinder_->draw(_state,bodyLength);
  _state.translate ( 0, 0, -arrowLength );
  cone_->draw(_state,arrowLength);
  _state.pop_modelview_matrix ();

  // Z-Axis
  _state.pick_set_name (4);
  _state.push_modelview_matrix ();
  _state.rotate (180, 0, 1, 0);
  _state.translate ( 0, 0, -bodyLength );
  cylinder_->draw(_state,bodyLength);
  _state.translate ( 0, 0, -arrowLength );
  cone_->draw(_state,arrowLength);
  _state.pop_modelview_matrix ();

}


//============================================================================


void
CoordsysNode::
draw(GLState&  _state  , const DrawModes::DrawMode& /*_drawMode*/)
{
  GLenum prev_depth = _state.depthFunc();
    
  GLboolean colorMask[4];
  glGetBooleanv (GL_COLOR_WRITEMASK, colorMask);
  
  // Push Modelview-Matrix
  _state.push_modelview_matrix();

  Vec4f lastBaseColor = _state.base_color();

  glPushAttrib( GL_LIGHTING_BIT ); // STACK_ATTRIBUTES <- LIGHTING_ATTRIBUTE
  ACG::GLState::enable(GL_LIGHTING);
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
  ACG::GLState::enable(GL_COLOR_MATERIAL);
  ACG::GLState::shadeModel(GL_SMOOTH);

  GLfloat zeroVec[4] = {0.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zeroVec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zeroVec);

  // Init state - changes when mode_ != POSITION
  Vec3d pos3D(0.0,0.0,0.0);

  if ( mode_ == SCREENPOS ) {

    int left, bottom, width, height;
    double aspect = _state.aspect();

    _state.get_viewport(left, bottom, width, height);

    // Projection reset
    _state.push_projection_matrix();
    _state.reset_projection();

    if (projectionMode_ == PERSPECTIVE_PROJECTION)
        _state.perspective(45.0, aspect, 0.8, 20.0);
    else
        _state.ortho(-0.65*aspect, 0.65*aspect, -0.65, 0.65, 0.8, 20.0);

    _state.push_modelview_matrix();
    _state.reset_modelview();

    float rel_size = 50.0;
    float projdist = sqrt ( (width*height) / rel_size );

    float posx = left + width - projdist ;
    float posy = bottom + height - projdist ;

    // get our desired coordsys position in scene coordinates
    pos3D = _state.unproject (Vec3d (posx, posy, 0.5));
    _state.pop_modelview_matrix();

    // reset scene translation
    // we want only the scene rotation to rotate the coordsys
    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

    _state.set_modelview (modelview);
    _state.translate (pos3D[0], pos3D[1], pos3D[2], MULT_FROM_LEFT);


    // clear the depth buffer behind the coordsys
    ACG::GLState::depthRange (1.0, 1.0);
    ACG::GLState::depthFunc (GL_ALWAYS);

    drawCoordsys(_state);

    ACG::GLState::depthRange (0.0, 1.0);
    ACG::GLState::depthFunc (GL_LESS);

    // draw coordsys
    drawCoordsys(_state);

    // set depth buffer to 0 so that nothing can paint over cordsys
    ACG::GLState::depthRange (0.0, 0.0);
    ACG::GLState::depthFunc (GL_ALWAYS);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    ACG::GLState::depthRange (0.0, 1.0);
    ACG::GLState::depthFunc (prev_depth);
    glColorMask (colorMask[0], colorMask[1], colorMask[2], colorMask[3]);

    // Projection reload
    _state.pop_projection_matrix();


  } else if (mode_ == POSITION) { /* mode_ == POSITION */

    GLMatrixd modelview = _state.modelview();

    modelview(0,3) = 0.0;
    modelview(1,3) = 0.0;
    modelview(2,3) = 0.0;

    _state.set_modelview (modelview);

    // clear depth buffer in coordsys region
    ACG::GLState::depthRange (1.0, 1.0);
    ACG::GLState::depthFunc (GL_ALWAYS);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // draw coordsys in normal mode
    ACG::GLState::depthRange (0.0, 1.0);
    ACG::GLState::depthFunc (GL_LESS);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // set depth buffer to 0 so that nothing can paint over cordsys
    ACG::GLState::depthRange (0.0, 0.0);
    ACG::GLState::depthFunc (GL_ALWAYS);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

    // Koordinatensystem zeichnen
    drawCoordsys(_state);

    // reset to default
    ACG::GLState::depthRange (0.0, 1.0);
    ACG::GLState::depthFunc (prev_depth);
    glColorMask (colorMask[0], colorMask[1], colorMask[2], colorMask[3]);
  }

  glPopAttrib();

  glColor4fv(lastBaseColor.data());

  // Reload old configuration
  _state.pop_modelview_matrix();
}


void
CoordsysNode::
setMode(const CoordsysMode _mode)
{
  mode_ = _mode;
}

void
CoordsysNode::
setProjectionMode(const ProjectionMode _mode)
{
  projectionMode_ = _mode;
}

void
CoordsysNode::
setPosition(const Vec3f& _pos)
{
  pos3f_ = _pos;
}

CoordsysNode::CoordsysMode
CoordsysNode::
getMode() const
{
  return mode_;
}

CoordsysNode::ProjectionMode
CoordsysNode::
getProjectionMode() const
{
  return projectionMode_;
}

void
CoordsysNode::pick(GLState& _state, PickTarget _target)
{
  GLenum prev_depth = _state.depthFunc();  
    
  if (_target == PICK_ANYTHING) {

    GLdouble mat[16];

    // Push Modelview-Matrix
    _state.push_modelview_matrix();
    _state.pick_set_maximum (5);
    _state.pick_set_name (0);

    // Init state - changes when mode_ != POSITION
    Vec3d pos3D(0.0,0.0,0.0);

    if ( mode_ == SCREENPOS ) {

      int left, bottom, width, height;
      double aspect = _state.aspect();

      _state.get_viewport(left, bottom, width, height);
      
      // Projection reset
      _state.push_projection_matrix();
      _state.reset_projection();

      if (projectionMode_ == PERSPECTIVE_PROJECTION)
        _state.perspective(45.0, aspect, 0.8, 20.0);
      else
        _state.ortho(-0.65*aspect, 0.65*aspect, -0.65, 0.65, 0.8, 20.0);

      _state.push_modelview_matrix();
      _state.reset_modelview();

      float rel_size = 50.0;
      float projdist = sqrt ( (width*height) / rel_size );

      float posx = left + width - projdist ;
      float posy = bottom + height - projdist ;

      // get our desired coordsys position in scene coordinates
      pos3D = _state.unproject (Vec3d (posx, posy, 0.5));
      _state.pop_modelview_matrix();

      // reset scene translation
      GLMatrixd modelview = _state.modelview();

      modelview(0,3) = 0.0;
      modelview(1,3) = 0.0;
      modelview(2,3) = 0.0;

      _state.set_modelview (modelview);
      _state.translate (pos3D[0], pos3D[1], pos3D[2], MULT_FROM_LEFT);

      // We don't have access to the pick matrix used during selection buffer picking
      // so we can't draw our pick area circle in this case
      if (_state.color_picking ())
      {
        // clear depth buffer behind coordsys node
        clearPickArea(_state, true, 1.0);

        // Koordinatensystem zeichnen
        drawCoordsysPick(_state);

        // set depth buffer to 0.0 so that nothing can paint above
        clearPickArea(_state, false, 0.0);
      }
      else
      {
        // clear depth buffer in coordsys region
        ACG::GLState::depthRange (1.0, 1.0);
        ACG::GLState::depthFunc (GL_ALWAYS);

        // Koordinatensystem zeichnen
        drawCoordsys(_state);

        // draw coordsys in normal mode
        ACG::GLState::depthRange (0.0, 1.0);
        ACG::GLState::depthFunc (GL_LESS);

        // Koordinatensystem zeichnen
        drawCoordsys(_state);

        // set depth buffer to 0 so tah nothing can paint over cordsys
        ACG::GLState::depthRange (0.0, 0.0);
        ACG::GLState::depthFunc (GL_ALWAYS);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

        // Koordinatensystem zeichnen
        drawCoordsys(_state);

        // reset to default
        ACG::GLState::depthRange (0.0, 1.0);
        ACG::GLState::depthFunc (prev_depth);
        glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      }

      // Projection reload
      _state.pop_projection_matrix();

    } else if (mode_ == POSITION) { /* mode_ == POSITION */

      // The selection buffer picking method might have set a 
      // pick matrix that has been multiplied with the projection matrix.
      // This is the only way to get the gl pick matrix again
      glMatrixMode(GL_PROJECTION);

      glPushMatrix ();
      glMultMatrixd( _state.inverse_projection().get_raw_data());

      glGetDoublev(GL_PROJECTION_MATRIX, mat);

      glPopMatrix ();

      GLMatrixd pickMat (mat);

      glMatrixMode(GL_MODELVIEW);

      GLMatrixd modelview = _state.modelview();

      modelview(0,3) = 0.0;
      modelview(1,3) = 0.0;
      modelview(2,3) = 0.0;

      // We don't have access to the pick matrix used during selection buffer picking
      // so we can't draw our pick area circle in this case
      if (_state.color_picking ())
      {
        // clear depth buffer behind coordsys node
        clearPickArea(_state, true, 1.0);

        // Koordinatensystem zeichnen
        drawCoordsysPick(_state);

        // set depth buffer to 0.0 so that nothing can paint above
        clearPickArea(_state, false, 0.0);
      }
      else
      {
        // clear depth buffer in coordsys region
        ACG::GLState::depthRange (1.0, 1.0);
        ACG::GLState::depthFunc (GL_ALWAYS);

        // Koordinatensystem zeichnen
        drawCoordsys(_state);

        // draw coordsys in normal mode
        ACG::GLState::depthRange (0.0, 1.0);
        ACG::GLState::depthFunc (GL_LESS);

        // Koordinatensystem zeichnen
        drawCoordsys(_state);

        // set depth buffer to 0 so tah nothing can paint over cordsys
        ACG::GLState::depthRange (0.0, 0.0);
        ACG::GLState::depthFunc (GL_ALWAYS);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

        // Koordinatensystem zeichnen
        drawCoordsys(_state);

        // reset to default
        ACG::GLState::depthRange (0.0, 1.0);
        ACG::GLState::depthFunc (prev_depth);
        glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      }
    }
    // Reload old configuration
    _state.pop_modelview_matrix();

  }
}

//----------------------------------------------------------------------------

void CoordsysNode::clearPickArea(GLState&  _state, bool _draw, GLfloat _depth)
{
  GLenum prev_depth = _state.depthFunc();
    
  std::vector<Vec2f> points;
  Vec2f center;
  float radius;

  int left, bottom, width, height;
  _state.get_viewport(left, bottom, width, height);

  GLboolean colorMask[4];
  glGetBooleanv (GL_COLOR_WRITEMASK, colorMask);

  GLUquadricObj *quadric = gluNewQuadric();

  // respect sphere radius
  Vec3d proj = _state.project (Vec3d (-0.01, -0.01, -0.01));
  points.push_back (Vec2f (proj[0], proj[1]));

  proj = _state.project (Vec3d (0.1, 0.0, 0.0));
  points.push_back (Vec2f (proj[0], proj[1]));

  proj = _state.project (Vec3d (0.0, 0.1, 0.0));
  points.push_back (Vec2f (proj[0], proj[1]));

  proj = _state.project (Vec3d (0.0, 0.0, 0.1));
  points.push_back (Vec2f (proj[0], proj[1]));


  // get bounding circle of projected 4 points of the coord node
  boundingCircle(points, center, radius);

  _state.push_projection_matrix();
  _state.reset_projection();

  _state.ortho (left, left + width, bottom, bottom + height, 0.0, 1.0);

  _state.push_modelview_matrix();
  _state.reset_modelview();
  ACG::GLState::depthFunc (GL_ALWAYS);
  ACG::GLState::depthRange (_depth, _depth);
  _state.translate (center[0], center[1], -0.5);

  if (_draw)
    _state.pick_set_name (0);
  else
    glColorMask(false, false, false, false);

  // 10% more to ensure everything is in
  gluDisk( quadric, 0, radius * 1.1, 10, 10 );

  ACG::GLState::depthFunc (prev_depth);
  _state.pop_modelview_matrix();
  _state.pop_projection_matrix();

  ACG::GLState::depthRange (0.0, 1.0);

  if (!_draw)
    glColorMask (colorMask[0], colorMask[1], colorMask[2], colorMask[3]);

  gluDeleteQuadric(quadric);
}

//----------------------------------------------------------------------------

void CoordsysNode::boundingCircle(std::vector<Vec2f> &_in, Vec2f &_center, float &_radius)
{
  if (_in.size () == 0)
    return;
  if (_in.size () < 2)
  {
    _center = _in[0];
    _radius = 0.0f;
    return;
  }
  bool found = false;

  // try all circumcircles of all possible lines
  for (unsigned int i = 0; i < _in.size () - 1; i++)
    for (unsigned int j = i + 1; j < _in.size (); j++)
    {
      Vec2f cen = (_in[i] + _in[j]) * 0.5;
      float rad = (_in[i] - cen).length ();
      bool allin = true;

      for (unsigned int k = 0; k < _in.size (); k++)
        if (k != i && k != j && (_in[k] - cen).length () > rad)
        {
          allin = false;
          break;
        }

      if (!allin)
        continue;

      if (found)
      {
        if (rad < _radius)
        {
          _center = cen;
          _radius = rad;
        }
      }
      else
      {
        found = true;
        _center = cen;
        _radius = rad;
      }
    }

  if (found)
    return;

  // try all circumcircles of all possible triangles
  for (unsigned int i = 0; i < _in.size () - 2; i++)
    for (unsigned int j = i + 1; j < _in.size () - 1; j++)
      for (unsigned int k = j + 1; k < _in.size (); k++)
      {
        float v = ((_in[k][0]-_in[j][0])*((_in[i][0]*_in[i][0])+(_in[i][1]*_in[i][1]))) +
                  ((_in[i][0]-_in[k][0])*((_in[j][0]*_in[j][0])+(_in[j][1]*_in[j][1]))) +
                  ((_in[j][0]-_in[i][0])*((_in[k][0]*_in[k][0])+(_in[k][1]*_in[k][1])));
        float u = ((_in[j][1]-_in[k][1])*((_in[i][0]*_in[i][0])+(_in[i][1]*_in[i][1]))) +
                  ((_in[k][1]-_in[i][1])*((_in[j][0]*_in[j][0])+(_in[j][1]*_in[j][1]))) +
                  ((_in[i][1]-_in[j][1])*((_in[k][0]*_in[k][0])+(_in[k][1]*_in[k][1])));
        float d = (_in[i][0]*_in[j][1])+(_in[j][0]*_in[k][1])+(_in[k][0]*_in[i][1]) -
                  (_in[i][0]*_in[k][1])-(_in[j][0]*_in[i][1])-(_in[k][0]*_in[j][1]);
        Vec2f cen(0.5 * (u/d), 0.5 * (v/d));
        float rad = (_in[i] - cen).length ();
        bool allin = true;

        for (unsigned int l = 0; l < _in.size (); l++)
          if (l != i && l != j && l != k && (_in[l] - cen).length () > rad)
          {
            allin = false;
            break;
          }

        if (!allin)
          continue;

        if (found)
        {
          if (rad < _radius)
          {
            _center = cen;
            _radius = rad;
          }
        }
        else
        {
          found = true;
          _center = cen;
          _radius = rad;
        }
      }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
