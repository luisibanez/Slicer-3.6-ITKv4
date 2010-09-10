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

#ifndef __qSlicerAbstractCoreModule_h
#define __qSlicerAbstractCoreModule_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkPimpl.h>

// QTBase includes
#include "qSlicerBaseQTCoreExport.h"

class qSlicerAbstractModuleRepresentation;
class vtkSlicerLogic;
class vtkSlicerApplicationLogic;
class vtkMRMLScene;
class QAction;
class qSlicerAbstractCoreModulePrivate;


#define qSlicerGetTitleMacro(_TITLE)               \
  static QString staticTitle() { return _TITLE; }  \
  virtual QString title()const { return _TITLE; }

/// qSlicerAbstractCoreModule is the base class of any module in Slicer.
/// Core modules, Loadable modules, CLI modules derive from it.
/// It is responsible to create the UI and the Logic:
/// createWidgetRepresentation() and createLogic() must be reimplemented in
/// derived classes.
/// A Slicer module has a name and a title: The name is its UID, the title
/// displayed to the user.
/// When a MRML scene is set to the module, the module set the scene to the
/// UI widget and the logic.
class Q_SLICER_BASE_QTCORE_EXPORT qSlicerAbstractCoreModule : public QObject
{
  /// Any object deriving from QObject must have the Q_OBJECT macro in
  /// order to have the signal/slots working and the meta-class name valid.
  Q_OBJECT

  /// The following property will be added to the meta-class
  /// and will also be available through PythonQt
  Q_PROPERTY(QString Name READ name)
  Q_PROPERTY(QString Title READ title)
  Q_PROPERTY(QString Category READ category)
  Q_PROPERTY(QString Contributor READ contributor)

public:

  typedef QObject Superclass;
  /// Constructor
  /// Warning: If there is no parent given, make sure you delete the object.
  /// The modules can typically be instanciated before the application
  /// is initialized (module manager, iomanager...). Most of the
  /// initialization must be done in qSlicerAbstractCoreModule::setup()
  qSlicerAbstractCoreModule(QObject *parent=0);

  virtual void printAdditionalInfo();

  ///
  /// Convenient method to return slicer wiki URL
  QString slicerWikiUrl()const{ return "http://www.slicer.org/slicerWiki/index.php"; }

  ///
  /// Initialize the module, an appLogic must be given to
  /// initialize the module
  void initialize(vtkSlicerApplicationLogic* appLogic);
  inline bool initialized() { return this->Initialized; }

  ///
  /// Set/Get the name of the module (must be unique)
  virtual QString name()const;
  virtual void setName(const QString& name);

  ///
  /// Title of the module, (displayed to the user)
  virtual QString title()const = 0;
  virtual QString category()const;
  virtual QString contributor()const;

  ///
  /// Return help/acknowledgement text
  /// These functions must be reimplemented in the derived classes
  virtual QString helpText()const;
  virtual QString acknowledgementText()const;

  ///
  /// This method allows to get a pointer to the WidgetRepresentation.
  /// If no WidgetRepresentation already exists, one will be created calling
  /// 'createWidgetRepresentation' method.
  qSlicerAbstractModuleRepresentation* widgetRepresentation();

  ///
  /// Get/Set the application logic.
  /// It must be set.
  void setAppLogic(vtkSlicerApplicationLogic* appLogic);
  vtkSlicerApplicationLogic* appLogic() const;

  ///
  /// This method allows to get a pointer to the ModuleLogic.
  /// If no moduleLogic already exists, one will be created calling
  /// 'createLogic' method.
  vtkSlicerLogic* logic();

  ///
  /// Return a pointer on the MRML scene
  vtkMRMLScene* mrmlScene() const;

  ///
  /// Returns true if the module is enabled.
  /// By default, a module is disabled
  bool isEnabled()const;

public slots:

  ///
  /// Enable/Disable the module
  virtual void setEnabled(bool enabled);

  ///
  /// Set the current MRML scene to the widget
  virtual void setMRMLScene(vtkMRMLScene*);

protected:
  ///
  /// All initialization code should be done in the setup
  virtual void setup() = 0;

  ///
  /// Create and return a widget representation for the module.
  virtual qSlicerAbstractModuleRepresentation* createWidgetRepresentation() = 0;

  ///
  /// Create and return the module logic
  /// Note: Only one instance of the logic will exist per module
  virtual vtkSlicerLogic* createLogic() = 0;


private:
  CTK_DECLARE_PRIVATE(qSlicerAbstractCoreModule);
  friend class qSlicerAbstractModuleRepresentation;
  void representationDeleted();
  ///
  /// Indicate if the module has already been initialized
  bool Initialized;
};

#endif