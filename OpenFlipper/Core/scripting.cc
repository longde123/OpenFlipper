/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
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
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
#include <QtUiTools/QUiLoader>


//== IMPLEMENTATION ==========================================================



void Core::slotScriptInfo( QString _pluginName , QString _functionName  ) {
  emit scriptInfo( _pluginName , _functionName );
}

void Core::slotExecuteScript( QString _script ) {
  emit executeScript( _script );
}

void Core::slotExecuteFileScript( QString _filename ) {
  emit executeFileScript( _filename );
}

void Core::slotGetScriptingEngine( QScriptEngine*& _engine  ) {
  _engine = &scriptEngine_;
}

void Core::slotGetAllAvailableFunctions( QStringList& _functions  ) {
  _functions = scriptingFunctions_;
}

void Core::scriptLogFunction( QString _output) {
   emit scriptLog(_output);
}

void Core::createWidget(QString _objectName, QString _uiFilename) {
  QUiLoader loader;

  QFile uiFile(_uiFilename);

  if ( !uiFile.exists() ) {
    emit log(LOGERR,tr("File does not exist : ") + _uiFilename );
    return;
  }

  uiFile.open(QIODevice::ReadOnly);
  QWidget *ui = loader.load(&uiFile);
  uiFile.close();

  if ( ui == 0 ) {
    emit log(LOGERR,tr("Unable to create QWidget from ui file for ") + _objectName );
	return;
  }

  QScriptValue scriptUi = scriptEngine_.newQObject(ui, QScriptEngine::ScriptOwnership);

  if ( !scriptUi.isValid() ) {
    emit log(LOGERR,tr("Unable to generate script interface for ") + _objectName );
	return;
  }

  scriptEngine_.globalObject().setProperty(_objectName, scriptUi);


  ui->show();

}

//-----------------------------------------------------------------------------

int Core::getObjectId( const QString _name ) {

	return PluginFunctions::getObjectId(_name);
}

//-----------------------------------------------------------------------------

void Core::setViewMode(QString _viewMode){

  if ( OpenFlipper::Options::gui() )
    coreWidget_->setViewMode( _viewMode );
}

//-----------------------------------------------------------------------------

void Core::setViewModeIcon(QString _mode, QString _iconName){

  if ( OpenFlipper::Options::gui() ){

    QFile file(_iconName);
    QFileInfo fileInfo(file);
  
    if ( ! file.exists() ){
      emit log(LOGERR, tr("Icon not found (%1)").arg(_iconName) );
      return;
    }

    file.copy(OpenFlipper::Options::configDirStr() + QDir::separator() + "Icons" + QDir::separator() + "viewMode_" + fileInfo.fileName() );

    coreWidget_->slotSetViewModeIcon( _mode, "viewMode_" + fileInfo.fileName() );
  }
}

//-----------------------------------------------------------------------------

void Core::moveToolBoxToTop(QString _name) {
    
  if(OpenFlipper::Options::gui()) {
    coreWidget_->moveToolBoxToTop(_name);
  }
}

//-----------------------------------------------------------------------------

void Core::moveToolBoxToBottom(QString _name) {
    
  if(OpenFlipper::Options::gui()) {
    coreWidget_->moveToolBoxToBottom(_name);
  }
}

//-----------------------------------------------------------------------------

void Core::addViewModeToolboxes(QString _modeName, QString _toolboxList) {

  QStringList list = _toolboxList.split(";");
  coreWidget_->slotAddViewModeToolboxes(_modeName,list);
}

//-----------------------------------------------------------------------------

void Core::addViewModeToolbars(QString _modeName, QString _toolbarList) {
  
  QStringList list = _toolbarList.split(";");
  coreWidget_->slotAddViewModeToolbars(_modeName,list);
}

//-----------------------------------------------------------------------------

void Core::addViewModeContextMenus(QString _modeName, QString _contextMenuList) {
  
  QStringList list = _contextMenuList.split(";");
  coreWidget_->slotAddViewModeContextMenus(_modeName,list);
}

void Core::addViewModeIcon(QString _modeName, QString _iconName) {
  coreWidget_->slotSetViewModeIcon(_modeName,true,_iconName);
}

//-----------------------------------------------------------------------------

void Core::setToolBoxSide(QString _side) {
  
    if(_side.toLower() == "left") {
        coreWidget_->setToolBoxOrientationOnTheRight(false);
    } else if(_side.toLower() == "right") {
        coreWidget_->setToolBoxOrientationOnTheRight(true);
    } else {
        emit log(LOGERR, QString("Could not display toolboxes on side '%1'. Use either 'left' or 'right' as string!").arg(_side));
    }
}

//-----------------------------------------------------------------------------

void Core::addToolbox(QString _name ,QWidget* _widget) {
  int id = -1;

  // Find the plugin which added this Toolbox
  for ( uint i = 0 ; i < plugins.size(); ++i ) {
    if ( plugins[i].plugin == sender() ) {
      id = i;
      break;
    }
  }

  // Find the scripting plugin because we assign this toolBox to it as we did not find the original sender
  if ( id == -1 ) {
    for ( uint i = 0 ; i < plugins.size(); ++i ) {
      if ( plugins[i].name == "Scripting" ) {
        id = i;
        break;
      }
    }


    if ( id == -1 ) {
      std::cerr << "Unknown sender plugin when adding Toolbox!" << std::endl;
      return;
    }
  }

  plugins[id].toolboxWidgets.push_back( std::pair< QString,QWidget* >( _name , _widget) );
  plugins[id].toolboxIcons.push_back( 0 );

  // add widget name to viewMode 'all'
  if ( !viewModes_[0]->visibleToolboxes.contains(_name) ){
    viewModes_[0]->visibleToolboxes << _name;
    viewModes_[0]->visibleToolboxes.sort();
  }

  setViewMode( OpenFlipper::Options::currentViewMode() );
}

//-----------------------------------------------------------------------------

void Core::addToolbox(QString _name ,QWidget* _widget, QIcon* _icon) {
  int id = -1;

  // Find the plugin which added this Toolbox
  for ( uint i = 0 ; i < plugins.size(); ++i ) {
    if ( plugins[i].plugin == sender() ) {
      id = i;
      break;
    }
  }

  // Find the scripting plugin because we assign this toolBox to it as we did not find the original sender
  if ( id == -1 ) {
    for ( uint i = 0 ; i < plugins.size(); ++i ) {
      if ( plugins[i].name == "Scripting" ) {
        id = i;
        break;
      }
    }


    if ( id == -1 ) {
      std::cerr << "Unknown sender plugin when adding Toolbox!" << std::endl;
      return;
    }
  }

  plugins[id].toolboxWidgets.push_back( std::pair< QString,QWidget* >( _name , _widget) );
  plugins[id].toolboxIcons.push_back( _icon );

  // add widget name to viewMode 'all'
  if ( !viewModes_[0]->visibleToolboxes.contains(_name) ){
    viewModes_[0]->visibleToolboxes << _name;
    viewModes_[0]->visibleToolboxes.sort();
  }

  setViewMode( OpenFlipper::Options::currentViewMode() );
}

void Core::setToolBoxActive(QString _toolBoxName, bool _active)
{
  if ( OpenFlipper::Options::gui() ){
    coreWidget_->toolBox_->setElementActive(_toolBoxName,_active);
  }
}

//=============================================================================
//== Script Special Functions =================================================
//=============================================================================

QScriptValue myPrintFunction(QScriptContext *context, QScriptEngine *engine)
{
  QString result;
  for (int i = 0; i < context->argumentCount(); ++i) {
    if (i > 0)
        result.append(" ");
    result.append(context->argument(i).toString());
  }

  // Get the textedit for Output ( Set in Core.cc )
  QScriptValue calleeData = context->callee().property("textedit");
  Core *widget = qobject_cast<Core*>(calleeData.toQObject());

  widget->scriptLogFunction(result);

  return engine->undefinedValue();
}

