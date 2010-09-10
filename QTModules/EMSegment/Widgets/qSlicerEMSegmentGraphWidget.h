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

#ifndef __qSlicerEMSegmentGraphWidget_h
#define __qSlicerEMSegmentGraphWidget_h

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerEMSegmentWidget.h"

#include "qSlicerEMSegmentModuleExport.h"

class qSlicerEMSegmentGraphWidgetPrivate;

class Q_SLICER_QTMODULES_EMSEGMENT_EXPORT qSlicerEMSegmentGraphWidget : public qSlicerEMSegmentWidget
{
  Q_OBJECT

public:
  typedef qSlicerEMSegmentWidget Superclass;
  explicit qSlicerEMSegmentGraphWidget(QWidget * newParent=0);

public slots:
  virtual void setMRMLManager(vtkEMSegmentMRMLManager* manager);
  void updateFromMRMLManager();

protected slots:
  void onCurrentInput0VolumeChanged(const QString& volumeName);
  void onCurrentInput1VolumeChanged(const QString& volumeName);
  void onClassVisibilityToggled(bool);

private:
  CTK_DECLARE_PRIVATE(qSlicerEMSegmentGraphWidget);
};

#endif