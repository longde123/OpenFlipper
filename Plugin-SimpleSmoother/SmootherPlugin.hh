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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#ifndef SMOOTHERPLUGIN_HH
#define SMOOTHERPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/common/Types.hh>

class SmootherPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)

  signals:
    //BaseInterface
    void updateView();
    void updatedObject(int _id);

    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

  public:

    // ToolboxInterface
    bool initializeToolbox(QWidget*& _widget);

    // BaseInterface
    QString name() { return (QString("Simple Smoother")); };
    QString description( ) { return (QString("Smooths the active Mesh")); };

   private:

    /// SpinBox for Number of iterations
    QSpinBox* iterationsSpinbox_;

   private slots:
    void simpleLaplace();

   public slots:
      QString version() { return QString("1.0"); };
};

#endif //SMOOTHERPLUGIN_HH
