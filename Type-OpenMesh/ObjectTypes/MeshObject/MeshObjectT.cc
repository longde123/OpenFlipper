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
  //  MyTypes
  //
  //=============================================================================

  #define MESHOBJECT_C


  //== INCLUDES =================================================================

  #include <OpenFlipper/common/Types.hh>
  #include <ACG/Scenegraph/DrawModes.hh>
  #include <OpenFlipper/common/GlobalOptions.hh>
  #include <OpenFlipper/BasePlugin/PluginFunctions.hh>


  //== TYPEDEFS =================================================================

  //== CLASS DEFINITION =========================================================

  template < class MeshT  >
  MeshObject< MeshT >::MeshObject(const MeshObject& _object) :
      BaseObjectData(_object),
      statusNode_(0),
      areaNode_(0),
      handleNode_(0),
      featureNode_(0),
      meshNode_(0),
      textureNode_(0),
      shaderNode_(0),
      statusView_(0),
      triangle_bsp_(0)
  {
    init(_object.mesh_);

    
    setName( name() );

//     textures_     = _object.textures_;
//     textureNames_ = _object.textureNames_;
  }

  /** Constructor for Mesh Objects. This object class gets a Separator Node giving
  *  the root node to which it should be connected. The mesh is generated internally
  *  and all nodes for visualization will be added below the scenegraph node.\n
  *  You dont need to create an object of this type manually. Use
  *  PluginFunctions::addTriMesh or PluginFunctions::addPolyMesh instead. The
  *  objectDataType has to match the one of MeshT ( see Types.hh::DataType )
  */
  template < class MeshT  >
  MeshObject< MeshT >::MeshObject( DataType _typeId ) :
    BaseObjectData(),
    mesh_(0),
    statusNode_(0),
    areaNode_(0),
    handleNode_(0),
    featureNode_(0),
    meshNode_(0),
    textureNode_(0),
    shaderNode_(0),
    statusView_(0),
    triangle_bsp_(0)
  {
    setDataType(_typeId);
    init();
  }

  /** Destructor for Mesh Objects. The destructor deletes the mesh and all
  *  Scenegraph nodes associated with the mesh or the object.
  */
  template < class MeshT  >
  MeshObject< MeshT >::~MeshObject()
  {
    // Delete the data attached to this object ( this will remove all perObject data)
    // Not the best way to do it but it will work.
    // This is only necessary if people use references to the mesh below and
    // they do something with the mesh in the destructor of their
    // perObjectData.
    deleteData();

    // Delete the Mesh only, if this object contains a mesh
    if ( mesh_ != NULL)  {
      delete mesh_;
      mesh_ = NULL;
    } else {
      std::cerr << "Destructor error : Mesh already deleted" << std::endl;
    }

    if ( triangle_bsp_ != 0 )
      delete triangle_bsp_;
    triangle_bsp_ = 0;

    // No need to delete the scenegraph Nodes as this will be managed by baseplugin
    areaNode_    = 0;
    handleNode_  = 0;
    featureNode_ = 0;
    meshNode_    = 0;
    textureNode_ = 0;
    shaderNode_  = 0;
    statusView_  = 0;
  }

  /** Cleanup Function for Mesh Objects. Deletes the contents of the whole object and
  * calls MeshObject::init afterwards.
  */
  template < class MeshT  >
  void MeshObject< MeshT >::cleanup() {
    // Delete the Mesh only, if this object contains a mesh
    if ( mesh_ != NULL)  {
      delete mesh_;
      mesh_ = NULL;
    } else {
      std::cerr << "Cleanup error : Triangle Mesh already deleted" << std::endl;
    }

    if ( triangle_bsp_ != 0 )
      delete triangle_bsp_;
    triangle_bsp_ = 0;

    BaseObjectData::cleanup();

    statusNode_  = 0;
    areaNode_    = 0;
    handleNode_  = 0;
    featureNode_ = 0;
    textureNode_ = 0;
    shaderNode_  = 0;
    meshNode_    = 0;
    statusView_  = 0;

    init();

  }

  /** This function initalizes the mesh object. It creates the scenegraph nodes,
  *  the mesh and requests all required properties for the mesh.
  */
  template < class MeshT  >
  void MeshObject< MeshT >::init(MeshT* _mesh) {

    if ( _mesh == 0 )
      mesh_ = new MeshT();
    else
      mesh_ = new MeshT(*_mesh);

    // Prepare mesh and request required properties
    mesh_->request_vertex_normals();
    mesh_->request_face_normals();
    mesh_->request_vertex_status();
    mesh_->request_halfedge_status();
    mesh_->request_face_status();
    mesh_->request_edge_status();
    mesh_->request_vertex_colors();
    mesh_->request_face_colors();

    // Only initialize scenegraph nodes when we initialized a gui!!
    if ( OpenFlipper::Options::nogui() )
      return;

    // This should never happen!
    if ( manipulatorNode() == NULL)
      std::cerr << "Error when creating Mesh Object! manipulatorNode is NULL!" << std::endl;


    textureNode_ = new ACG::SceneGraph::EnvMapNode(materialNode(),"NEW TextureNode for ", true, GL_LINEAR_MIPMAP_LINEAR );

    shaderNode_  = new ACG::SceneGraph::ShaderNode(textureNode() , "NEW ShaderNode for ");

    meshNode_    = new ACG::SceneGraph::MeshNodeT<MeshT>(*mesh_, shaderNode_, "NEW MeshNode");

    QString shaderDir = OpenFlipper::Options::shaderDirStr() + OpenFlipper::Options::dirSeparator();

    std::string shaderDirectory = std::string( shaderDir.toUtf8() );
    shaderNode_->setShaderDir( shaderDirectory );


    if ( QFile( shaderDir + "Phong/Vertex.glsl").exists() && QFile( shaderDir + "Phong/Fragment.glsl" ).exists() )
      shaderNode_->setShader(ACG::SceneGraph::DrawModes::SOLID_PHONG_SHADED,"Phong/Vertex.glsl" , "Phong/Fragment.glsl" );
    else
      std::cerr << "Shader Files for Phong not found!" << std::endl;


    // Node showing selection
    statusNode_ = new ACG::SceneGraph::SelectionNodeT<MeshT>(*mesh_, 0, "NEW StatusNode for mesh " );
    statusNode_->set_point_size(4.0);
    statusNode_->set_color(ACG::Vec4f(1.0f,0.0f,0.0f,1.0f));
    statusNode_->set_base_color(ACG::Vec4f(1.0f,0.0f,0.0f,1.0f));
    // Status nodes are handled specially by their StatusViewNodeT parent which
    // is why they get a NONE draw mode.
    statusNode_->drawMode(ACG::SceneGraph::DrawModes::NONE);

    // Node showing modeling region
    areaNode_ = new ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >(*mesh_, 0, "NEW AreaNode for mesh ");
    areaNode_->set_round_points(true);
    areaNode_->enable_alpha_test(0.5);
    areaNode_->set_point_size(7.0);
    areaNode_->set_color(ACG::Vec4f(0.4f, 0.4f, 1.0f, 1.0f));
    // Status nodes are handled specially by their StatusViewNodeT parent which
    // is why they get a NONE draw mode.
    areaNode_->drawMode(ACG::SceneGraph::DrawModes::NONE);

    // Node showing handle region
    handleNode_ = new ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >(*mesh_, 0, "NEW HandleNode for mesh ");
    handleNode_->set_round_points(true);
    handleNode_->enable_alpha_test(0.5);
    handleNode_->set_line_width(2.0);
    handleNode_->set_point_size(7.0);
    handleNode_->set_color(ACG::Vec4f(0.2f, 1.0f, 0.2f, 1.0f));
    // Status nodes are handled specially by their StatusViewNodeT parent which
    // is why they get a NONE draw mode.
    handleNode_->drawMode(ACG::SceneGraph::DrawModes::NONE);

    // Node showing feature selection
    featureNode_ = new ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >(*mesh_, 0, "NEW FeatureNode for mesh ");
    featureNode_->set_round_points(true);
    featureNode_->enable_alpha_test(0.5);
    featureNode_->set_line_width(2.0);
    featureNode_->set_point_size(7.0);
    featureNode_->set_color(ACG::Vec4f(1.0f, 0.2f, 1.0f, 1.0f));
    featureNode_->set_base_color(ACG::Vec4f(1.0f, 0.2f, 1.0f, 1.0f));
    // Status nodes are handled specially by their StatusViewNodeT parent which
    // is why they get a NONE draw mode.
    featureNode_->drawMode(ACG::SceneGraph::DrawModes::NONE);

    // Link the status nodes to the draw mesh of the mesh below them to reuse its buffers
    if (meshNode_) {
      statusNode_->setDrawMesh(meshNode_->getDrawMesh());
      featureNode_->setDrawMesh(meshNode_->getDrawMesh());
      areaNode_->setDrawMesh(meshNode_->getDrawMesh());
      handleNode_->setDrawMesh(meshNode_->getDrawMesh());
    }

    // Node rendering selections in correct order
    statusView_ = new ACG::SceneGraph::StatusViewNodeT<MeshT>(manipulatorNode(), "NEW StatusViewNode for mesh ",
      statusNode_,
      areaNode_,
      handleNode_,
      featureNode_);

    // make StatusViewNode parent of status nodes
    statusNode_->set_parent(statusView_);
    areaNode_->set_parent(statusView_);
    handleNode_->set_parent(statusView_);
    featureNode_->set_parent(statusView_);

    // Update all nodes
    update();
  }

  // ===============================================================================
  // Name/Path Handling
  // ===============================================================================

  /** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
  * BaseObjectData::setName.
  */
  template < class MeshT  >
  void MeshObject< MeshT >::setName( QString _name ) {
    BaseObjectData::setName(_name);

    // No update when gui is not active
    if ( OpenFlipper::Options::nogui() )
      return;

    std::string nodename = std::string("StatusNode for mesh " + _name.toUtf8() );
    statusNode_->name( nodename );

    nodename = std::string("AreaNode for mesh   " + _name.toUtf8() );
    areaNode_->name( nodename );

    nodename = std::string("HandleNode for mesh " + _name.toUtf8() );
    handleNode_->name( nodename );

    nodename = std::string("FeatureNode for mesh " + _name.toUtf8() );
    featureNode_->name( nodename );

    nodename = std::string("TextureNode for mesh "+ _name.toUtf8() );
    textureNode_->name( nodename );

    nodename = std::string("ShaderNode for mesh "+ _name.toUtf8() );
    shaderNode_->name( nodename );

    nodename = std::string("MeshNode for mesh "     + _name.toUtf8() );
    meshNode_->name( nodename );

    nodename = std::string("StatusViewNode for mesh "     + _name.toUtf8() );
    statusView_->name( nodename );
  }

  // ===============================================================================
  // Content
  // ===============================================================================

  /** Get a pointer to the objects mesh.
  * @return Pointer to the mesh
  */
  template < class MeshT  >
  MeshT* MeshObject< MeshT >::mesh() {
    return mesh_;
  }

  /** Updates the visualization of the object. Calls MeshObject::updateGeometry,
  *  MeshObject::updateTopology, MeshObject::updateSelection and
  *  MeshObject::updateModelingRegions.
  */
  template < class MeshT  >
  void MeshObject< MeshT >::update(UpdateType _type) {

    // No update necessary if no gui
    if ( OpenFlipper::Options::nogui() )
      return;

    PluginFunctions::setMainGLContext();

    if ( _type.contains(UPDATE_ALL) || _type.contains(UPDATE_TOPOLOGY) ){
      updateGeometry();
      updateColor();
      updateTopology();
      updateSelection();
      updateFeatures();
      updateModelingRegions();
      updateTexture();
    }
    else 
    {
      if ( _type.contains(UPDATE_GEOMETRY) ) {
        updateGeometry();
      } 
      if ( _type.contains(UPDATE_SELECTION) ) {
        updateSelection();
        updateFeatures();
        updateModelingRegions();
      }
      if ( _type.contains(UPDATE_COLOR) ) {
        updateColor();
      }
      if ( _type.contains(UPDATE_TEXTURE)) {
        updateTexture();
      }
    }
  }

  /** Updates the selection scenegraph nodes */
  template < class MeshT  >
  void MeshObject< MeshT >::updateSelection() {

    if ( statusNode_ ){
      statusNode_->updateSelection();
    }
  }

  /** Updates the geometry information in the mesh scenegraph node */
  template < class MeshT  >
  void MeshObject< MeshT >::updateGeometry() {

    if ( meshNode_ ) {
      meshNode_->update_geometry();

      // Also update the selection nodes here.
      // These nodes store their positions based on the mesh.
      // So they would be at the wrong position afterwards
      statusNode_->updateGeometry();
      featureNode_->updateGeometry();
      areaNode_->updateGeometry();
      handleNode_->updateGeometry();

    }

    invalidateTriangleBsp();
  }

  /** Updates the color information in the mesh scenegraph node */
  template < class MeshT  >
  void MeshObject< MeshT >::updateColor() {
    if ( meshNode_ )
      meshNode_->update_color();
  }

  /** Updates the topology information in the mesh scenegraph node */
  template < class MeshT  >
  void MeshObject< MeshT >::updateTopology() {
    if ( meshNode_ ) {
      meshNode_->update_topology();
      statusNode_->updateTopology();
      featureNode_->updateTopology();
      areaNode_->updateTopology();
      handleNode_->updateTopology();
    }
    invalidateTriangleBsp();
  }

  /** Updates the modeling regions scenegraph nodes */
  template < class MeshT  >
  void MeshObject< MeshT >::updateModelingRegions() {
    if ( areaNode_ && handleNode_ ) {
      areaNode_->updateSelection();
      handleNode_->updateSelection();
    }
  }

  /** Updates the modeling regions scenegraph nodes */
  template < class MeshT  >
  void MeshObject< MeshT >::updateFeatures() {
    if ( featureNode_ )
      featureNode_->updateSelection();
  }

  /** Updates the modeling regions scenegraph nodes */
  template < class MeshT  >
  void MeshObject< MeshT >::updateTexture() {
      meshNode_->update_textures();
  }


  // ===============================================================================
  // Visualization
  // ===============================================================================
  template < class MeshT  >
  void MeshObject< MeshT >::setSelectionColor(const ACG::Vec4f& _color)
  {
    if (statusNode_)
    {
      statusNode_->set_color(_color);
      statusNode_->set_base_color(_color);
    }
  }

  template < class MeshT  >
  ACG::Vec4f MeshObject< MeshT >::selectionColor() const
  {
    if (statusNode_)
      return statusNode_->base_color();
    else
      return ACG::Vec4f(-1.f,-1.f,-1.f,-1.f);
  }

  template < class MeshT  >
  void MeshObject< MeshT >::setAreaColor(const ACG::Vec4f& _color)
  {
    if (areaNode_)
    {
      areaNode_->set_color(_color);
      areaNode_->set_base_color(_color);
    }
  }
  template < class MeshT  >
  ACG::Vec4f MeshObject< MeshT >::areaColor() const
  {
    if (areaNode_)
      return areaNode_->base_color();
    else
      return ACG::Vec4f(-1.f,-1.f,-1.f,-1.f);
  }

  template < class MeshT  >
  void MeshObject< MeshT >::setFeatureColor(const ACG::Vec4f& _color)
  {
    if (featureNode_)
    {
      featureNode_->set_color(_color);
      featureNode_->set_base_color(_color);
    }
  }
  template < class MeshT  >
  ACG::Vec4f MeshObject< MeshT >::featureColor() const
  {
    if (featureNode_)
      return featureNode_->base_color();
    else
      return ACG::Vec4f(-1.f,-1.f,-1.f,-1.f);
  }

  template < class MeshT  >
  void MeshObject< MeshT >::setHandleColor(const ACG::Vec4f& _color)
  {
    if (handleNode_)
    {
      handleNode_->set_color(_color);
      handleNode_->set_base_color(_color);
    }
  }
  template < class MeshT  >
  ACG::Vec4f MeshObject< MeshT >::handleColor() const
  {
    if (handleNode_)
      return handleNode_->base_color();
    else
      return ACG::Vec4f(-1.f,-1.f,-1.f,-1.f);
  }


  /** Returns a pointer to the texture node
  * @return Pointer to the texture node
  */
  template < class MeshT  >
  ACG::SceneGraph::EnvMapNode* MeshObject< MeshT >::textureNode() {
    return textureNode_;
  }

  /** Returns a pointer to the shader node
  * @return Pointer to the shader node
  */
  template < class MeshT  >
  ACG::SceneGraph::ShaderNode* MeshObject< MeshT >::shaderNode() {
    return shaderNode_;
  }

  /** Shows or hides the selections on the object
   */
  template < class MeshT  >
  void  MeshObject< MeshT >::hideSelection( bool _hide ) {

    if ( _hide ) {
      statusNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
    } else {
      statusNode_->set_status( ACG::SceneGraph::BaseNode::Active );
    }

  }

  template < class MeshT  >
  void  MeshObject< MeshT >::hideFeatures( bool _hide ) {
    if ( _hide ) {
      featureNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
    } else {
      featureNode_->set_status( ACG::SceneGraph::BaseNode::Active );
    }
  }

  template < class MeshT  >
  bool  MeshObject< MeshT >::featuresVisible( ) {
    return ( featureNode_->status() == ACG::SceneGraph::BaseNode::Active );
  }

  /** Shows or hides the areas on the object
   */
  template < class MeshT  >
  void  MeshObject< MeshT >::hideArea( StatusBits _bit, bool _hide ) {
    ACG::SceneGraph::BaseNode::StatusMode status;

    if ( _hide ) {
      status = ACG::SceneGraph::BaseNode::HideNode;
    }else
      status = ACG::SceneGraph::BaseNode::Active;

    if ( _bit & AREA ) {
      areaNode_->set_status( status );
    }

    if ( _bit & HANDLEAREA )
      handleNode_->set_status( status );

  }

  template < class MeshT  >
  bool MeshObject< MeshT >::selectionVisible() {
    return ( statusNode_->status() == ACG::SceneGraph::BaseNode::Active );
  }

  template < class MeshT  >
  bool MeshObject< MeshT >::areaVisible( StatusBits _bit ) {
    bool status = true;

    if ( _bit & AREA )
      status &= ( areaNode_->status()   == ACG::SceneGraph::BaseNode::Active );

    if ( _bit & HANDLEAREA )
      status &= ( handleNode_->status() == ACG::SceneGraph::BaseNode::Active );

    return status;
  }

  /** Returns a pointer to the mesh node
  * @return Pointer to the mesh node
  */
  template < class MeshT  >
  ACG::SceneGraph::MeshNodeT<MeshT>* MeshObject< MeshT >::meshNode() {
    return meshNode_;
  }

  /** Get the Bounding box size of this object
   */
  template < class MeshT  >
  void MeshObject< MeshT >::boundingBox( ACG::Vec3d& _bbMin , ACG::Vec3d& _bbMax ) {
    if ( meshNode_ ) {
      _bbMin = ACG::Vec3d(FLT_MAX, FLT_MAX, FLT_MAX);
      _bbMax = ACG::Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
      meshNode_->boundingBox(_bbMin,_bbMax);
    } else {
      std::cerr << "Error: Bounding box computation via Scenegraph not available without gui" << std::endl;
    }
  }

  // ===============================================================================
  // Load/Save
  // ===============================================================================

  /** Load the mesh data from a file. The loader uses the OpenMesh object loaders and
  * therefore supports all mesh types supported by OpenMesh.
  *
  * @param _filename Full path of the file to load.
  */
  template < class MeshT  >
  bool MeshObject< MeshT >::loadMesh(QString _filename) {

    setFromFileName(_filename);

    // call the local function to update names
    setName( name() );

    std::string filename = std::string( _filename.toUtf8() );

    // load file
    bool ok = OpenMesh::IO::read_mesh( (*mesh()) , filename );
    if (!ok)
    {
      if ( dataType() == typeId("TriangleMesh") )
        std::cerr << "Main Application : Read error for Triangle Mesh at "<< filename << std::endl;
      if ( dataType() == typeId("PolyMesh") )
        std::cerr << "Main Application : Read error for Poly Mesh\n";
      return false;
    }

    mesh()->update_normals();

    update();

    show();

    return true;
  }

  // ===============================================================================
  // Object information
  // ===============================================================================

  /** Returns a string containing all information about the current object. This also
  * includes the information provided by BaseObjectData::getObjectinfo
  *
  * @return String containing the object information
  */
  template < class MeshT  >
  QString MeshObject< MeshT >::getObjectinfo() {
    QString output;

    output += "========================================================================\n";
    output += BaseObjectData::getObjectinfo();

    if ( dataType( typeId("TriangleMesh") ) )
      output += "Object Contains Triangle Mesh : ";

    if ( dataType( typeId("PolyMesh") ) )
      output += "Object Contains Poly Mesh : ";

    output += QString::number( mesh()->n_vertices() ) + " vertices, ";
    output += QString::number( mesh()->n_edges() ) += " edges ";
    output += QString::number( mesh()->n_faces() ) += " faces.\n";

    output += "========================================================================\n";
    return output;
  }

  // ===============================================================================
  // Picking
  // ===============================================================================

  /** Given an node index from PluginFunctions::scenegraphPick this function can be used to
  * check if the meshNode of the object has been picked.
  *
  * @param _node_idx Index of the picked mesh node
  * @return bool if the meshNode of this object is the picking target.
  */
  template < class MeshT  >
  bool MeshObject< MeshT >::picked( uint _node_idx ) {
    return ( _node_idx == meshNode_->id() );
  }


  template < class MeshT  >
  void MeshObject< MeshT >::enablePicking( bool _enable ) {
    if ( OpenFlipper::Options::nogui())
      return;

    meshNode_->enablePicking( _enable );
    areaNode_->enablePicking( _enable );
    handleNode_->enablePicking( _enable );
    featureNode_->enablePicking( _enable );
    textureNode_->enablePicking( _enable );
    shaderNode_->enablePicking( _enable );
  }

  template < class MeshT  >
  bool MeshObject< MeshT >::pickingEnabled() {
    return meshNode_->pickingEnabled();
  }

  // ===============================================================================
  // Octree
  // ===============================================================================

  template < class MeshT  >
  typename MeshObject< MeshT >::OMTriangleBSP*
  MeshObject< MeshT >::requestTriangleBsp() {

    if ( ! dataType( typeId("TriangleMesh") ) ) {
      std::cerr << "Bsps are only supported for triangle meshes." << std::endl;
      return 0;
    }

    // Create the tree if needed.
    if ( triangle_bsp_ == 0 )
    {
      // create Triangle BSP
      triangle_bsp_ = new OMTriangleBSP( *mesh() );

      // build Triangle BSP
      triangle_bsp_->reserve(mesh()->n_faces());

      typename MeshT::FIter f_it  = mesh()->faces_begin();
      typename MeshT::FIter f_end = mesh()->faces_end();

      for (; f_it!=f_end; ++f_it)
	     triangle_bsp_->push_back(*f_it);

      triangle_bsp_->build(10, 100); //max vertices per leaf 10, max depth 100
    }

    // return pointer to triangle bsp
    return triangle_bsp_;
  }

  template < class MeshT  >
  typename MeshObject< MeshT >::OMTriangleBSP*
  MeshObject< MeshT >::resetTriangleBsp() {
     if ( triangle_bsp_ != 0 )
     {
       delete triangle_bsp_;
       triangle_bsp_ = 0;
     }

     return requestTriangleBsp();
  }

  template < class MeshT  >
  void 
  MeshObject< MeshT >::invalidateTriangleBsp() {
     if ( triangle_bsp_ != 0 )
     {
       delete triangle_bsp_;
       triangle_bsp_ = 0;
     }
  }


  template < class MeshT  >
  bool 
  MeshObject< MeshT >::hasBsp() const { 
    return triangle_bsp_ != 0; 
  }


  //=============================================================================

  template < class MeshT  >
  BaseNode* 
  MeshObject< MeshT >::primaryNode(){
    return boundingBoxNode();
  } 

  //=============================================================================

