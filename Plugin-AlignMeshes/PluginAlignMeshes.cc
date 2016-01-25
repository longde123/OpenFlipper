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

#include "PluginAlignMeshes.hh"

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include "AlignT.hh"


PluginAlignMeshes::PluginAlignMeshes() :
toolBox_(0),
toolIcon_(0)
{

}

PluginAlignMeshes::~PluginAlignMeshes() {
  if ( OpenFlipper::Options::gui()) {
    delete toolBox_;
    delete toolIcon_;
  }
}

void PluginAlignMeshes::initializePlugin() {

  if ( OpenFlipper::Options::gui()) {
    toolBox_ = new AlignMeshesToolbox();

    toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"alignMeshes.png");
    emit addToolbox("Align Meshes", toolBox_, toolIcon_);

    connect(toolBox_->alignMeshesButton, SIGNAL(pressed()), SLOT(alignMeshes()));
  }
}

void PluginAlignMeshes::alignMeshes() {

  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_TRIANGLE_MESH); o_it
      != PluginFunctions::objectsEnd(); ++o_it) {


    if(o_it->dataType() == DATA_TRIANGLE_MESH) {

      TriMesh& mesh = * PluginFunctions::triMesh(*o_it);

      align::moveToCOG( mesh );
      align::rotate( mesh );

      emit log(LOGINFO,"Moved Triangle mesh alignMeshes");

    } else if(o_it->dataType() == DATA_POLY_MESH) {

      PolyMesh& mesh = * PluginFunctions::polyMesh(*o_it);

      align::moveToCOG( mesh );
      align::rotate( mesh );

      emit log(LOGINFO,"Moved Poly mesh alignMeshes");
    }

    emit updatedObject(o_it->id(), UPDATE_GEOMETRY);

  }
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(pluginalignmeshes, PluginAlignMeshes)
#endif