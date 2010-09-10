/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

=========================================================================auto=*/

#ifndef __qSlicerBaseQTCorePythonQtDecorators_h
#define __qSlicerBaseQTCorePythonQtDecorators_h

// Qt includes
#include <QObject>

// CTK includes
#include <ctkAbstractPythonManager.h>

// VTK includes
#include <vtkInstantiator.h>
#include <vtkPythonUtil.h>
#include <vtkPython.h>

// PythonQT includes
#include <PythonQtObjectPtr.h>

// SlicerQt includes
#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerModuleFactoryManager.h"
#include "qSlicerAbstractCoreModule.h"

#include "qSlicerBaseQTCoreExport.h"

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

class Q_SLICER_BASE_QTCORE_EXPORT qSlicerBaseQTBasePythonQtDecorators : public QObject
{
  Q_OBJECT

public:

  qSlicerBaseQTBasePythonQtDecorators(ctkAbstractPythonManager* pythonManager)
    {
    Q_ASSERT(pythonManager);
    pythonManager->registerClassForPythonQt(&qSlicerCoreApplication::staticMetaObject);
    pythonManager->registerClassForPythonQt(&qSlicerModuleManager::staticMetaObject);
    pythonManager->registerClassForPythonQt(&qSlicerAbstractCoreModule::staticMetaObject);
    pythonManager->registerCPPClassForPythonQt("qSlicerModuleFactoryManager");
    }

public slots:

  // static methods
  QList<QWidget*> static_qSlicerCoreApplication_allWidgets()
    { 
    return qSlicerCoreApplication::allWidgets();
    }

  //----------------------------------------------------------------------------
  // qSlicerCoreApplication

  qSlicerModuleManager* moduleManager(qSlicerCoreApplication* app)const
    {
    return app->moduleManager();
    }

  void sendEvent(qSlicerCoreApplication* app, QObject* _receiver, QEvent* _event)
    {
    app->sendEvent(_receiver, _event);
    }

  void processEvents(qSlicerCoreApplication* app)
    {
    app->processEvents();
    }

  //----------------------------------------------------------------------------
  // qSlicerModuleFactoryManager

  QStringList moduleNames(qSlicerModuleFactoryManager* _factoryManager)
    {
    return _factoryManager->moduleNames();
    }
  

  //----------------------------------------------------------------------------
  // qSlicerModuleManager

  qSlicerModuleFactoryManager* factoryManager(qSlicerModuleManager* _moduleManager)
    {
    return _moduleManager->factoryManager();
    }
  
  /// 
  bool loadModule(qSlicerModuleManager* _moduleManager, const QString& name)
    {
    return _moduleManager->loadModule(name);
    }

  /// 
  bool unLoadModule(qSlicerModuleManager* _moduleManager, const QString& name)
    {
    return _moduleManager->unLoadModule(name);
    }

  /// 
  bool isLoaded(qSlicerModuleManager* _moduleManager, const QString& name)const
    {
    return _moduleManager->isLoaded(name);
    }

  /// 
  qSlicerAbstractCoreModule* module(qSlicerModuleManager* _moduleManager, const QString& name)
    {
    return _moduleManager->module(name);
    }
  
};

#endif
