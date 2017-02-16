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

#include "SkeletonTransform.hh"
#include <ACG/Geometry/AlgorithmsAngleT.hh>
//-----------------------------------------------------------------------------

/// Le constructeur
SkeletonTransform::SkeletonTransform(Skeleton& _skeleton) : skeleton_(_skeleton)
{
  refPose_ = skeleton_.referencePose();
}

//-----------------------------------------------------------------------------

/** \brief scale all bones of the skeleton by the given factor
 *
 * @param _factor scale factor
 * @param _pose pose that should be scaled. if this is the refPose, all animations
 *              of the skeleton are scaled as well so that refPose and the animations
 *              have the same size. scaling other poses should only be used for intermediate
 *              poses. if _pose is NULL the refPose is used.
 *
 */
void SkeletonTransform::scaleSkeleton(double _factor, Skeleton::Pose* _pose) {

  // note: scaling the root joints is important since this controls e.g. the size of foot steps
  
  if ( _pose == 0)
    _pose = refPose_;

  //scale bones in the given pose
  for (Skeleton::Iterator it = skeleton_.begin(); it != skeleton_.end(); ++it )
      _pose->setLocalTranslation( (*it)->id(), _pose->localTranslation((*it)->id()) * _factor );

  // if given pose is refPose
  if ( _pose == refPose_){
    //scale bones in the animations
    for (size_t a=0; a < skeleton_.animationCount(); a++)
      for (unsigned int iFrame = 0; iFrame < skeleton_.animation(a)->frameCount(); iFrame++){

          Skeleton::Pose* pose = skeleton_.animation(a)->pose( iFrame );

          if ( pose == refPose_ )
            continue;

          for (Skeleton::Iterator it = skeleton_.begin(); it != skeleton_.end(); ++it )
              pose->setLocalTranslation( (*it)->id(), pose->localTranslation((*it)->id()) * _factor );
      }
  }
}

//-----------------------------------------------------------------------------

/** \brief translate the skeleton
 *
 * translates the skeleton in the reference pose and all animations simultanously
 *
 * @param _translation the translation vector
 * @param _pose pose that should be translated. if this is the refPose, all animations
 *              of the skeleton are scaled as well so that refPose and the animations
 *              have the same size. scaling other poses should only be used for intermediate
 *              poses. if _pose is NULL the refPose is used.
 *
 */
void SkeletonTransform::translateSkeleton(ACG::Vec3d _translation, Skeleton::Pose* _pose) {

  if ( _pose == 0)
    _pose = refPose_;
  
  //apply the transformation to given pose
  ACG::Vec3d position = _pose->globalTranslation( 0 );
  _pose->setGlobalTranslation( 0, position + _translation );
  
  // apply to all animations if the refpose was changed
  if ( _pose == refPose_ ){

    // apply transformation to all frames of all animations
    for (unsigned int a=0; a < skeleton_.animationCount(); a++)
    for (unsigned int iFrame = 0; iFrame < skeleton_.animation(a)->frameCount(); iFrame++){

      Skeleton::Pose* pose = skeleton_.animation(a)->pose( iFrame );

      if ( pose == refPose_ )
        continue;

      ACG::Vec3d position = pose->globalTranslation( 0 );
      pose->setGlobalTranslation( 0, position + _translation );
    }
  }
}

//-----------------------------------------------------------------------------

/** \brief transform the skeleton
 *
 * transform the skeleton in the reference pose and all animations simultanously
 *
 * @param _transformation The transformation to apply
 * @param _pose           The pose on which the transformation should be applied
 *
 */
void SkeletonTransform::transformSkeleton(Matrix4x4 _transformation, Skeleton::Pose* _pose) {

  if ( _pose == 0)
    _pose = refPose_;
  
  //apply the transformation to given pose
  ACG::Matrix4x4d local = _pose->localMatrix( 0 );
  _pose->setLocalMatrix( 0, _transformation * local );
  
  // apply to all animations if the refpose was changed
  if ( _pose == refPose_ ){

    // apply transformation to all frames of all animations
    for (unsigned int a=0; a < skeleton_.animationCount(); a++)
    for (unsigned int iFrame = 0; iFrame < skeleton_.animation(a)->frameCount(); iFrame++){

      Skeleton::Pose* pose = skeleton_.animation(a)->pose( iFrame );

      if ( pose == refPose_ )
        continue;

      ACG::Matrix4x4d local = pose->localMatrix( 0 );
      pose->setLocalMatrix( 0, _transformation * local );
    }
  }
}

//-----------------------------------------------------------------------------

/** \brief apply a transformation to a joint in the refPose
 *
 * apply a transformation to a joint in the refPose. this also applies suitable
 * transformations to all animation-frames.
 *
 * it's only possible to apply transformations to the refPose. applying transformations
 * to animation poses does not make much sense. Use rotateJoint to change joint positions
 * during the animation.
 *
 * @param _joint              Target joint
 * @param _matrix             The transformation to apply
 * @param _keepChildPositions Keep the global positions of child joints
 *
 */
void SkeletonTransform::transformJoint(Skeleton::Joint* _joint, Matrix4x4 _matrix, bool _keepChildPositions) {

  if (_joint == 0)
    return;
  
  Matrix4x4 transform;
  transform.identity();
  
  //get transformation matrix
  Skeleton::Joint* parent = _joint->parent();
  
  if ( parent != 0 )
    // new GlobalMatrix after application of _matrix is:  newGlobal = _matrix * activePose->getGlobal( joint )
    // new LocalMatrix : activePose->getGlobalInv( parent->getID() ) * newGlobal
    // Transformation from LocalMatrix to new LocalMatrix:
    transform = refPose_->globalMatrixInv( parent->id() ) * _matrix * refPose_->globalMatrix( _joint->id() ) * refPose_->localMatrixInv( _joint->id() );
  else
    // the transformation for the root joint has to be applied directly
    // _matrix defines a post-multiplication but we need a pre-multiplication matrix X in order to apply
    // the transformation to all frames: _matrix * Global = Global * X --> X = GlobalInverse * _matrix * global
    transform = refPose_->globalMatrixInv( _joint->id() ) * _matrix * refPose_->globalMatrix( _joint->id() );
  
  
  //transform refPose
  if ( parent != 0 ){

    Matrix4x4 newMatrix = transform * refPose_->localMatrix( _joint->id() );
    refPose_->setLocalMatrix( _joint->id(), newMatrix, !_keepChildPositions); //keep child for local means !keepChild global

  } else {
    //directly apply the global transformation
    Matrix4x4 newMatrix = refPose_->globalMatrix( _joint->id() ) * transform;
    refPose_->setLocalMatrix( _joint->id(), newMatrix, !_keepChildPositions);
  }

  // apply transformation to all frames of all animations
  for (unsigned int a=0; a < skeleton_.animationCount(); a++)
  for (unsigned int iFrame = 0; iFrame < skeleton_.animation(a)->frameCount(); iFrame++){
    
    Skeleton::Pose* pose = skeleton_.animation(a)->pose( iFrame );
  
    if ( parent != 0 ){
      
      //transform the local matrix
      Matrix4x4 newMatrix = transform * pose->localMatrix( _joint->id() );
      pose->setLocalMatrix( _joint->id(), newMatrix, !_keepChildPositions);

      // the transformation may have changed two things
      // 1. due to the changed length of neighboring bones the rotation
      //    angle changes this has to be corrected by rotating the coord-frame
      // 2. the length of the bone to a child may have changed. Check that
      //    the length is the same like in the refPose

    } else {
      
      //directly apply the global transformation
      Matrix4x4 newMatrix = pose->globalMatrix( _joint->id() ) * transform;
        
      pose->setLocalMatrix( _joint->id(), newMatrix, !_keepChildPositions);
    }
  }
}

//-----------------------------------------------------------------------------

/** \brief apply a translateion to a joint in the refPose
 *
 * apply a translation to a joint in the refPose. this also applies suitable
 * transformations to all animation-frames.
 *
 * it's only possible to apply translations to the refPose. applying translations
 * to animation poses does not make much sense. Use rotateJoint to change joint positions
 * during the animation.
 *
 * @param _joint              Target joint
 * @param _translation        The translation to apply
 * @param _keepChildPositions Keep the global positions of child joints
 *
 */
void SkeletonTransform::translateJoint(Skeleton::Joint* _joint, ACG::Vec3d _translation, bool _keepChildPositions) {
  ACG::GLMatrixd transformation;
  transformation.identity();
  transformation.translate( _translation );

  transformJoint(_joint, transformation, _keepChildPositions);
}

//-----------------------------------------------------------------------------

/** \brief Rotate a joint in an arbitrary Pose
 *
 * Rotate a joint in the given pose and with the given rotation matrix.
 *
 * @param _joint                 Target joint
 * @param _pose                  Target pose
 * @param _rotation              The rotation matrix that should be applied
 * @param _applyToWholeAnimation Should the relative rotation be applied to all frames of the animation?
 *
 */
void SkeletonTransform::rotateJoint(Skeleton::Joint* _joint, Skeleton::Pose* _pose, Matrix4x4 _rotation, bool _applyToWholeAnimation) {

  if (_joint == 0)
    return;
  
  if ( fabs(1.0 - determinant(_rotation)) > 1E-6 ){
    std::cerr << "Cannot rotate joint. Matrix is not a rotation matrix (det:" << determinant(_rotation) << ")" << std::endl;
    return;
  }

  Matrix4x4 transform;
  transform.identity();
  
  //get transformation matrix
  Skeleton::Joint* parent = _joint->parent();
  
  if ( parent != 0 )
    // new GlobalMatrix after application of _matrix is:  newGlobal = _matrix * activePose->getGlobal( joint )
    // new LocalMatrix : activePose->getGlobalInv( parent->getID() ) * newGlobal
    // Transformation from LocalMatrix to new LocalMatrix:
    transform = _pose->globalMatrixInv( parent->id() ) * _rotation * _pose->globalMatrix( _joint->id() ) * _pose->localMatrixInv( _joint->id() );
  else
    // the transformation for the root joint has to be applied directly
    // _matrix defines a post-multiplication but we need a pre-multiplication matrix X in order to apply
    // the transformation to all frames: _matrix * Global = Global * X --> X = GlobalInverse * _matrix * global
    transform = _pose->globalMatrixInv( _joint->id() ) * _rotation * _pose->globalMatrix( _joint->id() );


  //transform pose
  if ( (_pose == refPose_) || !_applyToWholeAnimation ){

    if ( parent != 0 ){

      Matrix4x4 newMatrix = transform * _pose->localMatrix( _joint->id() );
      _pose->setLocalMatrix( _joint->id(), newMatrix);

    } else {

      //directly apply the global transformation
      Matrix4x4 newMatrix = _pose->globalMatrix( _joint->id() ) * transform;
      _pose->setLocalMatrix( _joint->id(), newMatrix);
    }
  }

  //TODO don't apply to all frames but only to those belonging to the same animation as _pose
  if ( _applyToWholeAnimation ){
    // apply transformation to all frames of all animations
    for (unsigned int a=0; a < skeleton_.animationCount(); a++)
    for (unsigned int iFrame = 0; iFrame < skeleton_.animation(a)->frameCount(); iFrame++){
      
      Skeleton::Pose* pose = skeleton_.animation(a)->pose( iFrame );
    
      if ( parent != 0 ){
        //transform the local matrix
        Matrix4x4 newMatrix = transform * pose->localMatrix( _joint->id() );
          
        pose->setLocalMatrix( _joint->id(), newMatrix);
        
      } else {
        
        //directly apply the global transformation
        Matrix4x4 newMatrix = pose->globalMatrix( _joint->id() ) * transform;
          
        pose->setLocalMatrix( _joint->id(), newMatrix);
      }
    }
  }
}

//-----------------------------------------------------------------------------


double SkeletonTransform::determinant(Matrix4x4& _m) {
  double value;

  value =
  _m(0,3) * _m(1,2) * _m(2,1) * _m(3,0)-_m(0,2) * _m(1,3) * _m(2,1) * _m(3,0)-_m(0,3) * _m(1,1) * _m(2,2) * _m(3,0)+_m(0,1) * _m(1,3) * _m(2,2) * _m(3,0)+
  _m(0,2) * _m(1,1) * _m(2,3) * _m(3,0)-_m(0,1) * _m(1,2) * _m(2,3) * _m(3,0)-_m(0,3) * _m(1,2) * _m(2,0) * _m(3,1)+_m(0,2) * _m(1,3) * _m(2,0) * _m(3,1)+
  _m(0,3) * _m(1,0) * _m(2,2) * _m(3,1)-_m(0,0) * _m(1,3) * _m(2,2) * _m(3,1)-_m(0,2) * _m(1,0) * _m(2,3) * _m(3,1)+_m(0,0) * _m(1,2) * _m(2,3) * _m(3,1)+
  _m(0,3) * _m(1,1) * _m(2,0) * _m(3,2)-_m(0,1) * _m(1,3) * _m(2,0) * _m(3,2)-_m(0,3) * _m(1,0) * _m(2,1) * _m(3,2)+_m(0,0) * _m(1,3) * _m(2,1) * _m(3,2)+
  _m(0,1) * _m(1,0) * _m(2,3) * _m(3,2)-_m(0,0) * _m(1,1) * _m(2,3) * _m(3,2)-_m(0,2) * _m(1,1) * _m(2,0) * _m(3,3)+_m(0,1) * _m(1,2) * _m(2,0) * _m(3,3)+
  _m(0,2) * _m(1,0) * _m(2,1) * _m(3,3)-_m(0,0) * _m(1,2) * _m(2,1) * _m(3,3)-_m(0,1) * _m(1,0) * _m(2,2) * _m(3,3)+_m(0,0) * _m(1,1) * _m(2,2) * _m(3,3);

  return value;
}