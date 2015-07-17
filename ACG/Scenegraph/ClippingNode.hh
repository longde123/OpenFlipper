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
//  CLASS ClippingNode
//
//=============================================================================


#ifndef ACG_CLIPPING_NODE_HH
#define ACG_CLIPPING_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <string>
#include <fstream>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {
  

//== CLASS DEFINITION =========================================================

  
/** \class ClippingNode ClippingNode.hh <ACG/Scenegraph/ClippingNode.hh>

    Set material and some other stuff for this node and all its
    children.  All changes will be done in the enter() method undone
    in the leave() method.
**/

class ACGDLLEXPORT ClippingNode : public BaseNode
{
public:

  /// Default constructor. Applies all properties.
  ClippingNode( BaseNode*           _parent = 0,
		const std::string&  _name = "<ClippingNode>" )
    : BaseNode(_parent, _name),
      slice_width_(0),
      offset_(0)
  {
    offset_plane0_[0] = 0.0;
    offset_plane0_[1] = 0.0;
    offset_plane0_[2] = 0.0;
    offset_plane0_[3] = 0.0;

  }


  /// Destructor.
  virtual ~ClippingNode() {}

  /// set class name
  ACG_CLASSNAME(ClippingNode);

  /// set texture
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// restores original texture (or no-texture)
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// set position and normal of plane
  void set_plane(const Vec3f& _position, const Vec3f& _normal, float _eps=0.0);

  /// get position
  const Vec3f& position() const { return position_; }

  /// get normal
  const Vec3f& normal() const { return normal_; }

  /// get slice width
  float slice_width() const { return slice_width_; }

  /// sweep plane along normal by _dist
  void set_offset(float _dist);

  
private:

  Vec3f     position_, normal_;
  GLdouble  plane0_[4], plane1_[4], offset_plane0_[4], offset_plane1_[4];
  float     slice_width_, offset_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_CLIPPING_NODE_HH defined
//=============================================================================
