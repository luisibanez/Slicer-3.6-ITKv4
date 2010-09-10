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

  This file was originally developed by
    Danielle Pace and Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QVBoxLayout>

// CTK includes
#include <ctkWorkflow.h>
#include <ctkWorkflowWidget.h>

// EMSegment includes
#include "qSlicerEMSegmentWorkflowWidgetStep.h"
#include "qSlicerEMSegmentWorkflowWidget.h"

// EMSegment/Logic includes
#include <vtkSlicerEMSegmentLogic.h>

// EMSegment/MRML includes
#include <vtkEMSegmentMRMLManager.h>

//-----------------------------------------------------------------------------
class qSlicerEMSegmentWorkflowWidgetStepPrivate : public ctkPrivate<qSlicerEMSegmentWorkflowWidgetStep>
{
public:
  qSlicerEMSegmentWorkflowWidgetStepPrivate();

  vtkEMSegmentMRMLManager *        MRMLManager;
  vtkSlicerEMSegmentLogic *        EMSegmentLogic;
};

//-----------------------------------------------------------------------------
// qSlicerEMSegmentWorkflowWidgetStepPrivate methods

//-----------------------------------------------------------------------------
qSlicerEMSegmentWorkflowWidgetStepPrivate::qSlicerEMSegmentWorkflowWidgetStepPrivate()
{
  this->MRMLManager = 0;
  this->EMSegmentLogic = 0;
}

//-----------------------------------------------------------------------------
// qSlicerEMSegmentWorkflowWidgetStep methods

//-----------------------------------------------------------------------------
qSlicerEMSegmentWorkflowWidgetStep::qSlicerEMSegmentWorkflowWidgetStep(
  ctkWorkflow* newWorkflow, const QString& newId, QWidget* newParent) : 
Superclass(newWorkflow, newId, newParent)
{
  CTK_INIT_PRIVATE(qSlicerEMSegmentWorkflowWidgetStep);
}

//-----------------------------------------------------------------------------
qSlicerEMSegmentWorkflowWidgetStep::qSlicerEMSegmentWorkflowWidgetStep(QWidget* newParent) : 
Superclass(newParent)
{
  CTK_INIT_PRIVATE(qSlicerEMSegmentWorkflowWidgetStep);
}

//-----------------------------------------------------------------------------
CTK_GET_CXX(qSlicerEMSegmentWorkflowWidgetStep, vtkEMSegmentMRMLManager*, mrmlManager, MRMLManager);
CTK_GET_CXX(qSlicerEMSegmentWorkflowWidgetStep, vtkSlicerEMSegmentLogic*, emSegmentLogic, EMSegmentLogic);
CTK_SET_CXX(qSlicerEMSegmentWorkflowWidgetStep, vtkSlicerEMSegmentLogic*, setEMSegmentLogic, EMSegmentLogic);

//-----------------------------------------------------------------------------
vtkMRMLScene * qSlicerEMSegmentWorkflowWidgetStep::mrmlScene()const
{
  CTK_D(const qSlicerEMSegmentWorkflowWidgetStep);
  return d->MRMLManager ? d->MRMLManager->GetMRMLScene() : 0;
}

//-----------------------------------------------------------------------------
void qSlicerEMSegmentWorkflowWidgetStep::setMRMLManager(vtkEMSegmentMRMLManager * newMRMLManager)
{
  CTK_D(qSlicerEMSegmentWorkflowWidgetStep);
  if (d->MRMLManager == newMRMLManager)
    {
    return;
    }

  d->MRMLManager = newMRMLManager;

  emit mrmlSceneChanged(newMRMLManager ? newMRMLManager->GetMRMLScene() : 0);

  emit mrmlManagerChanged(newMRMLManager);
}
