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
#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenMesh/Core/IO/IOManager.hh>

#if QT_VERSION >= 0x050000
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "FileOM.hh"


// Defines for the type handling drop down box
#define TYPEAUTODETECT 0
#define TYPEASK        1
#define TYPEPOLY       2
#define TYPETRIANGLE   3

/// Constructor
FileOMPlugin::FileOMPlugin() :
        loadOptions_(0),
        saveOptions_(0),
        saveBinary_(0),
        saveVertexNormal_(0),
        saveVertexTexCoord_(0),
        saveVertexColor_(0),
        saveFaceColor_(0),
        saveFaceNormal_(0),
        saveDefaultButton_(0),
        triMeshHandling_(0),
        loadVertexNormal_(0),
        loadVertexTexCoord_(0),
        loadVertexColor_(0),
        loadFaceColor_(0),
        loadFaceNormal_(0),
        loadDefaultButton_(0)

{
}

//-----------------------------------------------------------------------------------------------------

void FileOMPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FileOMPlugin::getLoadFilters() {
    return QString( tr("OpenMesh Format files ( *.om )") );
};

//-----------------------------------------------------------------------------------------------------

QString FileOMPlugin::getSaveFilters() {
    return QString( tr("OpenMesh Format files ( *.om )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FileOMPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

int FileOMPlugin::loadObject(QString _filename) {

    int triMeshControl = TYPEAUTODETECT; // 0 == Auto-Detect
    
    if ( OpenFlipper::Options::gui() ){
        if ( triMeshHandling_ != 0 ){
          triMeshControl = triMeshHandling_->currentIndex();
        } else {
          triMeshControl = TYPEAUTODETECT;
        }
    }
    
    int objectId = -1;
    
    if(triMeshControl == TYPEAUTODETECT) {
        // If Auto-Detect is selected (triMeshControl == 0)
        objectId = loadPolyMeshObject(_filename);
        
        PolyMeshObject *object = 0;
        if(!PluginFunctions::getObject(objectId, object))
            return -1;
        
        for ( PolyMesh::FaceIter f_it = object->mesh()->faces_begin(); f_it != object->mesh()->faces_end() ; ++f_it) {
            
            // Count number of vertices for the current face
            uint count = 0;
            for ( PolyMesh::FaceVertexIter fv_it( *(object->mesh()),*f_it); fv_it.is_valid(); ++fv_it )
                ++count;
            
            // Check if it is a triangle. If not, this is really a poly mesh
            if ( count != 3 ) {
                
                PolyMeshObject* object(0);
                if(PluginFunctions::getObject( objectId, object )) {
                    
                    emit updatedObject(objectId, UPDATE_ALL);
                    emit openedFile( objectId );
                }
                
                return objectId;
            }
        }
        
    } else if (triMeshControl == TYPEASK ) {
      
        // If Ask is selected -> show dialog
        objectId = loadPolyMeshObject(_filename);
        
        bool triMesh = true;
        
        PolyMeshObject *object = 0;
        if(!PluginFunctions::getObject(objectId, object))
            return -1;
        
        for ( PolyMesh::FaceIter f_it = object->mesh()->faces_begin(); f_it != object->mesh()->faces_end() ; ++f_it) {
            
            // Count number of vertices for the current face
            uint count = 0;
            for ( PolyMesh::FaceVertexIter fv_it( *(object->mesh()),*f_it); fv_it.is_valid(); ++fv_it )
                ++count;
            
            // Check if it is a triangle. If not, this is really a poly mesh
            if ( count != 3 ) {
                triMesh = false;
                break;
            }
            
            if(triMesh == false) break;
        }
        
        // Note: If in non-gui mode, we will never enter this case branch

        QMetaObject::invokeMethod(this,"handleTrimeshDialog",Qt::BlockingQueuedConnection);
        if ((trimeshOptions == TYPEPOLY) ||
            (trimeshOptions == TYPEASK && !triMesh)) {
            
            PolyMeshObject* object(0);
            if(PluginFunctions::getObject( objectId, object )) {
            
                emit updatedObject(objectId, UPDATE_ALL);
                emit openedFile( objectId );
            }
            return objectId;
        }
                                                                     
    } else if (triMeshControl == TYPEPOLY) {
        // If always open as PolyMesh is selected
        
        objectId = loadPolyMeshObject(_filename);
        
        PolyMeshObject* object(0);
        if(PluginFunctions::getObject( objectId, object )) {
            
            emit updatedObject(objectId, UPDATE_ALL);
            emit openedFile( objectId );
        }
        
        return objectId;
    } else {
        // If always open as TriMesh is selected
        
        objectId = loadTriMeshObject(_filename);
        
        TriMeshObject* object(0);
        if(PluginFunctions::getObject( objectId, object )) {
            emit updatedObject(objectId, UPDATE_ALL);
            emit openedFile( objectId );
        }
        
        return objectId;
    }
    
    // Load object as triangle mesh
    if(objectId != -1) emit deleteObject(objectId);
    
    objectId = loadTriMeshObject(_filename);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( objectId, object )) {
        
        emit updatedObject(objectId, UPDATE_ALL);
        emit openedFile( objectId );
    }
    
    return objectId;
};

void FileOMPlugin::handleTrimeshDialog()
{
   QMessageBox msgBox;
   QPushButton *detectButton = msgBox.addButton(tr("Auto-Detect"), QMessageBox::ActionRole);
   QPushButton *triButton    = msgBox.addButton(tr("Open as triangle mesh"), QMessageBox::ActionRole);
   QPushButton *polyButton   = msgBox.addButton(tr("Open as poly mesh"), QMessageBox::ActionRole);
   msgBox.setWindowTitle( tr("Mesh types in file") );
   msgBox.setText( tr("You are about to open a file containing one or more mesh types. \n\n Which mesh type should be used?") );
   msgBox.setDefaultButton( detectButton );
   msgBox.exec();


   if (msgBox.clickedButton() == triButton)
    trimeshOptions =  TYPETRIANGLE ;
   else if (msgBox.clickedButton() == polyButton)
    trimeshOptions = TYPEPOLY ;
   else
     trimeshOptions = TYPEASK;
}

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FileOMPlugin::loadTriMeshObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
        if ( PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        object->setName(object->filename());
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() &&
            !OpenFlipper::Options::loadingRecentFile() && loadOptions_ != 0){
            
            if (loadVertexNormal_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadVertexTexCoord_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            if (loadFaceNormal_->isChecked())
                opt += OpenMesh::IO::Options::FaceNormal;
            
        } else {
            
            // Let OpenMesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            opt += OpenMesh::IO::Options::FaceNormal;
            
        }
        
        /// \todo only request if needed
        object->mesh()->request_vertex_texcoords2D();
        object->mesh()->request_halfedge_texcoords2D();
        object->mesh()->request_face_texture_index();
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOM : Read error for Triangle Mesh\n";
            emit deleteObject( object->id() );
            return -1;
        }
        
        object->mesh()->update_normals();
        
        backupTextureCoordinates(*(object->mesh()));

        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new triangle mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FileOMPlugin::loadPolyMeshObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
        if (PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        object->setName(object->filename());
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::sceneGraphUpdatesBlocked() &&
            !OpenFlipper::Options::loadingRecentFile() && loadOptions_ != 0){
            
            if (loadVertexNormal_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadVertexTexCoord_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            if (loadFaceNormal_->isChecked())
                opt += OpenMesh::IO::Options::FaceNormal;
            
        } else {
            
            // Let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            opt += OpenMesh::IO::Options::FaceNormal;
            
        }
        
        /// \todo only request if needed
        object->mesh()->request_vertex_texcoords2D();
        object->mesh()->request_halfedge_texcoords2D();
        object->mesh()->request_face_texture_index();
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOM : Read error for Poly Mesh\n";
            emit deleteObject( object->id() );
            return -1;
            
        }
        

        if ( ! (opt &  OpenMesh::IO::Options::VertexTexCoord) ) {
          object->mesh()->release_vertex_texcoords2D();
        }


        if ( ! (opt &  OpenMesh::IO::Options::FaceTexCoord) ) {
          object->mesh()->release_halfedge_texcoords2D();
        }


        object->mesh()->update_normals();
        
        backupTextureCoordinates(*(object->mesh()));

        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

template <class MeshT>
void FileOMPlugin::backupTextureCoordinates(MeshT& _mesh) {

    // Create a backup of the original per Vertex texture Coordinates
    if (_mesh.has_vertex_texcoords2D()) {

      OpenMesh::VPropHandleT< typename MeshT::TexCoord2D > oldVertexCoords;
      if (!_mesh.get_property_handle(oldVertexCoords, "Original Per Vertex Texture Coords"))
        _mesh.add_property(oldVertexCoords, "Original Per Vertex Texture Coords");

      for (typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it)
        _mesh.property(oldVertexCoords, *v_it) =  _mesh.texcoord2D(*v_it);

    }

    // Create a backup of the original per Face texture Coordinates
    if (_mesh.has_halfedge_texcoords2D()) {

      OpenMesh::HPropHandleT< typename MeshT::TexCoord2D > oldHalfedgeCoords;
      if (!_mesh.get_property_handle(oldHalfedgeCoords,"Original Per Face Texture Coords"))
        _mesh.add_property(oldHalfedgeCoords,"Original Per Face Texture Coords");

      for (typename MeshT::HalfedgeIter he_it = _mesh.halfedges_begin(); he_it != _mesh.halfedges_end(); ++he_it)
        _mesh.property(oldHalfedgeCoords, *he_it) =  _mesh.texcoord2D(*he_it);

    }
}

//-----------------------------------------------------------------------------------------------------

bool FileOMPlugin::saveObject(int _id, QString _filename)
{
    BaseObjectData* object;
    if ( !PluginFunctions::getObject(_id,object) ) {
      emit log(LOGERR, tr("saveObject : cannot get object id %1 for save name %2").arg(_id).arg(_filename) );
      return false;
    }
    
    std::string filename = std::string( _filename.toUtf8() );
    
    if ( object->dataType( DATA_POLY_MESH ) ) {

      PolyMeshObject* polObj = dynamic_cast<PolyMeshObject* >( object );

      OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;

      if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){

        PolyMesh* mesh = polObj->mesh();

          if (saveBinary_->isChecked())
              opt += OpenMesh::IO::Options::Binary;

          if (saveVertexNormal_->isChecked() && mesh->has_vertex_normals())
              opt += OpenMesh::IO::Options::VertexNormal;

          if (saveVertexTexCoord_->isChecked() && (mesh->has_vertex_texcoords1D() || mesh->has_vertex_texcoords2D() || mesh->has_vertex_texcoords3D())) {
              std::cerr << "File OM texture write" << std::endl;
              opt += OpenMesh::IO::Options::VertexTexCoord;
          }

          if (saveVertexColor_->isChecked() && mesh->has_vertex_colors())
              opt += OpenMesh::IO::Options::VertexColor;

          if (saveFaceColor_->isChecked() && mesh->has_face_colors())
              opt += OpenMesh::IO::Options::FaceColor;

          if (saveFaceNormal_->isChecked() && mesh->has_face_normals())
              opt += OpenMesh::IO::Options::FaceNormal;

      }
        
        object->setFromFileName(_filename);
        object->setName(object->filename());
        
        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );
        
        if (OpenMesh::IO::write_mesh(*polyObj->mesh(), filename.c_str(),opt) ){
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            return true;
        }else{
            emit log(LOGERR, tr("Unable to save ") + _filename);
            return false;
        }
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        object->setFromFileName(_filename);
        object->setName(object->filename());
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );
        
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
            
          TriMesh* mesh = triObj->mesh();

            if (saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;
            
            if (saveVertexNormal_->isChecked() && mesh->has_vertex_normals())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (saveVertexTexCoord_->isChecked() && (mesh->has_vertex_texcoords1D() || mesh->has_vertex_texcoords2D() || mesh->has_vertex_texcoords3D())) {
                std::cerr << "File OM texture write" << std::endl;
                opt += OpenMesh::IO::Options::VertexTexCoord;
            }
            
            if (saveVertexColor_->isChecked() && mesh->has_vertex_colors())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (saveFaceColor_->isChecked() && mesh->has_face_colors())
                opt += OpenMesh::IO::Options::FaceColor;
            
            if (saveFaceNormal_->isChecked() && mesh->has_face_normals())
                opt += OpenMesh::IO::Options::FaceNormal;
            
        }
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(),opt) ) {
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + _filename );
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOMPlugin::saveOptionsWidget(QString /*_currentFilter*/) {
    
    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        saveBinary_ = new QCheckBox("Save Binary");
        layout->addWidget(saveBinary_);
        
        saveVertexNormal_ = new QCheckBox("Save Vertex Normals");
        layout->addWidget(saveVertexNormal_);
        
        saveVertexTexCoord_ = new QCheckBox("Save Vertex TexCoords");
        layout->addWidget(saveVertexTexCoord_);
        
        saveVertexColor_ = new QCheckBox("Save Vertex Colors");
        layout->addWidget(saveVertexColor_);
        
        saveFaceColor_ = new QCheckBox("Save Face Colors");
        layout->addWidget(saveFaceColor_);
        
        saveFaceNormal_ = new QCheckBox("Save Face Normals");
        layout->addWidget(saveFaceNormal_);
      
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);       
        
        saveOptions_->setLayout(layout);
        
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));
        
        saveBinary_->setChecked( OpenFlipperSettings().value("FileOM/Save/Binary",true).toBool() );
        saveVertexNormal_->setChecked( OpenFlipperSettings().value("FileOM/Save/Normals",true).toBool() );
        saveVertexTexCoord_->setChecked( OpenFlipperSettings().value("FileOM/Save/TexCoords",true).toBool() );
        saveVertexColor_->setChecked( OpenFlipperSettings().value("FileOM/Save/VertexColor",true).toBool() );
        saveFaceColor_->setChecked( OpenFlipperSettings().value("FileOM/Save/FaceColor",true).toBool() );
        saveFaceNormal_->setChecked( OpenFlipperSettings().value("FileOM/Save/FaceNormal",true).toBool() );

    } 
    
    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOMPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
    
    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        QLabel* label = new QLabel(tr("If PolyMesh is a Triangle Mesh:"));
        
        layout->addWidget(label);
                
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Auto-Detect") );
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Always open as PolyMesh") );
        triMeshHandling_->addItem( tr("Always open as TriangleMesh") );
        
        layout->addWidget(triMeshHandling_);
        
        loadVertexNormal_ = new QCheckBox("Load Vertex Normals");
        layout->addWidget(loadVertexNormal_);
        
        loadVertexTexCoord_ = new QCheckBox("Load Vertex TexCoords");
        layout->addWidget(loadVertexTexCoord_);
                
        loadVertexColor_ = new QCheckBox("Load Vertex Colors");
        layout->addWidget(loadVertexColor_);
        
        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);
        
        loadFaceNormal_ = new QCheckBox("Load Face Normals");
        layout->addWidget(loadFaceNormal_);

        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FileOM/Load/TriMeshHandling",TYPEAUTODETECT).toInt() );
        
        loadVertexNormal_->setChecked( OpenFlipperSettings().value("FileOM/Load/Normals",true).toBool()  );
        loadVertexTexCoord_->setChecked( OpenFlipperSettings().value("FileOM/Load/TexCoords",true).toBool()  );
        loadVertexColor_->setChecked( OpenFlipperSettings().value("FileOM/Load/VertexColor",true).toBool() );
        loadFaceColor_->setChecked( OpenFlipperSettings().value("FileOM/Load/FaceColor",true).toBool()  );
        loadFaceColor_->setChecked( OpenFlipperSettings().value("FileOM/Load/FaceNormal",true).toBool()  );
        
    }
    
    return loadOptions_;
}

void FileOMPlugin::slotLoadDefault() {
    
    OpenFlipperSettings().setValue( "FileOM/Load/Normals",     loadVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Load/TexCoords",   loadVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Load/VertexColor", loadVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Load/FaceColor",   loadFaceColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Load/FaceNormal",   loadFaceNormal_->isChecked()  );

    OpenFlipperSettings().setValue( "FileOM/Load/TriMeshHandling", triMeshHandling_->currentIndex() );
    
    OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}


void FileOMPlugin::slotSaveDefault() {
    
    OpenFlipperSettings().setValue( "FileOM/Save/Binary",      saveBinary_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/Normals",     saveVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/TexCoords",   saveVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/VertexColor", saveVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/FaceColor",   saveFaceColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/FaceNormal",   saveFaceNormal_->isChecked()  );
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( fileomplugin , FileOMPlugin );
#endif

