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

#ifndef __qMRMLThreeDView_p_h
#define __qMRMLThreeDView_p_h

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

// qMRML includes
#include "qMRMLThreeDView.h"

class vtkMRMLDisplayableManagerGroup;
class vtkMRMLViewNode;
class vtkMRMLCameraNode;
class vtkObject;

//-----------------------------------------------------------------------------
class qMRMLThreeDViewPrivate: public QObject, public ctkPrivate<qMRMLThreeDView>
{
  Q_OBJECT
  QVTK_OBJECT
public:
  CTK_DECLARE_PUBLIC(qMRMLThreeDView);
  qMRMLThreeDViewPrivate();
  ~qMRMLThreeDViewPrivate();

  void setMRMLScene(vtkMRMLScene* scene);

  /// Loop over all CameraNode from the scene and return the one having
  /// its activeTag matching \a viewNode ID
//  vtkMRMLCameraNode* lookUpMRMLCameraNode(vtkMRMLViewNode* viewNode);

public slots:
  /// Handle MRML scene event
  void onSceneAboutToBeClosedEvent();
  void onSceneAboutToBeImportedEvent();
  void onSceneImportedEvent();
  //  void onSceneRestoredEvent();

  void onMRMLViewNodeModifiedEvent();

  void onResetFocalPointRequestedEvent();

  void onAnimationModeEvent();
  void rockView();

public:

  vtkMRMLDisplayableManagerGroup*    DisplayableManagerGroup;
  vtkMRMLScene*                      MRMLScene;
  vtkMRMLViewNode*                   MRMLViewNode;
};

#endif
