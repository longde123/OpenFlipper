#ifndef SUBDIVIDERPLUGIN_HH
#define SUBDIVIDERPLUGIN_HH

/** Simple Subdivider Plugin based on Subdivider code from Martin Habbeke
 */
#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "subdividerToolbar.hh"

// Subdivider types
//typename OpenMesh::Subdivider::Uniform:LoopT<TriMesh>  LoopT;
//typename OpenMesh::Subdivider::Uniform:Sqrt3T<TriMesh> Sqrt3T;

class SubdividerPlugin : public QObject, BaseInterface , ToolboxInterface, LoggingInterface, PickingInterface, MouseInterface, LoadSaveInterface, KeyInterface, ScriptInterface, BackupInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(BackupInterface)

signals:
  void updateView();
  void updatedObject(int _id, const UpdateType _type);

  void addPickMode( const std::string _mode );

  void log(Logtype _type, QString _message);
  void log(QString _message);

  void addEmptyObject( DataType _type, int& _id );

  void defineViewMode(QString _mode, QStringList _usedWidgets);

  void scriptInfo( QString _functionName  );

  void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget );   
  
  // BackupInterface
  void createBackup( int _id , QString _name );

private slots:

  void slotMouseEvent( QMouseEvent* _event );

  void slotMouseWheelEvent(QWheelEvent * /*_event*/, const std::string & /*_mode*/){};

  void slotPickModeChanged( const std::string& _mode);

  void slotKeyEvent( QKeyEvent* _event );

  void initializePlugin();
  
  void pluginsInitialized();
  
  /// Make plugin available in no-GUI mode
  void noguiSupported( ) {} ;

public slots:

  void slot_subdivide();

public :
  ~SubdividerPlugin() {};

  QString name() { return (QString("Subdivider")); };
  QString description( ) { return (QString("Subdivide a mesh")); };

private :
  /// Widget for Toolbox
  subdividerToolbarWidget* tool_;

private:

  bool test_trimesh_object(int _identifier, BaseObjectData*& _object);


public slots:
   QString version() { return QString("1.0"); };
};

#endif //SUBDIVIDERPLUGIN_HH
