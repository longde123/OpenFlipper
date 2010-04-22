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
 *   $Revision: 8520 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2010-02-10 15:56:59 +0100 (Mi, 10. Feb 2010) $                   *
 *                                                                           *
\*===========================================================================*/

#include "LightWidget.hh"
#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>

LightWidget::LightWidget(  ACG::SceneGraph::BaseNode* _node, QWidget *parent)
 : QDialog(parent),
 object_(0),
 light_(0),
 updatingWidgets_(false)
{
  setupUi(this);

  node_ = dynamic_cast< ACG::SceneGraph::LightNode* >(_node);
  
  if ( ! node_ ) {
    std::cerr << "LightWidget could not cast given node to LightNode!" << std::endl;
    return;
  }
  
  connect(directional,SIGNAL(clicked()),this,SLOT(directionalToggled()));
  
  connect(xpos,SIGNAL(editingFinished()),this,SLOT(directionalToggled()));
  connect(ypos,SIGNAL(editingFinished()),this,SLOT(directionalToggled()));
  connect(zpos,SIGNAL(editingFinished()),this,SLOT(directionalToggled()));
  
  connect(xdir,SIGNAL(editingFinished()),this,SLOT(directionalToggled()));
  connect(ydir,SIGNAL(editingFinished()),this,SLOT(directionalToggled()));
  connect(zdir,SIGNAL(editingFinished()),this,SLOT(directionalToggled()));
  
  connect(spotx,SIGNAL(editingFinished()),this,SLOT(spotChanged()));
  connect(spoty,SIGNAL(editingFinished()),this,SLOT(spotChanged()));
  connect(spotz,SIGNAL(editingFinished()),this,SLOT(spotChanged()));
  
  connect(angle,SIGNAL(editingFinished()),this,SLOT(spotChanged()));
  connect(exponent,SIGNAL(editingFinished()),this,SLOT(spotChanged()));
  
}

void LightWidget::showEvent ( QShowEvent * event )
{
  if ( !getObject() )
    return;
  
  // Block updates
  updatingWidgets_ = true;
  
  directional->setChecked( light_->directional() );
  
  // Switch to right tab
  if ( directional->isChecked() )
    stackedWidget->setCurrentIndex(1);
  else
    stackedWidget->setCurrentIndex(0);
  
  xdir->setText(QString::number(light_->direction()[0]));
  ydir->setText(QString::number(light_->direction()[1]));
  zdir->setText(QString::number(light_->direction()[2]));
  
  xpos->setText(QString::number(light_->position()[0]));
  ypos->setText(QString::number(light_->position()[1]));
  zpos->setText(QString::number(light_->position()[2]));

  
  fixedPosition->setChecked( light_->fixedPosition() );
  
  spotx->setText(QString::number(light_->spotDirection()[0]));
  spoty->setText(QString::number(light_->spotDirection()[1]));
  spotz->setText(QString::number(light_->spotDirection()[2]));
  
  angle->setText(QString::number(light_->spotCutoff()));
  exponent->setText(QString::number(light_->spotExponent()));
  
  // Allow updates
  updatingWidgets_ = false;
  
}


void LightWidget::directionalToggled() {
  
  // Block if we currently update the widgets or if we dont get the object
  if (updatingWidgets_ || !getObject() )
    return;
  
  // Switch to right tab
  if ( directional->isChecked() )
    stackedWidget->setCurrentIndex(1);
  else
    stackedWidget->setCurrentIndex(0);
  
  if ( directional->isChecked() )
    light_->direction( ACG::Vec3d( xdir->text().toDouble() ,ydir->text().toDouble() ,zdir->text().toDouble() ));
  else
    light_->position( ACG::Vec3d( xpos->text().toDouble() ,ypos->text().toDouble() ,zpos->text().toDouble() ));
  
  updated();
 
}

void LightWidget::spotChanged() {
  // Block if we currently update the widgets or if we dont get the object
  if (updatingWidgets_ || !getObject() )
    return;
  
  light_->spotDirection( ACG::Vec3d( spotx->text().toDouble() ,spoty->text().toDouble() ,spotz->text().toDouble() ));
  light_->spotCutoff(angle->text().toDouble());
  
  light_->spotExponent(exponent->text().toDouble());
  
  updated();
}

bool LightWidget::getObject() {
  
 if ( !object_ ) {
   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_LIGHT) ; o_it != PluginFunctions::objectsEnd(); ++o_it)  {
     if ( o_it->hasNode(node_) ) {
       object_ = PluginFunctions::lightObject(*o_it);
       light_  = PluginFunctions::lightSource(object_);
       return true;
     }
   }
   
   std::cerr << " Object not found! " << std::endl;
   
   // Not Found!
   return false;
 }
 
 return true;
}

void LightWidget::updated() {
  
  // Update the object
  object_->update();
  
  // Force a redraw
  node_->setDirty();
  
}
