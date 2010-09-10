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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerAbstractModulePanel_h
#define __qSlicerAbstractModulePanel_h

#include "qSlicerWidget.h"
#include "qSlicerBaseQTGUIExport.h"

class qSlicerAbstractModule;
class qSlicerAbstractModule; 

class Q_SLICER_BASE_QTGUI_EXPORT qSlicerAbstractModulePanel: public qSlicerWidget
{
  Q_OBJECT
public:
  qSlicerAbstractModulePanel(QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~qSlicerAbstractModulePanel();
  virtual void addModule(const QString& moduleName) = 0;
  virtual void removeModule(const QString& moduleName) = 0;
  virtual void removeAllModule() = 0;

signals:
  void moduleAdded(const QString& moduleName);
  //void moduleAboutToBeRemoved(const QString& moduleName);
  void moduleRemoved(const QString& moduleName);
};

#endif
