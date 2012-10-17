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

#ifndef DECIMATERPLUGIN_HH
#define DECIMATERPLUGIN_HH

//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QTimer>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "DecimaterInfo.hh"

#include "DecimaterToolbarWidget.hh"

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/McDecimaterT.hh>
#include <OpenMesh/Tools/Decimater/MixedDecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModHausdorffT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalDeviationT.hh>
#include <OpenMesh/Tools/Decimater/ModEdgeLengthT.hh>
#include <OpenMesh/Tools/Decimater/ModAspectRatioT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <ACG/Utils/SmartPointer.hh>
#include <vector>


//== CLASS DEFINITION =========================================================

/** Plugin for Decimater Support
 */
class DecimaterPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, ScriptInterface, BackupInterface, RPCInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(RPCInterface)
  Q_INTERFACES(LoadSaveInterface)

signals:

  // BaseInterface
  void updateView();
  void updatedObject(int _id, const UpdateType& _type);
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // RPC Interface
  void pluginExists( QString _pluginName , bool& _exists  ) ;
  void functionExists( QString _pluginName , QString _functionName , bool& _exists  );

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );

  // ScriptInterface
  void scriptInfo( QString _functionName );

  // BackupInterface
  void createBackup( int _id , QString _name, UpdateType _type = UPDATE_ALL );

private slots:
    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();

    void slotObjectUpdated( int _identifier , const UpdateType& _type ); // BaseInterface
    void slotObjectSelectionChanged( int _identifier ); // BaseInterface

    void objectDeleted(int _id); //LoadSaveInterface

    void slotAboutToRestore(int _id); //BackupInterface

public :

  /// Default constructor
  DecimaterPlugin();

  /// Default destructor
  ~DecimaterPlugin() {};

  /// Name of the Plugin
  QString name(){ return (QString("Decimater")); };

  /// Description of the Plugin
  QString description() { return (QString(tr("Mesh Decimation ..."))); };

private :

  typedef OpenMesh::Decimater::BaseDecimaterT< TriMesh >              BaseDecimaterType;
  typedef OpenMesh::Decimater::DecimaterT< TriMesh >                  DecimaterType;
  typedef OpenMesh::Decimater::McDecimaterT< TriMesh >                McDecimaterType;
  typedef OpenMesh::Decimater::MixedDecimaterT< TriMesh >             MixedDecimaterType;

  typedef OpenMesh::Decimater::ModAspectRatioT< TriMesh >::Handle     ModAspectRatioH;
  typedef OpenMesh::Decimater::ModEdgeLengthT< TriMesh >::Handle      ModEdgeLengthH;
  typedef OpenMesh::Decimater::ModHausdorffT< TriMesh >::Handle       ModHausdorffH;
  typedef OpenMesh::Decimater::ModIndependentSetsT< TriMesh >::Handle ModIndependentH;
  typedef OpenMesh::Decimater::ModNormalDeviationT< TriMesh >::Handle ModNormalDeviationH;
  typedef OpenMesh::Decimater::ModNormalFlippingT< TriMesh >::Handle  ModNormalFlippingH;
  typedef OpenMesh::Decimater::ModQuadricT< TriMesh >::Handle         ModQuadricH;
  typedef OpenMesh::Decimater::ModRoundnessT< TriMesh >::Handle       ModRoundnessH;

  /// Widget for Toolbox
  DecimaterToolbarWidget* tool_;

  //saves the decimater and the object id
  struct DecimaterInit
  {
    ptr::shared_ptr<BaseDecimaterType> decimater;
    int objId;
    ModAspectRatioH     hModAspectRatio;
    ModEdgeLengthH      hModEdgeLength;
    ModHausdorffH       hModHausdorff;
    ModIndependentH     hModIndependent;
    ModNormalDeviationH hModNormalDeviation;
    ModNormalFlippingH  hModNormalFlipping;
    ModQuadricH         hModQuadric;
    ModRoundnessH       hModRoundness;
  };
  std::vector< ptr::shared_ptr<DecimaterInit> > decimater_objects_;
  QIcon* toolIcon_;

private slots:

  /// decimating called from button in toolbox
  void slot_decimate();

  /// init called from button in toolbox
  void slot_initialize();

  /// roundness slider - spinbox sync
  void updateRoundness(int    _value);
  void updateRoundness(double _value);
  void updateAspectRatio(int _value);
  void updateAspectRatio(double _value);

  /// slider / spinbox updates
  void updateDistance ();
  void updateNormalDev ();
  void updateVertices ();
  void updateTriangles ();
  void updateEdgeLength ();

  /// update number of vertices information
  void slotUpdateNumVertices();
  void slotUpdateNumTriangles();

  void slotMixedCounterValueChanged(double);
  void slotMixedSliderValueChanged(int);
  void slotDisableDecimation();

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================

public slots:

  /** \brief decimate an object
   *
   * @param _objID       Id of the object that should be decimated
   * @param _constraints List of constraints for the decimater
   *
   */
  void decimate(int _objID, QVariantMap _constraints);

  /** @} */

public slots:
   QString version() { return QString("1.0"); };




};

#endif //DECIMATERPLUGIN_HH
