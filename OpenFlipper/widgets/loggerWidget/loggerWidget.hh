//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 1909 $
//   $Author: wilden $
//   $Date: 2008-06-03 18:45:21 +0200 (Tue, 03 Jun 2008) $
//
//=============================================================================




#ifndef LOGGERWIDGET_HH
#define LOGGERWIDGET_HH

#include <QtGui>

/** \brief Implementation of the logger Widget
 *
 * This class adds some special features to the textedit for the log window
 */
class LoggerWidget : public QTextEdit
{

Q_OBJECT

public:
  LoggerWidget( QWidget *parent = 0 );

protected:

  /** \brief Grab key events before TextEdit
   *
   * This function grabs all key events and passes them back to the core to handle them correctly
   */
  void keyPressEvent (QKeyEvent * _event );
};

#endif //LOGGERWIDGET_HH
