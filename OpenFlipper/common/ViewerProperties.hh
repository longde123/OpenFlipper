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
//  Property storage Class for glWidgets
//
//=============================================================================

/**
 * \file ViewerProperties.hh
 * This file contains a class for managing all viewer settings
 */

//
#ifndef VIEWERPROPERTIES_HH
#define VIEWERPROPERTIES_HH


#include <QObject>
#include <QColor>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <ACG/Math/VectorT.hh>
#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/GLState.hh>
#include <OpenFlipper/widgets/glWidget/CursorPainter.hh>

// forward declaration
class ViewObjectMarker;

namespace Viewer {

  /** \brief Enum listing action modes of the viewers.
   *
   * These enums are used to control the reaction of the MouseInterface and the viewer on mouse events.
   */
  enum ActionMode {
    /** \brief Examine or Move mode
     *
     *  In this mode no events are passed to the MouseInterface.
     *  Only the viewer is active with mouse navigation.
     */
    ExamineMode,

    /** \brief Picking Mode
     *
     * In this mode the MouseInterface::slotMouseEvent() function gets called.
     *
     * The picking itself has to be performed by the plugins e.g. when implementing
     * \ref mouseInterfacePage
     *
     */
    PickingMode,

    /** \brief Question Mode
     *
     * In this mode the MouseInterface::slotMouseEventIdentify() function gets called
     *
     */
    QuestionMode,

    /** \brief Light mode
     *
     * In this mode the MouseInterface::slotMouseEventLight() function is called
     */
    LightMode
  };

  class DLLEXPORT ViewerProperties : public QObject {

    Q_OBJECT

    public:
      ViewerProperties(int _id);
      ~ViewerProperties();
      
    //===========================================================================
    /** @name Draw Mode States
     * @{ */
    //===========================================================================      

    public:
      /// set draw mode (No test if this mode is available!)
      void drawMode(ACG::SceneGraph::DrawModes::DrawMode _mode);
      
      /// get current draw mode
      ACG::SceneGraph::DrawModes::DrawMode drawMode();
      
    private:
      
      ACG::SceneGraph::DrawModes::DrawMode currentDrawMode_;

    /** @} */
      
    //===========================================================================
    /** @name Snapshot settings
    * @{ */
    //===========================================================================

    public slots:
      /** Set the base file name and reset the counter for snapshots.

        The current snapshot counter will be added in front of the last "."
        in the filename.

        \a Note: Calling snapshotBaseFileName() will always reset the snapshot
        counter to 0.
      */
      void snapshotBaseFileName(const QString& _fname);

      /** Set the file type for snapshots. defaults to png" */
      void snapshotFileType(const QString& _type);
      
      /** Set the start index for the snapshot counter */
      void snapshotCounter(const int _counter);
      
      /** Get the file type for the current snapshot */
      QString snapshotFileType();
      
      /** Get the name for the current snapshot */
      QString snapshotName();

      /** Get the counter for the current snapshot and increases the counter */
      int snapshotCounter();

    private:
      QString                      snapshotName_;
      QString                      snapshotFileType_;
      int                          snapshotCounter_;

    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name Mouse Wheel Settings
    * @{ */
    //===========================================================================
    //===========================================================================


    public slots:
      /// Zoom factor when using mouse wheel
      double wheelZoomFactor();

      /// Zoom factor when using mouse wheel and pressing shift
      double wheelZoomFactorShift();

      /// Set zoom factor when using mouse wheel
      void wheelZoomFactor(double _factor);

      /// Set zoom factor when using mouse wheel and pressing shift
      void wheelZoomFactorShift(double _factor);

      /// Invert mouse wheel direction?
      bool wheelInvert();

      /// Invert mouse wheel direction
      void wheelInvert(bool _invert);

    private:
      double wZoomFactor_;

      double wZoomFactorShift_;

      bool   wInvert_;

    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name View Control
    * @{ */
    //===========================================================================
    //===========================================================================

    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name Rendering Settings
    * @{ */
    //===========================================================================
    //===========================================================================

    public slots:
      /** true if counter clockwise orientation should be used to define front facing orientation.
       */
      bool isCCWFront();

      /// Set counter clockwise orientation as front
      void ccwFront();
      /// Set clockwise orientation as front
      void cwFront();

    private:
      bool CCWFront_;


    //===========================================================================

    public slots:

      /// Get current background color
      ACG::Vec4f backgroundColor();

      /// Get current background color
      QRgb backgroundColorRgb();

      /// Get current background color
      QColor backgroundQColor();

      /** Set background color.
      */
      void backgroundColor( ACG::Vec4f _color );

      /** Set background color.
      */
      void backgroundColor( QRgb _color );

      /** Set background color.
      */
      void backgroundColor( QColor _color );

    private:
      ACG::Vec4f backgroundColor_;


    //===========================================================================

    public slots:
      /** Lock update of display.
      If locked (isUpdateLocked()) then all calls to updateDisplayList()
      and updateGL() will have no effect! This is true until the display is
      unlockUpdate()'ed.
      */
      void lockUpdate();

      /// Unlock display locked by updateLock().
      void unLockUpdate();

      /** Are updateDisplayList() and updateGL() locked?
      (c.f. lockUpdate()) */
      bool updateLocked();

    private:
      int locked_;


    //===========================================================================

    public slots:
      /// Get current state of backface culling
      bool backFaceCulling();

      /// Enable or disable backface culling
      void backFaceCulling(bool _state );

    private:
      bool backFaceCulling_;

    //===========================================================================

    public slots:
      /// set 2-sided lighting on/off
      void twoSidedLighting(bool _state );

      /// is 2-sided lighing enabled?
      bool twoSidedLighting();

    private:
      bool twoSidedLighting_;
      
    //===========================================================================

    public slots:
      /// set multisampling on/off
      void multisampling(bool _state );

      /// is multisampling enabled?
      bool multisampling();

    private:
      bool multisampling_;
      
    //===========================================================================

    public slots:
      /// set mipmapping on/off
      void mipmapping(bool _state );

      /// is mipmapping enabled?
      bool mipmapping();

    private:
      bool mipmapping_;

    //===========================================================================

    public slots:
      /// set 2-sided lighting on/off
      void animation(bool _state );

      /// is 2-sided lighing enabled?
      bool animation();

    private:
      bool animation_;


    //===========================================================================

    public:
      /// Get the glState of the Viewer
      ACG::GLState& glState();
      const ACG::GLState& glState() const;

      void setglState(ACG::GLState* _glState);

    private:
      /// Pointer to the glState of the Viewer
      ACG::GLState* glState_;

    //===========================================================================

    public slots:
      /// set object marker for viewer
      void objectMarker (ViewObjectMarker* _marker);

      /// current object marker
      ViewObjectMarker* objectMarker();

    private:
      ViewObjectMarker *objectMarker_;

    //===========================================================================

    public:
      int currentViewingDirection();

      void currentViewingDirection(int _dir);

    private:
      int currentViewingDirection_;
      
    //===========================================================================

    public:
      bool rotationLocked();

      void rotationLocked(bool _lock);

    private:
      bool rotationLocked_;

    //===========================================================================

    /** @} */
    
    //===========================================================================
    //===========================================================================
    /** @name Projection controls and information
    * @{ */
    //===========================================================================
    //===========================================================================

    public:
      
      /// Get width of the gl scene in orthogonal projection mode (defaults to 2.0)
      double orthoWidth();
      
      /// Set the width of the gl scene in orthogonal projection mode
      void orthoWidth(double _width);
      
      
      /// Return distance to near Plane
      double nearPlane();
      
      /// Set near and far plane at the same time
      void setPlanes( double _near, double _far  );
      
      /// Return distance to far Plane
      double farPlane();
      
      /// Get current scene center (rendering center)
      ACG::Vec3d sceneCenter();
      
      /// Set current scene center (rendering center)
      void sceneCenter(ACG::Vec3d _center);

      
      /// Get radius of the current scene
      double sceneRadius();
      
      /// Set radius of the current scene
      void sceneRadius(double _radius );
      
      
      /// Get virtual trackball center (rotation center when using mouse)
      ACG::Vec3d trackballCenter();
      
      /// Set virtual trackball center (rotation center when using mouse)
      void trackballCenter(ACG::Vec3d _center);
      
      
      /// Get trackball radius (rotation sphere when using mouse)
      double trackballRadius();
      
      /// Set trackball radius   (rotation sphere when using mouse)
      void trackballRadius(double _radius );
      
    private:
      
      /// Width of the gl scene in orthogonal mode ( defaults to 2.0 )
      double orthoWidth_;
      
      /// Distance to near Plane
      double nearPlane_;
      
      ///distance to far Plane
      double farPlane_;
      
      /// Current scene center (rendering center)
      ACG::Vec3d sceneCenter_;
      
      /// Radius of the current scene
      double sceneRadius_;
      
      /// Virtual trackball center (rotation center when using mouse)
      ACG::Vec3d trackballCenter_;
      
      /// trackball radius   (rotation sphere when using mouse)
      double trackballRadius_;
      
    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name Stereo Settings
     * @{ */
    //===========================================================================
    //===========================================================================
    
    public:

      void stereo(bool _stereo);
      bool stereo();

    private:
      /// Flag if stereo should be enabled for the current viewer
      bool stereo_;


    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name Stereo Settings
     * @{ */
    //===========================================================================
    //===========================================================================

    public:
      CursorPainter* cursorPainter();
      void cursorPainter( CursorPainter* _painter );

      ACG::Vec3d cursorPoint3D();
      void cursorPoint3D(ACG::Vec3d _pos);

      bool cursorPositionValid();
      void cursorPositionValid(bool _valid);

    private:

      // Used to draw the current cursor in the scene
      CursorPainter *cursorPainter_;

      // position of the cursor picked into the scenegraph
      ACG::Vec3d cursorPoint3D_;

      // indicates that we successfully mapped the cursor into the scene
      bool cursorPositionValid_;

    /** @} */

    signals:

      /** \brief This signal is emitted when the configuration has been changed
       *
      */
      void updated();

      /** \brief This signal is emitted when the drawmode changed
       *
       */
      void drawModeChanged(int _viewerId );

      void getPickMode(std::string& _mode );
      void setPickMode(const std::string&  _mode );

      void getActionMode(Viewer::ActionMode& _am);
      void setActionMode(const Viewer::ActionMode  _am);

    public:

      /// get active pick mode
      std::string pickMode();

      /// set the pickMode
      void pickMode(const std::string& _name);

      /// get the action mode
      Viewer::ActionMode actionMode();

      /// set active action mode
      void actionMode(const Viewer::ActionMode _am);

    //===========================================================================
    //===========================================================================
    /** @name Viewer id handling
     * @{ */
    //===========================================================================
    //===========================================================================

      /// Get the id of the viewer this viewerproperties belongs to
      int viewerId();

      /// Set the id of the viewer this viewerproperties belongs to
      void viewerId(int _id);

    private:

      /// Stores the id of the viewer this property belongs to
      int viewerId_;

      /// String for the Settings storage
      QString settingsSection_;

    /** @} */

  };

}


#endif //VIEWERPROPERTIES_HH

