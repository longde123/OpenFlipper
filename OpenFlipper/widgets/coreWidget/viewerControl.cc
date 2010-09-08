/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/widgets/snapshotDialog/SnapshotDialog.hh>


//== IMPLEMENTATION ==========================================================

//=============================================================================

void CoreWidget::slotToggleStereoMode()
{
  stereoActive_ = !stereoActive_;

  if ( stereoActive_ ) {
    statusBar_->showMessage(tr("Stereo enabled"));
    stereoButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"stereo.png") );
  } else {
    statusBar_->showMessage(tr("Stereo disabled"));
    stereoButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"mono.png") );
  }

  cursorPainter_->setEnabled (stereoActive_);

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    examiner_widgets_[i]->setStereoMode(stereoActive_);
}

void CoreWidget::slotSetGlobalBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i )
      PluginFunctions::viewerProperties(i).backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                      ((double) c.greenF()) ,
                                                                      ((double) c.blueF())  ,
                                                                        1.0));

  OpenFlipperSettings().setValue("Core/Gui/glViewer/defaultBackgroundColor",c);

}

void CoreWidget::slotSetContextBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    PluginFunctions::viewerProperties().backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                    ((double) c.greenF()) ,
                                                                    ((double) c.blueF())  ,
                                                                     1.0));
}


/// Set the viewer to home position
void CoreWidget::slotContextHomeView() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->home();
}

/// Show / hide wheels in examiner windows
void CoreWidget::slotSwitchWheels(bool _state) {
  std::vector< glViewer* >::iterator it = examiner_widgets_.begin();

  for(; it != examiner_widgets_.end(); it++)
    _state ? (*it)->slotShowWheels() : (*it)->slotHideWheels();
}

/// Switch navigation mode
void CoreWidget::slotSwitchNavigation(bool _egomode) {
  std::vector< glViewer* >::iterator it = examiner_widgets_.begin();

  for(; it != examiner_widgets_.end(); it++) {
    _egomode ? (*it)->navigationMode(glViewer::FIRSTPERSON_NAVIGATION) :
    	(*it)->navigationMode(glViewer::NORMAL_NAVIGATION);
  }
}

/// Set the viewer to home position
void CoreWidget::slotGlobalHomeView() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->home();
}

/// Set the viewers home position
void CoreWidget::slotContextSetHomeView() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->setHome();
}


/// Set the home position for all viewers
void CoreWidget::slotGlobalSetHomeView() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->setHome();
}

/// Change view on active viewer to view complete scene
void CoreWidget::slotContextViewAll() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->viewAll();
}


/// Change view on all viewers to view complete scene
void CoreWidget::slotGlobalViewAll() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->viewAll();
}

/// Toggle projection mode of the active viewer
void CoreWidget::slotContextSwitchProjection() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->toggleProjectionMode();
}

/// Toggle projection mode of all viewers to perspective projection
void CoreWidget::slotGlobalPerspectiveProjection() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->perspectiveProjection();
}

/// Toggle projection mode of all viewers to orthogonal projection
void CoreWidget::slotGlobalOrthographicProjection() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->orthographicProjection();
}


/// If animation is disabled in all viewers, enable it in all viewers. Otherwise disable it.
void CoreWidget::slotGlobalToggleAnimation() {
  int enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).animation() )
      enabledCount++;
  }
  
  slotGlobalChangeAnimation (enabledCount == 0);
}

/// Set the animation mode for all viewers
void CoreWidget::slotGlobalChangeAnimation(bool _animation){
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).animation(_animation);
}

/// Set the animation mode for active viewer
void CoreWidget::slotLocalChangeAnimation(bool _animation){
  PluginFunctions::viewerProperties().animation(_animation);
}


/// If backface culling is disabled in all viewers, enable it in all viewers. Otherwise disable it.
void CoreWidget::slotGlobalToggleBackFaceCulling() {
  int enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).backFaceCulling() )
      enabledCount++;
  }
  
  slotGlobalChangeBackFaceCulling (enabledCount == 0);
}

/// Set backface culling for all viewers
void CoreWidget::slotGlobalChangeBackFaceCulling(bool _backFaceCulling){
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).backFaceCulling(_backFaceCulling);

}

/// Set backface culling for active viewer
void CoreWidget::slotLocalChangeBackFaceCulling(bool _backFaceCulling){
  PluginFunctions::viewerProperties().backFaceCulling(_backFaceCulling);
}


/// If two-sided lighting is disabled in all viewers, enable it in all viewers. Otherwise disable it.
void CoreWidget::slotGlobalToggleTwoSidedLighting() {
  int enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).twoSidedLighting() )
      enabledCount++;
  }
  
  slotGlobalChangeTwoSidedLighting (enabledCount == 0);
}

/// Set two-sided lighting for all viewers
void CoreWidget::slotGlobalChangeTwoSidedLighting(bool _lighting) {
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).twoSidedLighting(_lighting);
}

/// Set two-sided lighting for active viewer
void CoreWidget::slotLocalChangeTwoSidedLighting(bool _lighting) {
  PluginFunctions::viewerProperties().twoSidedLighting(_lighting);
}


/// If multisampling is disabled in all viewers, enable it in all viewers. Otherwise disable it.
void CoreWidget::slotGlobalToggleMultisampling() {
  int enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).multisampling() )
      enabledCount++;
  }
  
  slotGlobalChangeMultisampling (enabledCount == 0);
}

/// Set multisampling for all viewers
void CoreWidget::slotGlobalChangeMultisampling(bool _multisampling) {
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).multisampling( _multisampling );
}

/// Set multisampling for active viewer
void CoreWidget::slotLocalChangeMultisampling(bool _multisampling) {
  PluginFunctions::viewerProperties().multisampling( _multisampling );
}


/// If mipmapping is disabled in all viewers, enable it in all viewers. Otherwise disable it.
void CoreWidget::slotGlobalToggleMipmapping() {
  int enabledCount  = 0;

  for ( int i = 0 ; i< PluginFunctions::viewers(); ++i ) {
    if ( PluginFunctions::viewerProperties(i).mipmapping() )
      enabledCount++;
  }
  
  slotGlobalChangeMipmapping (enabledCount == 0);
}

/// Set mipmapping for all viewers
void CoreWidget::slotGlobalChangeMipmapping(bool _mipmapping) {
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).mipmapping( _mipmapping );
}

/// Set mipmapping for active viewer
void CoreWidget::slotLocalChangeMipmapping(bool _mipmapping) {
  PluginFunctions::viewerProperties().mipmapping( _mipmapping );
}

/// Take a snapshot of the current Viewer
void CoreWidget::slotExaminerSnapshot() {

  QFileInfo fi(PluginFunctions::viewerProperties().snapshotName());
  int counter = PluginFunctions::viewerProperties().snapshotCounter();

    // Add leading zeros
  QString number = QString::number(counter);
  while ( number.size() < 7 )
    number = "0" + number;

  QString suggest = fi.baseName() + "." + number + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter(tr("Images (*.png *.ppm *.jpg)"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory( fi.path() );
  dialog.selectFile( suggest );
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save Snapshot"));

 if (dialog.exec()){
     QString newName = dialog.selectedFiles()[0];

      if (newName != fi.path() + OpenFlipper::Options::dirSeparator() + suggest)
        PluginFunctions::viewerProperties().snapshotBaseFileName(newName);

      QImage image;
      examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot(image);

      image.save(newName);
  }
}

///Take a snapshot of the whole application
void CoreWidget::applicationSnapshotDialog() {

  QFileInfo fi(snapshotName_);
  
  if (snapshotName_ == "")
    fi.setFile( OpenFlipperSettings().value("Core/CurrentDir").toString() + QDir::separator() + "snap.png" );

  // Add leading zeros
  QString number = QString::number(snapshotCounter_);
  while ( number.size() < 7 )
    number = "0" + number;

  QString suggest = fi.path() + QDir::separator() + fi.baseName() + "." + number + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  int w = width();
  int h = height();

  SnapshotDialog dialog(suggest, false, w, h, 0);
  
  connect(&dialog, SIGNAL(resizeApplication(int,int)), this, SIGNAL(resizeApplication(int,int)) );

  bool ok = dialog.exec();

  if ( ok ){
    QString newName = dialog.filename->text();

    if (newName != suggest){
      
      OpenFlipperSettings().setValue("Core/CurrentDir", QFileInfo(newName).absolutePath() );
      
      snapshotName_ = newName;
      snapshotCounter_ = 1;
    }else
      snapshotCounter_++;

    QApplication::processEvents();

    QPixmap pic = QPixmap::grabWindow( winId() );

    pic.save(newName);
  }

  emit resizeApplication(w,h);
}

///Take a snapshot of the whole application
void CoreWidget::applicationSnapshot() {

  QFileInfo fi(snapshotName_);

  // Add leading zeros
  QString number = QString::number(snapshotCounter_++);
  while ( number.size() < 7 )
    number = "0" + number;

  QString suggest = fi.path() + QDir::separator() +fi.baseName() + "." + number + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  QApplication::processEvents();

  QPixmap pic = QPixmap::grabWindow( winId() );

  pic.save(suggest);
}


///Take a snapshot of all viewers
void CoreWidget::viewerSnapshotDialog() {

  QFileInfo fi(snapshotName_);
  
  if (snapshotName_ == "")
    fi.setFile( OpenFlipperSettings().value("Core/CurrentDir").toString() + QDir::separator() + "snap.png" );

  // Add leading zeros
  QString number = QString::number(snapshotCounter_);
  while ( number.size() < 7 )
    number = "0" + number;

  QString suggest = fi.path() + QDir::separator() + fi.baseName() + "." + number + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  int w = glView_->width();
  int h = glView_->height();
  
  SnapshotDialog dialog(suggest, true, w, h, 0);
  
  bool ok = dialog.exec();

  if (ok){
    QString newName = dialog.filename->text();

    if (newName != suggest){
      
      OpenFlipperSettings().setValue("Core/CurrentDir", QFileInfo(newName).absolutePath() );
      
      snapshotName_ = newName;
      snapshotCounter_ = 1;
      
    }else
      snapshotCounter_++;
    
    //now take the snapshot
    switch ( baseLayout_->mode() ){

      case QtMultiViewLayout::SingleView:
      {
        QImage finalImage;

        examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot(finalImage,
                dialog.snapWidth->value(), dialog.snapHeight->value(),
                dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked(),
		dialog.multisampling->isChecked() ? dialog.num_samples->value() : 1);

        finalImage.save(newName);

        break;
      }
      case QtMultiViewLayout::Grid:
      {
        // Compute size of each viewer
        int w = dialog.snapWidth->value();
        int h = dialog.snapHeight->value();
        
        // Relative size of first viewer (in relation to the other viewers
        double relSizeW = (double)examiner_widgets_[0]->glWidth() / (double)glScene_->width();
        double relSizeH = (double)examiner_widgets_[0]->glHeight() / (double)glScene_->height();
          
        QImage img0,img1,img2,img3;

        examiner_widgets_[0]->snapshot(img0, (int)((double)w * relSizeW),           (int)((double)h * relSizeH),
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        examiner_widgets_[1]->snapshot(img1, (int)((double)w * (1.0 - relSizeW)),   (int)((double)h * relSizeH),
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        examiner_widgets_[2]->snapshot(img2, (int)((double)w * relSizeW),           (int)((double)h * (1.0 - relSizeH)),
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        examiner_widgets_[3]->snapshot(img3, (int)((double)w * (1.0 - relSizeW)),   (int)((double)h * (1.0 - relSizeH)),
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        
        img0.save("/home/kremer/multsnap0.png");
        img1.save("/home/kremer/multsnap1.png");
        img2.save("/home/kremer/multsnap2.png");
        img3.save("/home/kremer/multsnap3.png");

        QImage finalImage(img0.width() + img1.width()+2, img0.height() + img2.height()+2, QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&finalImage);

        painter.fillRect(0,0,finalImage.width(), finalImage.height(), QBrush(Qt::gray));

        painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                          QRectF(           0,             0, img0.width(), img0.height()) );
        painter.drawImage(QRectF(img0.width()+2,           0, img1.width(), img1.height()),img1,
                          QRectF(           0,             0, img1.width(), img1.height()) );
        painter.drawImage(QRectF(          0,img0.height()+2, img2.width(), img2.height()),img2,
                          QRectF(           0,             0, img2.width(), img2.height()) );
        painter.drawImage(QRectF(img0.width()+2, img0.height()+2, img3.width(), img3.height()),img3,
                          QRectF(           0,             0, img3.width(), img3.height()) );

        finalImage.save(newName);

        break;
      }
      case QtMultiViewLayout::HSplit:
      {
        // Compute size of each viewer
        int w = dialog.snapWidth->value();
        int h = dialog.snapHeight->value();
        
        // Relative size of first viewer (in relation to the other viewers
        double relSizeW = (double)examiner_widgets_[0]->glWidth() / (double)glScene_->width();
        double relSizeH1 = (double)examiner_widgets_[1]->glHeight() / (double)glScene_->height();
        double relSizeH2 = (double)examiner_widgets_[2]->glHeight() / (double)glScene_->height();
        double relSizeH3 = (double)examiner_widgets_[3]->glHeight() / (double)glScene_->height();
          
        QImage img0,img1,img2,img3;

        examiner_widgets_[0]->snapshot(img0, (int)((double)w * relSizeW), h,
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        examiner_widgets_[1]->snapshot(img1, (int)((double)w * (1.0 - relSizeW)), relSizeH1 * (double)h,
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        examiner_widgets_[2]->snapshot(img2, (int)((double)w * (1.0 - relSizeW)), relSizeH2 * (double)h,
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());
        examiner_widgets_[3]->snapshot(img3, (int)((double)w * (1.0 - relSizeW)), relSizeH3 * (double)h,
                                       dialog.transparent->isChecked(), dialog.hideCoordsys->isChecked());

        QImage finalImage(img0.width() + img1.width() +2, img0.height(), QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&finalImage);

        painter.fillRect(0,0,finalImage.width(), finalImage.height(), QBrush(Qt::gray));

        painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                          QRectF(           0,             0, img0.width(), img0.height()) );
        painter.drawImage(QRectF(img0.width()+2,             0, img1.width(), img1.height()),img1,
                          QRectF(           0,             0, img1.width(), img1.height()) );
        painter.drawImage(QRectF(img0.width()+2, img1.height()+2, img2.width(), img2.height()),img2,
                          QRectF(           0,             0, img2.width(), img2.height()) );
        painter.drawImage(QRectF(img0.width()+2, img1.height()+img2.height()+4, img3.width(),img3.height()),img3,
                          QRectF(           0,             0, img3.width(), img3.height()) );

        finalImage.save(newName);

        break;
      }
      default: break;

    }
  }
  //glView_->resize(w, h);
}

///Take a snapshot of all viewers
void CoreWidget::viewerSnapshot() {

  QFileInfo fi(snapshotName_);

  // Add leading zeros
  QString number = QString::number(snapshotCounter_++);
  while ( number.size() < 7 )
    number = "0" + number;

  QString suggest = fi.path() + QDir::separator() +fi.baseName() + "." + number + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;


  switch ( baseLayout_->mode() ){

    case QtMultiViewLayout::SingleView:
    {
      QImage finalImage;

      examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot(finalImage);

      finalImage.save(suggest);

      break;
    }
    case QtMultiViewLayout::Grid:
    {
      QImage img0,img1,img2,img3;
      
      examiner_widgets_[0]->snapshot(img0);
      examiner_widgets_[1]->snapshot(img1);
      examiner_widgets_[2]->snapshot(img2);
      examiner_widgets_[3]->snapshot(img3);

      QImage finalImage(img0.width() + img1.width() + 2, img0.height() + img2.height() + 2, QImage::Format_ARGB32_Premultiplied);

      QPainter painter(&finalImage);

      painter.fillRect(0,0,finalImage.width(), finalImage.height(), QBrush(Qt::gray));

      painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                        QRectF(           0,             0, img0.width(), img0.height()) );
      painter.drawImage(QRectF(img0.width()+2,           0, img1.width(), img1.height()),img1,
                        QRectF(           0,             0, img1.width(), img1.height()) );
      painter.drawImage(QRectF(           0, img0.height()+2, img2.width(), img2.height()),img2,
                        QRectF(           0,             0, img2.width(), img2.height()) );
      painter.drawImage(QRectF(img0.width()+2, img0.height()+2, img3.width(), img3.height()),img3,
                        QRectF(           0,             0, img3.width(), img3.height()) );

      finalImage.save(suggest);

      break;
    }
    case QtMultiViewLayout::HSplit:
    {
      QImage img0,img1,img2,img3;

      examiner_widgets_[0]->snapshot(img0);
      examiner_widgets_[1]->snapshot(img1);
      examiner_widgets_[2]->snapshot(img2);
      examiner_widgets_[3]->snapshot(img3);

      QImage finalImage(img0.width() + img1.width() + 2, img0.height(), QImage::Format_ARGB32_Premultiplied);

      QPainter painter(&finalImage);

      painter.fillRect(0,0,finalImage.width(), finalImage.height(), QBrush(Qt::gray));

      painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                        QRectF(           0,             0, img0.width(), img0.height()) );
      painter.drawImage(QRectF(img0.width()+2,             0, img1.width(), img1.height()),img1,
                        QRectF(           0,             0, img1.width(), img1.height()) );
      painter.drawImage(QRectF(img0.width()+2, img1.height()+2, img2.width(), img2.height()),img2,
                        QRectF(           0,             0, img2.width(), img2.height()) );
      painter.drawImage(QRectF(img0.width()+2, img1.height()+img2.height()+4, img3.width(),img3.height()),img3,
                        QRectF(           0,             0, img3.width(), img3.height()) );

      finalImage.save(suggest);

      break;
    }
    default: break;

  }
}

void CoreWidget::applicationSnapshotName(QString _name) {

  snapshotName_ = _name;
  snapshotCounter_ = 0;
}


void CoreWidget::slotPasteView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionPasteView();
}

void CoreWidget::slotCopyView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionCopyView();
}

void CoreWidget::slotCoordSysVisibility(bool _visible){

  ACG::SceneGraph::BaseNode* root = PluginFunctions::getSceneGraphRootNode();
  ACG::SceneGraph::BaseNode* coordSys = root->find("Core Coordsys Node");

  if (coordSys == 0){
    emit log( LOGERR, tr("CoordSys Node not found"));
    return;
  }

if (_visible)
    coordSys->show();
  else
    coordSys->hide();

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    examiner_widgets_[i]->updateGL();

}

void CoreWidget::slotSetViewingDirection(QAction* _action) {

  PluginFunctions::setFixedView( _action->data().toInt() );
}

void CoreWidget::slotLockRotation(bool _lock) {

  PluginFunctions::allowRotation( !_lock, PluginFunctions::activeExaminer() );
}

void CoreWidget::moveBack() {
    examiner_widgets_[PluginFunctions::activeExaminer()]->moveBack();
}

void CoreWidget::moveForward() {
    examiner_widgets_[PluginFunctions::activeExaminer()]->moveForward();
}

void CoreWidget::strafeLeft() {
    examiner_widgets_[PluginFunctions::activeExaminer()]->strafeLeft();
}

void CoreWidget::strafeRight() {
    examiner_widgets_[PluginFunctions::activeExaminer()]->strafeRight();
}

//=============================================================================
