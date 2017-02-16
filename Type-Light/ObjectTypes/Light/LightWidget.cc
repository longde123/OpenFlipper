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

#include "LightWidget.hh"

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
  
  connect(fixedPosition ,SIGNAL(clicked()),this,SLOT(fixedPositionChanged()));
  
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
  
  brightness->setTracking(true);
  connect(brightness, SIGNAL(valueChanged(int)), this, SLOT(brightnessChanged(int)));
  
  connect(ambientR,SIGNAL(editingFinished()),this,SLOT(ambientChanged()));
  connect(ambientG,SIGNAL(editingFinished()),this,SLOT(ambientChanged()));
  connect(ambientB,SIGNAL(editingFinished()),this,SLOT(ambientChanged()));
  connect(ambientA,SIGNAL(editingFinished()),this,SLOT(ambientChanged()));
  
  connect(diffuseR,SIGNAL(editingFinished()),this,SLOT(diffuseChanged()));
  connect(diffuseG,SIGNAL(editingFinished()),this,SLOT(diffuseChanged()));
  connect(diffuseB,SIGNAL(editingFinished()),this,SLOT(diffuseChanged()));
  connect(diffuseA,SIGNAL(editingFinished()),this,SLOT(diffuseChanged()));
  
  connect(specularR,SIGNAL(editingFinished()),this,SLOT(specularChanged()));
  connect(specularG,SIGNAL(editingFinished()),this,SLOT(specularChanged()));
  connect(specularB,SIGNAL(editingFinished()),this,SLOT(specularChanged()));
  connect(specularA,SIGNAL(editingFinished()),this,SLOT(specularChanged()));
  
  connect(radius,SIGNAL(editingFinished()),this,SLOT(radiusChanged()));
}

void LightWidget::showEvent ( QShowEvent * /*event*/ )
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
  
  ambientR->setText(QString::number(light_->ambientColor()[0]));
  ambientG->setText(QString::number(light_->ambientColor()[1]));
  ambientB->setText(QString::number(light_->ambientColor()[2]));
  ambientA->setText(QString::number(light_->ambientColor()[3]));
  
  diffuseR->setText(QString::number(light_->diffuseColor()[0]));
  diffuseG->setText(QString::number(light_->diffuseColor()[1]));
  diffuseB->setText(QString::number(light_->diffuseColor()[2]));
  diffuseA->setText(QString::number(light_->diffuseColor()[3]));
  
  specularR->setText(QString::number(light_->specularColor()[0]));
  specularG->setText(QString::number(light_->specularColor()[1]));
  specularB->setText(QString::number(light_->specularColor()[2]));
  specularA->setText(QString::number(light_->specularColor()[3]));
  
  brightness->setSliderPosition((int)(light_->brightness()*100));
  
  radius->setText(QString::number(light_->radius()));
  
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

void LightWidget::fixedPositionChanged() {
  
  // Block if we currently update the widgets or if we dont get the object
  if (updatingWidgets_ || !getObject() )
    return;
  
  light_->fixedPosition( fixedPosition->isChecked() );
  
  updated();
}

void LightWidget::ambientChanged() {
    
    // Block if we currently update the widgets or if we dont get the object
    if (updatingWidgets_ || !getObject() )
        return;
  
    light_->ambientColor(ACG::Vec4f(ambientR->text().toDouble(),
                                    ambientG->text().toDouble(),
                                    ambientB->text().toDouble(),
                                    ambientA->text().toDouble()));
    updated();
}

void LightWidget::diffuseChanged() {
    
    // Block if we currently update the widgets or if we dont get the object
    if (updatingWidgets_ || !getObject() )
        return;
  
    light_->diffuseColor(ACG::Vec4f(diffuseR->text().toDouble(),
                                    diffuseG->text().toDouble(),
                                    diffuseB->text().toDouble(),
                                    diffuseA->text().toDouble()));
    updated();
}

void LightWidget::specularChanged() {
    
    // Block if we currently update the widgets or if we dont get the object
    if (updatingWidgets_ || !getObject() )
        return;
  
    light_->specularColor(ACG::Vec4f(specularR->text().toDouble(),
                                     specularG->text().toDouble(),
                                     specularB->text().toDouble(),
                                     specularA->text().toDouble()));
    updated();
}
   
void LightWidget::brightnessChanged(int _newValue) {
    
    // Block if we currently update the widgets or if we dont get the object
    if (updatingWidgets_ || !getObject() )
        return;
    
    float pos = _newValue;
    pos /= 100.0f;
    
    light_->brightness(pos);
    
    updated();
}

void LightWidget::radiusChanged() {
    
    // Block if we currently update the widgets or if we dont get the object
    if (updatingWidgets_ || !getObject() )
        return;
    
    light_->radius(radius->text().toDouble());
    
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