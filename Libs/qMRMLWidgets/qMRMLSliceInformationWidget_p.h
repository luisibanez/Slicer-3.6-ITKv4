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

#ifndef __qMRMLSliceInformationWidget_p_h
#define __qMRMLSliceInformationWidget_p_h

/// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

// qMRML includes
#include "qMRMLSliceInformationWidget.h"
#include "ui_qMRMLSliceInformationWidget.h"

// MRMLLogic includes
#include <vtkMRMLSliceLogic.h>

/// VTK includes
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkImageData.h>

class QAction;
class ctkVTKSliceView;
class vtkMRMLSliceNode;
class vtkObject;

//-----------------------------------------------------------------------------
class qMRMLSliceInformationWidgetPrivate: public QObject,
                                   public ctkPrivate<qMRMLSliceInformationWidget>,
                                   public Ui_qMRMLSliceInformationWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  CTK_DECLARE_PUBLIC(qMRMLSliceInformationWidget);
  qMRMLSliceInformationWidgetPrivate();
  ~qMRMLSliceInformationWidgetPrivate();

  void setupUi(qMRMLWidget* widget);

public slots:
  /// Update widget state using the associated MRML slice node
  void updateWidgetFromMRMLSliceNode();



public:
  vtkMRMLSliceNode*                   MRMLSliceNode;
  QButtonGroup*                       SliceSpacingModeGroup;
  
};

#endif
