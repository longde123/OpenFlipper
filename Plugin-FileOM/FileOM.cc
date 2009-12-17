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
*   $Revision: 7936 $                                                       *
*   $Author: kremer $                                                      *
*   $Date: 2009-12-15 14:01:19 +0100 (Tue, 15 Dec 2009) $                   *
*                                                                           *
\*===========================================================================*/

#include <QtGui>
#include <QFileInfo>
#include <QSettings>
#include <QPushButton>

#include "FileOM.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

/// Constructor
FileOMPlugin::FileOMPlugin()
: loadOptions_(0),
  saveOptions_(0),
  triMeshHandling_(0) {
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

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
        if (PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
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
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOM : Read error for Poly Mesh\n";
            emit deleteObject( object->id() );
            return -1;
            
        } else {
            
            int triMeshControl = 1; // 1 == keep polyMesh .. do nothing
            
            if ( OpenFlipper::Options::gui() ){
                if ( triMeshHandling_ != 0 ){
                    triMeshControl = triMeshHandling_->currentIndex();
                } else
                    triMeshControl = 0;
            }

            //check if it's actually a triangle mesh
            if (triMeshControl != 1){ // 1 == do nothing
                
                PolyMesh& mesh = *( object->mesh() );
                
                bool isTriangleMesh = true;
                
                for ( PolyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end() ; ++f_it) {
                    
                    // Count number of vertices for the current face
                    uint count = 0;
                    for ( PolyMesh::FaceVertexIter fv_it( mesh,f_it); fv_it; ++fv_it )
                        ++count;
                    
                    // Check if it is a triangle. If not, this is really a poly mesh
                    if ( count != 3 ) {
                        isTriangleMesh = false;
                        break;
                    }
                    
                }
                
                // Mesh loaded as polymesh is actually a triangle mesh. Ask the user to reload as triangle mesh or keep it as poly mesh.
                if ( isTriangleMesh )
                    
                    if ( triMeshControl == 0){ //ask what to do
                        
                        QMessageBox::StandardButton result = QMessageBox::question ( 0,
                            tr("TriMesh loaded as PolyMesh"),
                            tr("You opened the mesh as a poly mesh but actually its a triangle mesh. \nShould it be opened as a triangle mesh?"),
                            (QMessageBox::Yes | QMessageBox::No ),
                            QMessageBox::Yes );
                            
                            // User decided to reload as triangle mesh
                            if ( result == QMessageBox::No )
                                isTriangleMesh = false;
                    }
                    
                    if ( isTriangleMesh ){
                        std::cerr << "Plugin FileOM : Reloading mesh as Triangle Mesh\n";
                        
                        emit deleteObject( object->id() );
                        
                        return loadTriMeshObject(_filename);
                    }
            }
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        emit openedFile( object->id() );
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
};

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
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
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
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOM : Read error for Triangle Mesh\n";
            emit deleteObject( object->id() );
            return -1;
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        emit openedFile( object->id() );
        
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
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
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
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOM : Read error for Poly Mesh\n";
            emit deleteObject( object->id() );
            return -1;
            
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        emit openedFile( object->id() );
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

bool FileOMPlugin::saveObject(int _id, QString _filename)
{
    BaseObjectData* object;
    PluginFunctions::getObject(_id,object);
    
    std::string filename = std::string( _filename.toUtf8() );
    
    if ( object->dataType( DATA_POLY_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );
        
        if (OpenMesh::IO::write_mesh(*polyObj->mesh(), filename.c_str()) ){
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        }else{
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );
        
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
            
            if (saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;
            
            if (saveVertexNormal_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (saveVertexTexCoord_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
            if (saveVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (saveFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            if (saveFaceNormal_->isChecked())
                opt += OpenMesh::IO::Options::FaceNormal;
            
        }
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(),opt) ) {
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOMPlugin::saveOptionsWidget(QString _currentFilter) {
    
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
        
        /// \todo : possible options ?:
        // Triangle Mesh in File -> Load as Triangle Mesh
        // Triangle Mesh in File -> Load as Poly Mesh
        // Poly Mesh in File     -> Load as Triangle Mesh
        // Poly Mesh in File     -> Load as Poly Mesh
        
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Open as PolyMesh") );
        triMeshHandling_->addItem( tr("Open as TriangleMesh") );
        
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
        
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FileOM/Load/TriMeshHandling",2).toInt() );
        
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
}


void FileOMPlugin::slotSaveDefault() {
    
    OpenFlipperSettings().setValue( "FileOM/Save/Binary",      saveBinary_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/Normals",     saveVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/TexCoords",   saveVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/VertexColor", saveVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/FaceColor",   saveFaceColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FileOM/Save/FaceNormal",   saveFaceNormal_->isChecked()  );
  
}

Q_EXPORT_PLUGIN2( fileomplugin , FileOMPlugin );