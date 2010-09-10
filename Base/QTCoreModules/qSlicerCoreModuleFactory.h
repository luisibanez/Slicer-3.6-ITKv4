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

#ifndef __qSlicerCoreModuleFactory_h
#define __qSlicerCoreModuleFactory_h

// CTK includes
#include <ctkPimpl.h>
#include <ctkAbstractQObjectFactory.h>

// SlicerQt includes
#include "qSlicerAbstractModule.h"
#include "qSlicerBaseQTCoreModulesExport.h"

class qSlicerCoreModuleFactoryPrivate;

class Q_SLICER_BASE_QTCOREMODULES_EXPORT qSlicerCoreModuleFactory :
  public ctkAbstractQObjectFactory<qSlicerAbstractCoreModule>
{
public:

  typedef ctkAbstractQObjectFactory<qSlicerAbstractCoreModule> Superclass;
  qSlicerCoreModuleFactory();
  virtual ~qSlicerCoreModuleFactory(){}

  ///
  virtual void registerItems();

  ///
  virtual QString objectNameToKey(const QString& objectName);

  /// Extract module name given a core module \a className
  /// For example: 
  ///  qSlicerCamerasModule -> cameras
  ///  qSlicerTransformsModule -> transforms
  static QString extractModuleName(const QString& className);

private:
  CTK_DECLARE_PRIVATE(qSlicerCoreModuleFactory);
};

#endif