#ifndef __qSlicerAnnotationModule_h
#define __qSlicerAnnotationModule_h

// SlicerQT includes
#include "qSlicerLoadableModule.h"

// CTK includes
#include <ctkPimpl.h>

#include "qSlicerAnnotationModuleExport.h"

class qSlicerAbstractModuleWidget;
class qSlicerAnnotationModulePrivate;

class Q_SLICER_QTMODULES_ANNOTATIONS_EXPORT qSlicerAnnotationModule :
  public qSlicerLoadableModule
{
  Q_OBJECT
  Q_INTERFACES(qSlicerLoadableModule)
public:

  typedef qSlicerLoadableModule Superclass;
  qSlicerAnnotationModule(QObject *parent=0);

  ///
  /// Return the help and acknowledgement text for the Annotation module.
  virtual QString helpText()const ;
  virtual QString acknowledgementText()const;

  ///
  /// Return the icon of the Annotation module.
  virtual QIcon icon()const;

  virtual ~qSlicerAnnotationModule(){}

  qSlicerGetTitleMacro(QTMODULE_TITLE);

protected:

  ///
  /// All initialization code should be done in the setup
  virtual void setup();

  // Description:
  // Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation* createWidgetRepresentation();

  // Description:
  // Create and return the logic associated to this module
  virtual vtkSlicerLogic* createLogic();

private:
  CTK_DECLARE_PRIVATE(qSlicerAnnotationModule);
};

#endif
