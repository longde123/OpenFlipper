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




#ifndef TYPESKELETONPLUGIN_HH
#define TYPESKELETONPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>

class TypeSkeletonPlugin : public QObject, BaseInterface, LoadSaveInterface, LoggingInterface, TypeInterface, ContextMenuInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(TypeInterface)
   Q_INTERFACES(ContextMenuInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-TypeSkeleton")
#endif

  signals:
    // Logging interface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // LoadSave Interface
    void emptyObjectAdded( int _id );

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);
    void addContextMenuItem(QAction* _action , DataType _objectType , ContextMenuType _type );
    
  private slots:
    void pluginsInitialized();
    void noguiSupported( ) {} ;
    
    void slotUpdateContextMenu( int _objectId );
    
    void slotShowIndices();
    void slotShowCoordFrames();
    void slotShowMotionPath();

  private:
    QAction* showIndicesAction_;
    QAction* showCoordFramesAction_;
    QAction* showMotionAction_;
    
  public :

     ~TypeSkeletonPlugin() {};
     TypeSkeletonPlugin();

     QString name() { return (QString("TypeSkeleton")); };
     QString description( ) { return (QString(tr("Register Skeleton type"))); };
     
     bool registerType();

  public slots:

    // Base Interface
    QString version() { return QString("1.0"); };
    
    // Type Interface
    int addEmpty();
    DataType supportedType() { return DATA_SKELETON; }; 
    
    // Type Interface
    void generateBackup( int _id, QString _name, UpdateType _type );
    
};

#endif //TYPESKELETONPLUGIN_HH