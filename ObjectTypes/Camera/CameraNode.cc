/* ========================================================================= *
 *                                                                           *
 *                               OpenFlipper                                 *
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
 * ========================================================================= */

/*===========================================================================*\
 *
 *   $Revision$
 *   $Date$
 *
\*===========================================================================*/





//=============================================================================
//
//  CLASS GlutPrimitiveNode - IMPLEMENTATION
//
//=============================================================================

#define CAMERAVISNODE_C

//== INCLUDES =================================================================
#include "CameraNode.hh"

#include <ACG/GL/IRenderer.hh>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

const float axis_length = 0.1f;

//== IMPLEMENTATION ==========================================================

/// Default constructor.
CameraNode::CameraNode(BaseNode* _parent, std::string _name) :
    BaseNode(_parent, _name),
    bbmin_(FLT_MAX,FLT_MAX,FLT_MAX),
    bbmax_(FLT_MIN,FLT_MIN,FLT_MIN),
    cylinder_(0),
    cone_(0),
    showFrustum_(false) {

    modelView_.identity();

    projection_.identity();
    // Setup a standard projection ( Full fovy 90, aspect 1.0, near 1.0, far 2.0 )
    projection_.perspective(45 ,1.0,1.0,2.0);

    far_ = 2.0;
    near_ = 1.0;


    width_  = 500;
    height_ = 500;

    updateBoundingBoxes(modelView_);

    cylinder_ = new GLCylinder(8, 4, 1.0f, false, false);
    cone_ = new GLCone(8, 1, 1.0f, 0.0f, true, false);
}

CameraNode::~CameraNode() {
  if (cylinder_)
    delete cylinder_;

  if (cone_)
    delete cone_;
}

void CameraNode::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax) {
    _bbMin.minimize(bbmin_);
    _bbMax.maximize(bbmax_);
}

//----------------------------------------------------------------------------

DrawModes::DrawMode CameraNode::availableDrawModes() const {
    return (DrawModes::POINTS |
            DrawModes::WIREFRAME |
            DrawModes::HIDDENLINE |
            DrawModes::SOLID_FLAT_SHADED |
            DrawModes::SOLID_SMOOTH_SHADED);
}

//----------------------------------------------------------------------------

void CameraNode::draw(GLState& _state, const DrawModes::DrawMode& /*_drawMode*/) {

    glPushAttrib(GL_LIGHTING_BIT);
    glPushAttrib(GL_ENABLE_BIT);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::enable(GL_LIGHTING); // Turn lighting on

    // Store modelview matrix
    _state.push_modelview_matrix();

    Vec4f lastBaseColor     = _state.base_color();
    Vec4f lastDiffuseColor  = _state.diffuse_color();
    Vec4f lastSpecularColor = _state.specular_color();

    // Set modelview matrix such that it matches
    // the remote settings (+ the local transformation).
    // This is performed by multiplying the local
    // modelview matrix by the inverse remote
    // modelview matrix: M_l' = M_l * M^{-1}_r
    ACG::GLMatrixd modelview = _state.modelview();
    _state.set_modelview(modelview * modelView_);
    
    // Update bounding box data and clipped_ flag
    updateBoundingBoxes(modelview);

    _state.set_base_color(ACG::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _state.set_diffuse_color(ACG::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    _state.set_specular_color(ACG::Vec4f(1.0f, 1.0f, 0.0f, 1.0f));

    // Draw camera box
    
    glPushAttrib(GL_LIGHTING_BIT);
    ACG::GLState::disable(GL_LIGHTING); // Disable lighting
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-half_width_, -half_height_, -near_);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(half_width_, -half_height_, -near_);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-half_width_, half_height_, -near_);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(half_width_, half_height_, -near_);

    glVertex3f(-half_width_, -half_height_, -near_);
    glVertex3f(half_width_, -half_height_, -near_);

    glVertex3f(-half_width_, -half_height_, -near_);
    glVertex3f(-half_width_, half_height_, -near_);

    glVertex3f(half_width_, half_height_, -near_);
    glVertex3f(half_width_, -half_height_, -near_);

    glVertex3f(half_width_, half_height_, -near_);
    glVertex3f(-half_width_, half_height_, -near_);
    glEnd();
    glPopAttrib();

    // Render frustum
    if(showFrustum_) {

        // Draw left side of frustum
        ACG::GLState::enable (GL_BLEND);
        glPushAttrib(GL_LIGHTING_BIT);
        ACG::GLState::disable(GL_LIGHTING); // Disable lighting
        
        glColor4f(0.0f, 0.5f, 0.0f, 1.0f);
        
        glBegin(GL_LINES);
        
        // Top plane
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        glVertex3f(-far_half_width_, far_half_height_, -far_);
        
        glVertex3f(-far_half_width_, far_half_height_, -far_);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        
        // Bottom plane
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, far_half_height_, -far_);
        
        glVertex3f(far_half_width_, far_half_height_, -far_);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        // Left
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-far_half_width_, far_half_height_, -far_);
        
        glVertex3f(-far_half_width_, far_half_height_, -far_);
        glVertex3f(far_half_width_, far_half_height_, -far_);
        
        glVertex3f(far_half_width_, far_half_height_, -far_);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        // Right
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        // Far
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, far_half_height_, -far_);
        
        glVertex3f(far_half_width_, far_half_height_, -far_);
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        
        glEnd();

        glColor4f(0.0f, 1.0f, 0.0f, 0.01f);

        glBegin(GL_TRIANGLES);

        // Top plane
        glVertex3f(-half_width_, -half_height_, -1.0f);
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        glVertex3f(-far_half_width_, far_half_height_, -far_);

        glVertex3f(-far_half_width_, far_half_height_, -far_);
        glVertex3f(-half_width_, half_height_, -1.0f);
        glVertex3f(-half_width_, -half_height_, -1.0f);

        // Bottom plane
        glVertex3f(half_width_, -half_height_, -1.0f);
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, far_half_height_, -far_);

        glVertex3f(far_half_width_, far_half_height_, -far_);
        glVertex3f(half_width_, half_height_, -1.0f);
        glVertex3f(half_width_, -half_height_, -1.0f);

        // Left
        glVertex3f(-half_width_, half_height_, -1.0f);
        glVertex3f(-far_half_width_, far_half_height_, -far_);
        glVertex3f(far_half_width_, far_half_height_, -far_);

        glVertex3f(far_half_width_, far_half_height_, -far_);
        glVertex3f(half_width_, half_height_, -1.0f);
        glVertex3f(-half_width_, half_height_, -1.0f);

        // Right
        glVertex3f(-half_width_, -half_height_, -1.0f);
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, -far_half_height_, -far_);

        glVertex3f(far_half_width_, -far_half_height_, -far_);
        glVertex3f(half_width_, -half_height_, -1.0f);
        glVertex3f(-half_width_, -half_height_, -1.0f);

        // Far
        glVertex3f(-far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, -far_half_height_, -far_);
        glVertex3f(far_half_width_, far_half_height_, -far_);

        glVertex3f(far_half_width_, far_half_height_, -far_);
        glVertex3f(-far_half_width_, far_half_height_, -far_);
        glVertex3f(-far_half_width_, -far_half_height_, -far_);

        glEnd();
        

        glPopAttrib(); // LIGHTING
    }

    // Draw right vector
    _state.rotate(90, 0.0, 1.0, 0.0);

    _state.set_base_color(ACG::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
    _state.set_diffuse_color(ACG::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
    _state.set_specular_color(ACG::Vec4f(1.0f, 0.4f, 0.4f, 1.0f));

    cylinder_->setBottomRadius(axis_length/20.0f);
    cylinder_->setTopRadius(axis_length/20.0f);
    cylinder_->draw(_state, axis_length);

    // Draw top
    _state.translate(0.0, 0.0, axis_length );
    cone_->setBottomRadius(axis_length/5.0f);
    cone_->setTopRadius(0.0f);
    cone_->draw(_state, axis_length/2.0f);
    _state.translate(0.0, 0.0, -axis_length );

    // Draw up vector
    _state.rotate(-90, 1.0, 0.0, 0.0);

    _state.set_base_color(ACG::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
    _state.set_diffuse_color(ACG::Vec4f(0.0f, 1.0f, 0.0f, 1.0f));
    _state.set_specular_color(ACG::Vec4f(0.4f, 1.0f, 0.4f, 1.0f));

    cylinder_->draw(_state, axis_length);

    // Draw top
    _state.translate(0.0, 0.0, axis_length );
    cone_->draw(_state, axis_length/2.0f);
    _state.translate(0.0, 0.0, -axis_length );

    // Draw viewing direction vector
    _state.rotate(90, 0.0, 1.0, 0.0);

    _state.set_base_color(ACG::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
    _state.set_diffuse_color(ACG::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
    _state.set_specular_color(ACG::Vec4f(0.4f, 0.4f, 1.0f, 1.0f));

    cylinder_->draw(_state, axis_length);

    // Draw top
    _state.translate(0.0, 0.0, axis_length );
    cone_->draw(_state, axis_length/2.0f);
    _state.translate(0.0, 0.0, -axis_length );


    // Reset to previous modelview
    _state.pop_modelview_matrix();

    _state.set_base_color(lastBaseColor);
    _state.set_diffuse_color(lastDiffuseColor);
    _state.set_specular_color(lastSpecularColor);

    glPopAttrib(); // GL_ENABLE_BIT
    glPopAttrib(); // LIGHTING
}

//----------------------------------------------------------------------------

void CameraNode::getRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat)
{
  if (!vbo_.is_valid())
  {
    // cube in clip space
    float data[] = 
    {
      // float4 pos    float2 coeff
      -1, -1, 1, 1,     1, 0, //0  frustum vertices..
      -1, -1, -1, 1,    1, 0, //1
      1, -1, -1, 1,     1, 0, //2
      1, -1, 1, 1,      1, 0, //3
      -1, 1, 1, 1,      1, 0, //4
      1, 1, 1, 1,       1, 0, //5
      1, 1, -1, 1,      1, 0, //6
      -1, 1, -1, 1,     1, 0, //7

      0, 0, 0, 0,       0, 1, //8 cam origin vertex
    };

    vbo_.upload(sizeof(data), data, GL_STATIC_DRAW);

    vdecl_.clear();
    vdecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_POSITION);
    vdecl_.addElement(GL_FLOAT, 2, VERTEX_USAGE_SHADER_INPUT, size_t(0), "inCamOriginCoeff");
  }

  if (!ibo_.is_valid())
  {
    int data[] = 
    {
      // frustum triangles
      3,2,6 , 6,5,3 , // right
      1,0,4 , 4,7,1 , // left
      4,5,6 , 6,7,4 , // top
      0,1,2 , 2,3,0 , // bottom
      0,3,5 , 5,4,0 , // back
//      2,1,7 , 7,6,2 , // front

      // frustum lines
      3,2, 2,6, 6,5, 5,3, // right
      1,0, 0,4, 4,7, 7,1, // left
      4,5, 5,6, 6,7, 7,4, // top
      0,1, 1,2, 2,3, 3,0, // bottom
      0,3, 3,5, 5,4, 4,0, // back
      2,1, 1,7, 7,6, 6,2, // front

      // cam origin to near plane lines
      8,1, 8,2, 8,6, 8,7
    };

    ibo_.upload(sizeof(data), data, GL_STATIC_DRAW);
  }

  GLMatrixf camWorldToClip = projection_ * modelView_;

  GLMatrixf camClipToWorld(camWorldToClip);
  camClipToWorld.invert();

  GLMatrixf camViewToWorld = modelView_;
  camViewToWorld.invert();

  Vec4f camOriginWS(camViewToWorld(0, 3),
    camViewToWorld(1, 3),
    camViewToWorld(2, 3),
    1.0f);

  RenderObject obj;
  obj.initFromState(&_state);
  obj.depthTest = true;
  obj.shaderDesc.shadeMode = SG_SHADE_UNLIT;


  obj.vertexBuffer = vbo_.id();
  obj.indexBuffer = ibo_.id();
  obj.vertexDecl = &vdecl_;


  obj.shaderDesc.vertexTemplateFile = "Camera/vertex.glsl";

  obj.setUniform("clipSpaceToWorld", camClipToWorld);
  obj.setUniform("camOriginWS", camOriginWS);

  bool hasOrigin = projection_.isPerspective();

  GLsizei lineOffset = 5 * 6;

  if (showFrustum_)
  {
    obj.blending = true;
    obj.emissive = Vec3f(0.0f, 1.0f, 0.0f);
    obj.alpha = 0.01f;
    obj.glDrawElements(GL_TRIANGLES, lineOffset, GL_UNSIGNED_INT, 0);
//    _renderer->addRenderObject(&obj);

    GLsizei lineCount = hasOrigin ? 4 * 7 : 4 * 6;

    obj.emissive = Vec3f(0.0f, 0.5f, 0.0f);
    obj.alpha = 1.0f;
    obj.blending = false;
    obj.glDrawElements(GL_LINES, lineCount * 2, GL_UNSIGNED_INT, (GLvoid*)(lineOffset * sizeof(int)));
    _renderer->addRenderObject(&obj);
  }
  else
  {
    obj.emissive = Vec3f(0.0f, 0.5f, 0.0f);

    // only front plane
    GLsizei lineCount = hasOrigin ? 4 * 1 : 4 * 2;
    lineOffset += 4 * 5;

    obj.glDrawElements(GL_LINES, lineCount, GL_UNSIGNED_INT, (GLvoid*)(lineOffset * sizeof(int)));
    _renderer->addRenderObject(&obj);
  }
}

//----------------------------------------------------------------------------

void CameraNode::pick(GLState& _state, PickTarget /*_target*/) {

    _state.pick_set_maximum(2);

    _state.pick_set_name(0);

    // Store modelview matrix
    _state.push_modelview_matrix();

    // Set modelview matrix such that it matches
    // the remote settings (+ the local transformation).
    // This is performed by multiplying the local
    // modelview matrix by the inverse remote
    // modelview matrix: M_l' = M_l * M^{-1}_r
    ACG::GLMatrixd modelview = _state.modelview();
    _state.set_modelview(modelview * modelView_);
    
    // Update bounding box data and clipped_ flag
    updateBoundingBoxes(modelview);

    // Draw camera box
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-half_width_, -half_height_, -near_);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(half_width_, -half_height_, -near_);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-half_width_, half_height_, -near_);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(half_width_, half_height_, -near_);

    glVertex3f(-half_width_, -half_height_, -near_);
    glVertex3f(half_width_, -half_height_, -near_);

    glVertex3f(-half_width_, -half_height_, -near_);
    glVertex3f(-half_width_, half_height_, -near_);

    glVertex3f(half_width_, half_height_, -near_);
    glVertex3f(half_width_, -half_height_, -near_);

    glVertex3f(half_width_, half_height_, -near_);
    glVertex3f(-half_width_, half_height_, -near_);
    glEnd();


    _state.pick_set_name(1);

    // Draw right vector
    _state.rotate(90, 0.0, 1.0, 0.0);

    cylinder_->setBottomRadius(axis_length/20.0f);
    cylinder_->setTopRadius(axis_length/20.0f);
    cylinder_->draw(_state, axis_length);

    // Draw top
    _state.translate(0.0, 0.0, axis_length );
    cone_->setBottomRadius(axis_length/5.0f);
    cone_->setTopRadius(0.0f);
    cone_->draw(_state, axis_length/2.0f);
    _state.translate(0.0, 0.0, -axis_length );

    // Draw up vector
    _state.rotate(-90, 1.0, 0.0, 0.0);

    cylinder_->draw(_state, axis_length);

    // Draw top
    _state.translate(0.0, 0.0, axis_length );
    cone_->draw(_state, axis_length/2.0f);
    _state.translate(0.0, 0.0, -axis_length );

    // Draw viewing direction vector
    _state.rotate(90, 0.0, 1.0, 0.0);

    cylinder_->draw(_state, axis_length);

    // Draw top
    _state.translate(0.0, 0.0, axis_length );
    cone_->draw(_state, axis_length/2.0f);
    _state.translate(0.0, 0.0, -axis_length );

    // Reset to previous modelview
    _state.pop_modelview_matrix();
}

//----------------------------------------------------------------------------

void CameraNode::updateBoundingBoxes(GLMatrixd& _modelview) {

    // Get fovy of remote projection
    fovy_ = atan(1/projection_(1,1)) * 2;

    // Set bounding box of camera to be of sufficient
    // size to cover any camera rotation.
    // Note: 1.41421 = sqrt(2)

    aspectRatio_ = (double)width_ / (double)height_;

    half_height_ = height_/2.0;
    half_width_ = aspectRatio_ * half_height_;

    if(showFrustum_) {
        far_half_height_ = tan(fovy_/2) * far_;
        far_half_width_ = far_half_height_ * aspectRatio_;
    }
    
    OpenMesh::Vec3d e = OpenMesh::Vec3d(modelView_(0,3), modelView_(1,3), modelView_(2,3));
  
    OpenMesh::Vec3d tmp(std::max(1.41421, half_width_), std::max(1.41421, half_height_), 1.41421);

    bbmin_ = e - tmp;
    bbmax_ = e + tmp;
}



//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
