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



#include "SmootherPlugin.hh"

#include <ACG/Utils/StopWatch.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/TetrahedralMesh/TetrahedralMesh.hh>

SmootherPlugin::SmootherPlugin() :
        iterationsSpinbox_(0)
{

}

SmootherPlugin::~SmootherPlugin()
{

}

void SmootherPlugin::initializePlugin()
{
   // Create the Toolbox Widget
   QWidget* toolBox = new QWidget();
   QGridLayout* layout = new QGridLayout(toolBox);

   QPushButton* smoothButton = new QPushButton("&Smooth",toolBox);
   smoothButton->setToolTip(tr("Smooths an Object using Laplacian Smoothing."));
   smoothButton->setWhatsThis(tr("Smooths an Object using Laplacian Smoothing. Use the Smooth Plugin for more options."));



   iterationsSpinbox_ =  new QSpinBox(toolBox) ;
   iterationsSpinbox_->setMinimum(1);
   iterationsSpinbox_->setMaximum(1000);
   iterationsSpinbox_->setSingleStep(1);
   iterationsSpinbox_->setToolTip(tr("The number of the smooting operations."));
   iterationsSpinbox_->setWhatsThis(tr("Give the number, how often the Laplacian Smoothing should modify the object."));

   QLabel* label = new QLabel("Iterations:");

   layout->addWidget( label             , 0, 0);
   layout->addWidget( smoothButton      , 1, 1);
   layout->addWidget( iterationsSpinbox_, 0, 1);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   connect( smoothButton, SIGNAL(clicked()), this, SLOT(simpleLaplace()) );

   QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"smoother1.png");
   emit addToolbox( tr("Simple Smoother") , toolBox, toolIcon );
}

void SmootherPlugin::pluginsInitialized() {
    
    // Emit slot description
    emit setSlotDescription(tr("simpleLaplace(int)"),   tr("Smooth mesh using the Laplace operator with uniform weights."),
                            QStringList(tr("iterations")), QStringList(tr("Number of iterations")));
}

/** \brief simpleLaplace
 *
 *  Smooth mesh using the Laplace operator
 *  with uniform weights.
 */
void SmootherPlugin::simpleLaplace() {

    int iterations = 1;
    
    if(!OpenFlipper::Options::nogui()) {
        iterations = iterationsSpinbox_->value();
    }
    
    simpleLaplace(iterations);
}

/** \brief simpleLaplace
 *
 * Smooth mesh using the Laplace operator
 * with uniform weights.
 *
 * @param _iterations Number of iterations
 */
void SmootherPlugin::simpleLaplace(int _iterations) {
    
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    bool selectionExists = false;

    if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {

        // Get the mesh to work on
      TriMesh* mesh = PluginFunctions::triMesh(*o_it);

      // Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT< TriMesh::Point > origPositions;

      // Add a property to the mesh to store original vertex positions
      mesh->add_property( origPositions, "SmootherPlugin_Original_Positions" );

      for ( int i = 0 ; i < _iterations ; ++i ) {

          // Copy original positions to backup ( in Vertex property )
          TriMesh::VertexIter v_it, v_end=mesh->vertices_end();
          for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            mesh->property( origPositions, *v_it ) = mesh->point(*v_it);
            // See if at least one vertex has been selected
            selectionExists |= mesh->status(*v_it).selected();
          }

          // Do one smoothing step (For each point of the mesh ... )
          for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {

            if(selectionExists && mesh->status(*v_it).selected() == false) {
              continue;
            }

            TriMesh::Point point = TriMesh::Point(0.0,0.0,0.0);

            // Flag, to skip boundary vertices
            bool skip = false;

            // ( .. for each Outoing halfedge .. )
            TriMesh::VertexOHalfedgeIter voh_it(*mesh,*v_it);
            for ( ; voh_it.is_valid(); ++voh_it ) {
                // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
                point += mesh->property( origPositions, mesh->to_vertex_handle(*voh_it) );

                // Check if the current Halfedge is a boundary halfedge
                // If it is, abort and keep the current vertex position
                if ( mesh->is_boundary( *voh_it ) ) {
                  skip = true;
                  break;
                }

            }

            // Devide by the valence of the current vertex
            point /= mesh->valence( *v_it );

            if ( ! skip ) {
                // Set new position for the mesh if its not on the boundary
                mesh->point(*v_it) = point;
            }
          }

      }// Iterations end

      // Remove the property
      mesh->remove_property( origPositions );

      mesh->update_normals();

      emit updatedObject( o_it->id(), UPDATE_GEOMETRY );
      
      // Create backup
      emit createBackup(o_it->id(), "Simple Smoothing", UPDATE_GEOMETRY );

   } else if ( o_it->dataType( DATA_POLY_MESH ) ) {

       // Get the mesh to work on
      PolyMesh* mesh = PluginFunctions::polyMesh(*o_it);

      // Property for the active mesh to store original point positions
      OpenMesh::VPropHandleT< TriMesh::Point > origPositions;

      // Add a property to the mesh to store original vertex positions
      mesh->add_property( origPositions, "SmootherPlugin_Original_Positions" );

      for ( int i = 0 ; i < _iterations ; ++i ) {

         // Copy original positions to backup ( in Vertex property )
         PolyMesh::VertexIter v_it, v_end=mesh->vertices_end();
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            mesh->property( origPositions, *v_it ) = mesh->point(*v_it);
            // See if at least one vertex has been selected
            selectionExists |= mesh->status(*v_it).selected();
         }

         // Do one smoothing step (For each point of the mesh ... )
         for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it) {

            if(selectionExists && mesh->status(*v_it).selected() == false) {
              continue;
            }

            PolyMesh::Point point = PolyMesh::Point(0.0,0.0,0.0);

            // Flag, to skip boundary vertices
            bool skip = false;

            // ( .. for each Outoing halfedge .. )
            PolyMesh::VertexOHalfedgeIter voh_it(*mesh,*v_it);
            for ( ; voh_it.is_valid(); ++voh_it ) {
               // .. add the (original) position of the Neighbour ( end of the outgoing halfedge )
               point += mesh->property( origPositions, mesh->to_vertex_handle(*voh_it) );

               // Check if the current Halfedge is a boundary halfedge
               // If it is, abort and keep the current vertex position
               if ( mesh->is_boundary( *voh_it ) ) {
                  skip = true;
                  break;
               }

            }

            // Devide by the valence of the current vertex
            point /= mesh->valence( *v_it );

            if ( ! skip ) {
               // Set new position for the mesh if its not on the boundary
               mesh->point(*v_it) = point;
            }
         }

      }// Iterations end

      // Remove the property
      mesh->remove_property( origPositions );

      mesh->update_normals();

      emit updatedObject( o_it->id() , UPDATE_GEOMETRY);
      
      // Create backup
      emit createBackup(o_it->id(), "Simple Smoothing", UPDATE_GEOMETRY);

    } else if ( o_it->dataType( DATA_TETRAHEDRAL_MESH ) ) {

      // Get the mesh to work on
      TetrahedralMesh* mesh = PluginFunctions::tetrahedralMesh(*o_it);

      TetrahedralMeshObject::StatusAttrib& status = PluginFunctions::tetrahedralMeshObject(*o_it)->status();

      // Property for the active mesh to store new point positions
      OpenVolumeMesh::VertexPropertyT<TetrahedralMesh::PointT> newPositions =
      mesh->request_vertex_property<TetrahedralMesh::PointT>("newPositions");

      ACG::StopWatch sw; sw.start();
      for ( int i = 0 ; i < _iterations ; ++i ) {

        for(OpenVolumeMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
          if(!status[*v_it].selected())
          {
            newPositions[*v_it] = TetrahedralMesh::PointT(0,0,0);
            int n = 0.0;
            for(OpenVolumeMesh::VertexOHalfEdgeIter voh_it = mesh->voh_iter(*v_it); voh_it.valid(); ++voh_it)
            {
              newPositions[*v_it] += mesh->vertex(mesh->halfedge(*voh_it).to_vertex());
              ++n;
            }

            newPositions[*v_it] /= double(n);
          }

        // set new positions
        for(OpenVolumeMesh::VertexIter v_it = mesh->vertices_begin(); v_it != mesh->vertices_end(); ++v_it)
          if(!status[*v_it].selected())
          {
            mesh->set_vertex(*v_it,newPositions[*v_it]);
          }

      }// Iterations end

      std::cerr << _iterations << "smoothing iterations took " << sw.stop()/1000.0 << "s on mesh with #V = " << mesh->n_vertices() << std::endl;

      emit updatedObject( o_it->id() , UPDATE_GEOMETRY);

      // Create backup
      emit createBackup(o_it->id(), "Simple Smoothing", UPDATE_GEOMETRY);
    }
    else
    {
      emit log(LOGERR, "DataType not supported.");
    }
  }
  
  // Show script logging
  emit scriptInfo("simpleLaplace(" + QString::number(_iterations) + ")");
  
  emit updateView();
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( smootherplugin , SmootherPlugin );
#endif



