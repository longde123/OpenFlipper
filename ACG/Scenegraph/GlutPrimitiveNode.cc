/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
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
//  CLASS GlutPrimitiveNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "GlutPrimitiveNode.hh"

#include <ACG/GL/IRenderer.hh>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 

GlutPrimitiveNode::GlutPrimitiveNode( BaseNode*         _parent,
                                      std::string       _name )
  : BaseNode(_parent, _name),
    setColor_(true)
{
  const int slices = 20;
  const int stacks = 20;

  sphere_   = new ACG::GLSphere(slices,stacks);
};

//----------------------------------------------------------------------------

GlutPrimitiveNode::GlutPrimitiveNode(GlutPrimitiveType _type,
                                     BaseNode* _parent,
                                     std::string _name) :
        BaseNode(_parent, _name),
        setColor_(true)
{
  const int slices = 20;
  const int stacks = 20;

  // add a single primitive of the given type
  Primitive p(_type);
  primitives_.push_back(p);

  sphere_ = new ACG::GLSphere(slices, stacks);
}

void 
GlutPrimitiveNode::
set_position(const Vec3d& _p, int _idx)
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    primitives_[_idx].position = _p; 
}

//----------------------------------------------------------------------------

const Vec3d
GlutPrimitiveNode::
get_position(int _idx) const 
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    return primitives_[_idx].position; 
  
  return Vec3d(-1,-1,-1);
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
set_size(double _s, int _idx) 
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    primitives_[_idx].size = _s; 
}

//----------------------------------------------------------------------------

double
GlutPrimitiveNode::
get_size(int _idx) const
{
  if (_idx > -1 && _idx < (int)primitives_.size())
    return primitives_[_idx].size; 
  return -1;
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (int i = 0; i < (int)primitives_.size(); ++i)
  {
    Vec3d sizeVec(primitives_[i].size, primitives_[i].size, primitives_[i].size);
    _bbMax.maximize(primitives_[i].position + sizeVec);
    _bbMin.minimize(primitives_[i].position - sizeVec);
  }
}

//----------------------------------------------------------------------------
  
DrawModes::DrawMode
GlutPrimitiveNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS              |
	   DrawModes::WIREFRAME           |
	   DrawModes::HIDDENLINE          |
	   DrawModes::SOLID_FLAT_SHADED   |
	   DrawModes::SOLID_SMOOTH_SHADED |
	   DrawModes::SOLID_FACES_COLORED );
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{  
  float backupColorDiffuse[4];
  float backupColorAmbient[4];
  if ( setColor_ ) {
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glGetFloatv(GL_CURRENT_COLOR,backupColorDiffuse);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glGetFloatv(GL_CURRENT_COLOR,backupColorAmbient);
  }
  for (size_t i = 0; i < primitives_.size(); ++i)
  {

  
    glPushMatrix();
    glTranslatef(primitives_[i].position[0], primitives_[i].position[1], primitives_[i].position[2]);


    if (_drawMode & DrawModes::POINTS)
    {
      ACG::GLState::disable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_FLAT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      draw_obj(i);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    if (_drawMode & DrawModes::WIREFRAME)
    {
      ACG::GLState::disable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_FLAT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      draw_obj(i);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (_drawMode & DrawModes::SOLID_FACES_COLORED)
    {
      ACG::GLState::disable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_FLAT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      
      if ( setColor_ ) {
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor(primitives_[i].color);
      }

      draw_obj(i);
      
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (_drawMode & DrawModes::HIDDENLINE)
    {
      Vec4f base_color_backup = _state.base_color();

      ACG::GLState::disable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_FLAT);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glColor(_state.clear_color());
      ACG::GLState::depthRange(0.01, 1.0);
      draw_obj(i);

      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glColor(base_color_backup);
      ACG::GLState::depthRange(0.0, 1.0);
      draw_obj(i);

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }


    if (_drawMode & DrawModes::SOLID_FLAT_SHADED)
    {
      ACG::GLState::enable( GL_COLOR_MATERIAL );
      ACG::GLState::enable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_FLAT);
      
      if ( setColor_ ) {
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor(primitives_[i].color);
      }
      
      draw_obj(i);
    }


    if (_drawMode & DrawModes::SOLID_SMOOTH_SHADED)
    {
      ACG::GLState::enable( GL_COLOR_MATERIAL );
      ACG::GLState::enable(GL_LIGHTING);
      ACG::GLState::shadeModel(GL_SMOOTH);
      
      if ( setColor_ ) {
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor(primitives_[i].color);
      }

      draw_obj(i);
    }

    glPopMatrix();
    if ( setColor_ ) {
      glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
      glColor4fv(backupColorDiffuse);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
      glColor4fv(backupColorAmbient);
    }
  } // end of primitives iter
}

//----------------------------------------------------------------------------

size_t
GlutPrimitiveNode::
add_primitive(GlutPrimitiveType _type, Vec3d _pos, Vec3d _axis, ACG::Vec4f _color)
{
  Primitive p(_type, _pos, _axis, _color);
  primitives_.push_back(p);
  return primitives_.size() - 1;
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::draw_obj(int _idx) const
{
  if (_idx < 0 || _idx >= (int)primitives_.size()) // range check
    return;

  Vec3d axis  = primitives_[_idx].axis;
  double size = axis.norm();

  if (size > 1e-10)
  {
    glPushMatrix();

    Vec3d direction = axis;
    Vec3d z_axis(0,0,1);
    Vec3d rot_normal;
    double rot_angle;

    direction.normalize();
    rot_angle  = acos((z_axis | direction)) * 180 / M_PI;
    rot_normal = ((z_axis % direction).normalize());
  

    if (fabs(rot_angle) > 0.0001 && fabs(180 - rot_angle) > 0.0001)
      glRotatef(rot_angle,rot_normal[0], rot_normal[1], rot_normal[2]);
    else
      glRotatef(rot_angle,1,0,0);

    
    switch (primitives_[_idx].type)
    {
      case CONE: 
        glutSolidCone(primitives_[_idx].size, primitives_[_idx].innersize, primitives_[_idx].slices, primitives_[_idx].stacks);
        break;

      case CUBE: 
        glutSolidCube(primitives_[_idx].size);
        break;
      
      case DODECAHEDRON: 
        glutSolidDodecahedron();
        break;
      
      case ICOSAHEDRON: 
        glutSolidIcosahedron();
        break;

      case OCTAHEDRON:
        glutSolidOctahedron();
        break;

      case  SPHERE: 
        glutSolidSphere(primitives_[_idx].size, primitives_[_idx].slices, primitives_[_idx].stacks);
        break;
        
      case TEAPOT: 
        glutSolidTeapot(primitives_[_idx].size);
        break;

      case TETRAHEDRON: 
        glutSolidTetrahedron();
        break;

      case TORUS: 
        glutSolidTorus(primitives_[_idx].innersize, primitives_[_idx].size, primitives_[_idx].slices, primitives_[_idx].stacks);
        break;
    }

    glPopMatrix();
  }
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
pick(GLState& _state , PickTarget _target)
{
  // initialize picking stack
  if (!_state.pick_set_maximum (primitives_.size()))
  {
    std::cerr << "Strange pickSetMaximum failed for index " << primitives_.size() << " in GlutPrimitiveNode\n";
    return;
  }

  switch (_target)
  {
    case PICK_ANYTHING:
    case PICK_FACE: 
    { 
      for (int i = 0; i < (int)primitives_.size(); ++i)
      {
        _state.pick_set_name(i);
        glPushMatrix();
        glTranslatef(primitives_[i].position[0], primitives_[i].position[1], primitives_[i].position[2]);
        draw_obj(i);
        glPopMatrix();
      }
      break; 
    }

    default:
      break;
  }      
}

//----------------------------------------------------------------------------

void
GlutPrimitiveNode::
getRenderObjects(IRenderer* _renderer, GLState&  _state , const DrawModes::DrawMode&  _drawMode , const Material* _mat) {

  // init base render object
  RenderObject ro;
  ro.initFromState(&_state);

  // the selection sphere uses alpha blending against scene meshes
  //  set priority-order > 0 to draw this after meshes
  ro.priority = 1;

  // enable depth-test
  ro.depthTest = true;

  for (int i = 0; i < (int)primitives_.size(); ++i)
  {

    // Set the right position
    _state.push_modelview_matrix();
    _state.translate(primitives_[i].position);
    ro.modelview = _state.modelview();
    _state.pop_modelview_matrix();

    Material localMaterial = *_mat;
    if (setColor_)
    {
      //localMaterial.color(primitives_[i].color);
      //localMaterial.ambientColor(primitives_[i].color);
      localMaterial.diffuseColor(primitives_[i].color);
      localMaterial.baseColor(primitives_[i].color * .5f);
    }

    ro.setMaterial(&localMaterial);
    ro.shaderDesc.shadeMode = SG_SHADE_PHONG;

    switch (primitives_[i].type) {
      case SPHERE:

        // Sphere
        ro.debugName = (QString("glutprimitive.sphere no %1: ").arg(i)+QString(name().c_str())).toLatin1();

        sphere_->addToRenderer(_renderer, &ro, primitives_[i].size);

        break;

      default:
        // TODO: The other glut primitives are not yet supported by the advanced renderers
        std::cerr << "Sorry, but the glut renderer objects are not available for this renderer yet!" << std::endl;
        break;
    }


  }

}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
