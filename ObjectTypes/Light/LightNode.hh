/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 8548 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2010-02-12 14:40:13 +0100 (Fr, 12. Feb 2010) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS LightNode
//
//=============================================================================

#ifndef ACG_LIGHT_NODE_HH
#define ACG_LIGHT_NODE_HH


//== INCLUDES =================================================================

#include "ACG/Scenegraph/BaseNode.hh"
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "ACG/GL/gl.hh"
#include <string>
#include <vector>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {
  

//== CLASS DEFINITION =========================================================

/** \class LightNode LightNode.hh <ACG/Scenegraph/LightNode.hh>

    Set LightSources (0 to 7) for this node and all its children.  
    All changes will be done in the enter() method undone
    in the leave() method.
**/

/// Structure to hold options for one LightSource
class DLLEXPORT LightSource
{
  // Declare Node as friend for direct access to memberss
  friend class LightNode;
  
public:
    
  /// Default Constructor
  LightSource();
  
  /** \brief Set position for LightSource
  * 
  * This function sets the position of a light source. OpenGL handles
  * a position for a light source if the homogeneous coordinate is
  * 1.0. If it is 0.0 the light source will be directional. So a
  * 1.0 is set in this function to get a positioned light source.
  */
  void position( Vec3d _pos);
  
  /** \brief Get the position of the LightSource
  *
  * Be careful with this function. Check if the light source is not directional with
  * the directional() function
  */
  Vec3d position();
  
  /** \brief Check if the light source is a directional light source
  *
  * If this light Source is a directional light source the function will return true.
  * Otherwise false as the light source has a position.
  */
  bool directional();
  
  /** \brief Get direction of the light source
  *
  * Get direction of light.
  */
  Vec3d direction();
  
  /** \brief Set the direction of the light source
  *
  * This function creates a directional light source. The position is not used by OpenGL.
  * Internally the positions homogeneous coordinate is set to 0.0.
  */
  void direction( Vec3d _pos);
  
   /// enable LightSource 
  void enable();

  /// disable LightSource 
  void disable();
  
  /// Get light source status
  bool enabled();
  
  /** \brief Set spot direction 
  *
  * \todo Improve documentation
  */
  void spotDirection( Vec3d _pos);
  
  /** \brief get spot direction
  *
  * \todo Improve documentation
  */
  Vec3d spotDirection( );

  /// set Ambient color for LightSource 
  void ambientColor(  Vec4f _color);
  
  /// get Ambient color for LightSource 
  Vec4f ambientColor();

  /// set Diffuse color for LightSource 
  void diffuseColor(  Vec4f _color);

  /// get Diffuse color for LightSource 
  Vec4f diffuseColor();
  
  /// set Specular color for LightSource 
  void specularColor(  Vec4f _color);
  
  /// get Specular color for LightSource 
  Vec4f specularColor();

  /// make LightSource fixed or moveable with ModelViewMatrix
  void fixedPosition( bool _state);
  
  bool fixedPosition();
  
  void spotExponent(float _exponent);
  
  float spotExponent();
  
  void spotCutoff(float _cutoff);
  
  float spotCutoff();
  
  void constantAttenuation(float _constantAttenuation);
  
  float constantAttenuation();
  
  void linearAttenuation(float _linearAttenuation);
  
  float linearAttenuation();
  
  void quadraticAttenuation(float _quadraticAttenuation);
  
  float quadraticAttenuation();
  
protected:

  Vec4f position_;
  
  bool enabled_;
  bool fixedPosition_;
  Vec4f ambientColor_;
  Vec4f diffuseColor_;
  Vec4f specularColor_;
  
  Vec4f realPosition_;
  Vec3d spotDirection_;
  float spotExponent_;
  float spotCutoff_;
  float constantAttenuation_;
  float linearAttenuation_;
  float quadraticAttenuation_;
};


class DLLEXPORT LightNode : public BaseNode
{

public:

  /// Default constructor. Applies all properties.
  LightNode( BaseNode*           _parent = 0,
		   const std::string&  _name = "<LightNode>");

  /// Destructor.
  virtual ~LightNode();
  
  /// Set the light source parameters
  void setLightSource(LightSource _light ) { light_ = _light; };

  ACG_CLASSNAME(LightNode);

  /// set current Light Sources
  void enter(GLState& _state, DrawModes::DrawMode _drawmode);
  /// restores original Light Sources
  void leave(GLState& _state, DrawModes::DrawMode _drawmode);

 
  

private:

  /// set _light Options in OpenGL for GL_LIGHT#_index
  void setParameters(GLenum _index, LightSource& _light);

  /// get _light Options in OpenGL for GL_LIGHT#_index
  void getParameters(GLenum _index, LightSource& _light);

private:

  GLenum      lightId_;
  
  /// store LightSources of this node
  LightSource light_;

  /// save old LightSources
  LightSource lightSave_;
};

/**
* \class LightSourceHandle
*
* Assign a unique OpenGL light source identifier to each of the
* light source nodes in the scene. These relations are
* stored in a map. If there is no free light source left,
* return GL_INVALID_ENUM as light source enumerant.
*/
class DLLEXPORT LightSourceHandle {
    public:
        LightSourceHandle() {
            // Get max number of lights
            GLint maxLights;
            glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
            for(int i = 0; i < maxLights; ++i) {
                lights_.insert(std::pair<GLenum, LightNode*>(GL_LIGHT0 + i, 0));
            }
        };
        
        GLenum getLight(LightNode* _node) {
            GLenum light = GL_INVALID_ENUM;
            for(std::map<GLenum, LightNode*>::iterator it = lights_.begin();
                it != lights_.end(); ++it) {
                
                if(it->second == 0) {
                    lights_[it->first] = _node;
                    light = it->first;
                    break;
                }
            }
            return light;
        };
        
        void removeLight(LightNode* _node) {
            for(std::map<GLenum, LightNode*>::iterator it = lights_.begin();
                it != lights_.end(); ++it) {
                
                if(it->second == _node) {
                    lights_[it->first] = 0;
                    break;
                }
            }
        };
        
    private:
        std::map<GLenum, LightNode*> lights_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LIGHT_NODE_HH defined
//=============================================================================

