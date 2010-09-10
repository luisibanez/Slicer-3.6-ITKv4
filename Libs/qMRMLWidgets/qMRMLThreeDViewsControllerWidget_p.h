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

#ifndef __qMRMLThreeDViewsControllerWidget_p_h
#define __qMRMLThreeDViewsControllerWidget_p_h

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

// qMRML includes
#include "qMRMLThreeDViewsControllerWidget.h"
#include "ui_qMRMLThreeDViewsControllerWidget.h"

// VTK includes
#include <vtkWeakPointer.h>

class QAction;
class ctkButtonGroup;
class qMRMLActionSignalMapper;
class vtkMRMLViewNode;

//-----------------------------------------------------------------------------
class qMRMLThreeDViewsControllerWidgetPrivate: public QObject,
                                   public ctkPrivate<qMRMLThreeDViewsControllerWidget>,
                                   public Ui_qMRMLThreeDViewsControllerWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:

  CTK_DECLARE_PUBLIC(qMRMLThreeDViewsControllerWidget);
  qMRMLThreeDViewsControllerWidgetPrivate();
  ~qMRMLThreeDViewsControllerWidgetPrivate(){}

  void setupUi(qMRMLWidget* widget);

public slots:

  void updateWidgetFromMRML();

  void setOrthographicModeEnabled(bool enabled);

  void pitchActiveView();
  void rollActiveView();
  void yawActiveView();

  void zoomIn();
  void zoomOut();

  void resetFocalPoint();

  void setStereoType(int newStereoType);

  void set3DAxisVisible(bool visible);
  void set3DAxisLabelVisible(bool visible);

  void setLightBlueBackground();
  void setBlackBackground();
  void setWhiteBackground();
  void setBackgroundColor(double newBackgroundColor[3]);

  void onSpinViewButtonToggled(bool enabled);
  void onRockViewButtonToggled(bool enabled);
  void setAnimationMode(int newAnimationMode);

public:

  // TODO In LayoutManager, use GetActive/IsActive flag ...
  vtkWeakPointer<vtkMRMLViewNode>  ActiveMRMLThreeDViewNode;

  qMRMLActionSignalMapper*         StereoTypesMapper;
  ctkButtonGroup*                  AnimateViewButtonGroup;
  
};

#endif
