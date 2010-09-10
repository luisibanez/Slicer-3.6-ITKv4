/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerApplication_h
#define __qSlicerApplication_h

// Qt includes
#include <QPalette>

// CTK includes
#include <ctkPimpl.h>

// QTCORE includes
#include "qSlicerCoreApplication.h"

// QTGUI includes
#include "qSlicerBaseQTGUIExport.h"

class qSlicerApplicationPrivate;
class qSlicerCommandOptions;
class qSlicerIOManager;
#ifdef Slicer3_USE_PYTHONQT
class qSlicerPythonManager;
#endif
class qSlicerLayoutManager;
class qSlicerWidget;

class Q_SLICER_BASE_QTGUI_EXPORT qSlicerApplication : public qSlicerCoreApplication
{
  Q_OBJECT

public:

  typedef qSlicerCoreApplication Superclass;
  qSlicerApplication(int &argc, char **argv);
  virtual ~qSlicerApplication();

  ///
  /// Return a reference to the application singleton
  static qSlicerApplication* application();

  ///
  void initialize(bool& exitWhenDone);

  ///
  /// Get commandOptions
  qSlicerCommandOptions* commandOptions();

  ///
  /// Get IO Manager
  qSlicerIOManager* ioManager();

  #ifdef Slicer3_USE_PYTHONQT
  /// Get Python Manager
  qSlicerPythonManager * pythonManager();
  #endif

  ///
  /// Set/Get layout manager
  qSlicerLayoutManager* layoutManager()const;
  void setLayoutManager(qSlicerLayoutManager* layoutManager);

  ///
  /// Set the visibility of the top level widgets.
  /// If set to 'False', the state of the widget will be saved.
  /// If set to 'True, if possible the saved state will be restored.
  void setTopLevelWidgetsVisible(bool visible);

  ///
  /// Set the visibility of a given top level widgets
  void setTopLevelWidgetVisible(qSlicerWidget* widget, bool visible);

  ///
  /// Set/Get default window flags that could be used when displaying top level widgets
  void setDefaultWindowFlags(Qt::WindowFlags defaultWindowFlags);
  Qt::WindowFlags defaultWindowFlags() const;

  ///
  /// TODO
  /// See http://doc.trolltech.com/4.6/qapplication.html#commitData
  /// and http://doc.trolltech.com/4.6/qsessionmanager.html#allowsInteraction
  //virtual void commitData(QSessionManager & manager);
protected:
  QSettings* newSettings(const QString& organization, const QString& application);
private:
  CTK_DECLARE_PRIVATE(qSlicerApplication);
};

/// Apply the Slicer palette to the \c palette
/// Note also that the palette parameter is passed by reference and will be
/// updated using the native paletter and applying Slicer specific properties.
void  Q_SLICER_BASE_QTGUI_EXPORT qSlicerApplyPalette(QPalette& palette);

#endif
