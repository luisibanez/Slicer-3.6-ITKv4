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

#ifndef __qSlicerMeasurementsAngleWidget_h
#define __qSlicerMeasurementsAngleWidget_h 

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerWidget.h"

#include "qSlicerMeasurementsModuleExport.h"

class qSlicerMeasurementsAngleWidgetPrivate;

class Q_SLICER_QTMODULES_MEASUREMENTS_EXPORT qSlicerMeasurementsAngleWidget : public qSlicerWidget
{ 
  Q_OBJECT

public:

  typedef qSlicerWidget Superclass;
  qSlicerMeasurementsAngleWidget(QWidget *parent=0);
  
  virtual void printAdditionalInfo();

private:
  CTK_DECLARE_PRIVATE(qSlicerMeasurementsAngleWidget);
};

#endif
