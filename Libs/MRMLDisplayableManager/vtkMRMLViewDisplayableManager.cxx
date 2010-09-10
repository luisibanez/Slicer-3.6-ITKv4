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

// MRMLDisplayableManager includes
#include "vtkMRMLViewDisplayableManager.h"
#include "vtkThreeDViewInteractorStyle.h"
#include "vtkMRMLCameraDisplayableManager.h"
#include "vtkMRMLDisplayableManagerGroup.h"

// MRML includes
#include <vtkMRMLCameraNode.h>
#include <vtkMRMLViewNode.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkBoundingBox.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkOutlineSource.h>
#include <vtkVectorText.h>
#include <vtkProperty.h>
#include <vtkMath.h>

// STD includes
#include <cassert>
#include <vector>

// Convenient macro
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLViewDisplayableManager );
vtkCxxRevisionMacro(vtkMRMLViewDisplayableManager, "$Revision: 13525 $");

//---------------------------------------------------------------------------
class vtkMRMLViewDisplayableManager::vtkInternal
{
public:
  vtkInternal(vtkMRMLViewDisplayableManager * external);
  ~vtkInternal();

  void CreateAxis();
  void AddAxis(vtkRenderer * renderer);
  void UpdateAxis(vtkRenderer * renderer, vtkMRMLViewNode * viewNode);

  void UpdateAxisVisibility();
  void UpdateAxisLabelVisibility();
  void SetAxisLabelColor(double newAxisLabelColor[3]);

  void UpdateRenderMode();

  void UpdateStereoType();

  void UpdateBackgroundColor();

  std::vector<vtkSmartPointer<vtkFollower> > AxisLabelActors;
  vtkSmartPointer<vtkActor>                  BoxAxisActor;
  vtkBoundingBox*                            BoxAxisBoundingBox;
  vtkMRMLViewDisplayableManager*             External;
};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLViewDisplayableManager::vtkInternal::vtkInternal(vtkMRMLViewDisplayableManager * external)
{
  this->External = external;
  this->BoxAxisBoundingBox = new vtkBoundingBox();
  this->CreateAxis();
}

//---------------------------------------------------------------------------
vtkMRMLViewDisplayableManager::vtkInternal::~vtkInternal()
{
  delete this->BoxAxisBoundingBox;
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::CreateAxis()
{
  // Create the default bounding box
  VTK_CREATE(vtkOutlineSource, boxSource);
  VTK_CREATE(vtkPolyDataMapper, boxMapper);
  boxMapper->SetInput(boxSource->GetOutput());
   
  this->BoxAxisActor = vtkSmartPointer<vtkActor>::New();
  this->BoxAxisActor->SetMapper(boxMapper);
  this->BoxAxisActor->SetScale(1.0, 1.0, 1.0);
  this->BoxAxisActor->GetProperty()->SetColor(1.0, 0.0, 1.0);
  this->BoxAxisActor->SetPickable(0);

  this->AxisLabelActors.clear();

  const char* labels[6] = {"R", "A", "S", "L", "P", "I"};

  for(int i = 0; i < 6; ++i)
    {
    VTK_CREATE(vtkVectorText, axisText);
    axisText->SetText(labels[i]);

    VTK_CREATE(vtkPolyDataMapper, axisMapper);
    axisMapper->SetInput(axisText->GetOutput());

    VTK_CREATE(vtkFollower, axisActor);
    axisActor->SetMapper(axisMapper);
    axisActor->SetPickable(0);
    this->AxisLabelActors.push_back(axisActor);

    axisActor->GetProperty()->SetColor(1, 1, 1); // White
    axisActor->GetProperty()->SetDiffuse(0.0);
    axisActor->GetProperty()->SetAmbient(1.0);
    axisActor->GetProperty()->SetSpecular(0.0);
    }
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::AddAxis(vtkRenderer * renderer)
{
  assert(renderer);

  renderer->AddViewProp(this->BoxAxisActor);

  for(std::size_t i = 0; i < this->AxisLabelActors.size(); ++i)
    {
    vtkFollower* actor = this->AxisLabelActors[i];
    renderer->AddViewProp(actor);
    }
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::UpdateAxis(vtkRenderer * renderer,
                                                            vtkMRMLViewNode * viewNode)
{
  assert(renderer);
  assert(renderer->IsActiveCameraCreated());
  assert(viewNode);

  // Turn off box and axis labels to compute bounds
  this->BoxAxisActor->VisibilityOff();

  for(std::size_t i = 0; i < this->AxisLabelActors.size(); ++i)
    {
    vtkFollower* actor = this->AxisLabelActors[i];
    actor->VisibilityOff();
    }

  // Compute bounds
  double bounds[6];
  renderer->ComputeVisiblePropBounds(bounds);

  // If there are no visible props, create a default set of bounds
  vtkBoundingBox newBBox;
  if (!vtkMath::AreBoundsInitialized(bounds))
    {
    newBBox.SetBounds(-100.0, 100.0,
                      -100.0, 100.0,
                      -100.0, 100.0);
    }
  else
    {
    newBBox.SetBounds(bounds);

    // Check for degenerate bounds
    double maxLength = newBBox.GetMaxLength();
    double minPoint[3], maxPoint[3];
    newBBox.GetMinPoint(minPoint[0], minPoint[1], minPoint[2]);
    newBBox.GetMaxPoint(maxPoint[0], maxPoint[1], maxPoint[2]);

    for (unsigned int i = 0; i < 3; i++)
      {
      if (newBBox.GetLength(i) == 0.0)
        {
        minPoint[i] = minPoint[i] - maxLength * .05;
        maxPoint[i] = maxPoint[i] + maxLength * .05;
        }
      }
    newBBox.SetMinPoint(minPoint);
    newBBox.SetMaxPoint(maxPoint);
    }

  // See if bounding box has changed. If not, no need to change the axis actors.
  bool bBoxChanged = false;
  if (newBBox != *(this->BoxAxisBoundingBox))
    {
    bBoxChanged = true;
    *(this->BoxAxisBoundingBox) = newBBox;

    double bounds[6];
    this->BoxAxisBoundingBox->GetBounds(bounds);

    VTK_CREATE(vtkOutlineSource, boxSource);
    boxSource->SetBounds(bounds);

    VTK_CREATE(vtkPolyDataMapper, boxMapper);
    boxMapper->SetInput(boxSource->GetOutput());

    this->BoxAxisActor->SetMapper(boxMapper);
    this->BoxAxisActor->SetScale(1.0, 1.0, 1.0);

    double letterSize = viewNode->GetLetterSize();

    for(std::size_t i = 0; i < this->AxisLabelActors.size(); ++i)
      {
      vtkFollower* actor = this->AxisLabelActors[i];
      actor->SetScale(
        this->BoxAxisBoundingBox->GetMaxLength() * letterSize,
        this->BoxAxisBoundingBox->GetMaxLength() * letterSize,
        this->BoxAxisBoundingBox->GetMaxLength() * letterSize);
      actor->SetOrigin(.5, .5,.5);
      }

    // Position the axis labels
    double center[3];
    this->BoxAxisBoundingBox->GetCenter(center);

    double offset = this->BoxAxisBoundingBox->GetMaxLength() * letterSize * 1.5;
    this->AxisLabelActors[0]->SetPosition(               // R
      bounds[1] + offset,
      center[1],
      center[2]);
    this->AxisLabelActors[1]->SetPosition(               // A
      center[0],
      bounds[3] + offset,
      center[2]);
    this->AxisLabelActors[2]->SetPosition(               // S
      center[0],
      center[1],
      bounds[5] + offset);

    this->AxisLabelActors[3]->SetPosition(               // L
      bounds[0] - offset,
      center[1],
      center[2]);
    this->AxisLabelActors[4]->SetPosition(               // P
      center[0],
      bounds[2] - offset,
      center[2]);
    this->AxisLabelActors[5]->SetPosition(               // I
      center[0],
      center[1],
      bounds[4] - offset);
    }

  // Update camera and make the axis visible again
  this->BoxAxisActor->VisibilityOn();
  for(std::size_t i = 0; i < this->AxisLabelActors.size(); ++i)
    {
    vtkFollower* actor = this->AxisLabelActors[i];
    actor->SetCamera(renderer->GetActiveCamera());
    actor->VisibilityOn();
    }

  // Until we come up with a solution for all use cases, the resetting
  // of the camera is disabled
#if 0
  if (bBoxChanged)
    {
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Dolly(1.5);
    renderer->ResetCameraClippingRange();
    }
#endif
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::UpdateAxisVisibility()
{
  bool visible = this->External->GetMRMLViewNode()->GetBoxVisible();
  vtkDebugWithObjectMacro(this->External, << "UpdateAxisVisibility:" << visible);
  this->BoxAxisActor->SetVisibility(visible);
  this->External->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::UpdateAxisLabelVisibility()
{
  bool visible = this->External->GetMRMLViewNode()->GetAxisLabelsVisible();
  vtkDebugWithObjectMacro(this->External, << "UpdateAxisLabelVisibility:" << visible);
  for(std::size_t i = 0; i < this->AxisLabelActors.size(); ++i)
    {
    vtkFollower* actor = this->AxisLabelActors[i];
    actor->SetVisibility(visible);
    }
  this->External->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::SetAxisLabelColor(double newAxisLabelColor[3])
{
  for(std::size_t i = 0; i < this->AxisLabelActors.size(); ++i)
    {
    vtkFollower* actor = this->AxisLabelActors[i];
    actor->GetProperty()->SetColor(newAxisLabelColor);
    }
  this->External->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::UpdateRenderMode()
{
  vtkDebugWithObjectMacro(this->External, << "UpdateRenderMode:" <<
                this->External->GetMRMLViewNode()->GetRenderMode());

  assert(this->External->GetRenderer()->IsActiveCameraCreated());

  vtkCamera *cam = this->External->GetRenderer()->GetActiveCamera();
  if (this->External->GetMRMLViewNode()->GetRenderMode() == vtkMRMLViewNode::Perspective)
    {
    cam->ParallelProjectionOff();
    }
  else if (this->External->GetMRMLViewNode()->GetRenderMode() == vtkMRMLViewNode::Orthographic)
    {
    cam->ParallelProjectionOn();
    cam->SetParallelScale(this->External->GetMRMLViewNode()->GetFieldOfView());
    }
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::UpdateStereoType()
{
  vtkDebugWithObjectMacro(this->External, << "UpdateStereoType:" <<
                this->External->GetMRMLViewNode()->GetStereoType());

  vtkRenderWindow * renderWindow = this->External->GetRenderer()->GetRenderWindow();
  int stereoType = this->External->GetMRMLViewNode()->GetStereoType();

  if (stereoType == vtkMRMLViewNode::RedBlue)
    {
    renderWindow->SetStereoTypeToRedBlue();
    }
  else if (stereoType == vtkMRMLViewNode::Anaglyph)
    {
    renderWindow->SetStereoTypeToAnaglyph();
    //renderWindow->SetAnaglyphColorSaturation(0.1);
    }
  else if (stereoType == vtkMRMLViewNode::CrystalEyes)
    {
    renderWindow->SetStereoTypeToCrystalEyes();
    }
  else if (stereoType == vtkMRMLViewNode::Interlaced)
    {
    renderWindow->SetStereoTypeToInterlaced();
    }

  renderWindow->SetStereoRender(stereoType != vtkMRMLViewNode::NoStereo);
  this->External->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::vtkInternal::UpdateBackgroundColor()
{
  double backgroundColor[3] = {0.0, 0.0, 0.0};
  this->External->GetMRMLViewNode()->GetBackgroundColor(backgroundColor);
  vtkDebugWithObjectMacro(this->External, << "UpdateBackgroundColor (" <<
                backgroundColor[0] << ", " << backgroundColor[1] << ", "
                << backgroundColor[2] << ")");
  this->External->GetRenderer()->SetBackground(backgroundColor);

  // If new background color is White, switch axis color label to black
  if (backgroundColor[0] == 1.0 && backgroundColor[1] == 1.0 && backgroundColor[2] == 1.0)
    {
    double black[3] = {0.0, 0.0, 0.0};
    this->SetAxisLabelColor(black);
    }
  else
    {
    double white[3] = {1.0, 1.0, 1.0};
    this->SetAxisLabelColor(white);
    }

  this->External->RequestRender();
}

//---------------------------------------------------------------------------
// vtkMRMLViewDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLViewDisplayableManager::vtkMRMLViewDisplayableManager()
{
  this->Internal = new vtkInternal(this);
}

//---------------------------------------------------------------------------
vtkMRMLViewDisplayableManager::~vtkMRMLViewDisplayableManager()
{
  delete this->Internal;
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::AdditionnalInitializeStep()
{
  this->AddMRMLDisplayableManagerEvent(vtkMRMLViewNode::RenderModeEvent);
  this->AddMRMLDisplayableManagerEvent(vtkMRMLViewNode::VisibilityEvent);
  this->AddMRMLDisplayableManagerEvent(vtkMRMLViewNode::StereoModeEvent);
  this->AddMRMLDisplayableManagerEvent(vtkMRMLViewNode::BackgroundColorEvent);
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::Create()
{
  assert(this->GetRenderer());
  assert(this->GetMRMLViewNode());

  this->Internal->AddAxis(this->GetRenderer());

  // CameraNodeDisplayableManager is expected to be instantiated !
  vtkMRMLCameraDisplayableManager * cameraDisplayableManager =
      vtkMRMLCameraDisplayableManager::SafeDownCast(
          this->GetDisplayableManagerGroup()->GetDisplayableManagerByClassName(
              "vtkMRMLCameraDisplayableManager"));
  assert(cameraDisplayableManager);

  // Listen for ActiveCameraChangedEvent
  cameraDisplayableManager->AddObserver(vtkMRMLCameraDisplayableManager::ActiveCameraChangedEvent,
                                        this->GetMRMLCallbackCommand());

  // If there is a active camera available, it means the vtkMRMLCameraDisplayableManager
  // has already been created and ActiveCameraChangedEvent already invoked.
  if (this->GetRenderer()->IsActiveCameraCreated())
    {
    this->Internal->UpdateAxis(this->GetRenderer(), this->GetMRMLViewNode());
    }
}

//---------------------------------------------------------------------------
void vtkMRMLViewDisplayableManager::ProcessMRMLEvents(vtkObject * caller,
                                                      unsigned long event,
                                                      void *callData)
{
  if (vtkMRMLCameraDisplayableManager::SafeDownCast(caller))
    {
    if (event == vtkMRMLCameraDisplayableManager::ActiveCameraChangedEvent)
      {
      vtkDebugMacro(<< "ProcessMRMLEvents - ActiveCameraChangedEvent");
      this->Internal->UpdateAxis(this->GetRenderer(), this->GetMRMLViewNode());
      }
    }
  else if(vtkMRMLViewNode::SafeDownCast(caller))
    {
    if (event == vtkMRMLViewNode::RenderModeEvent)
      {
      vtkDebugMacro(<< "ProcessMRMLEvents - RenderModeEvent");
      this->Internal->UpdateRenderMode();
      }
    else if (event == vtkMRMLViewNode::VisibilityEvent)
      {
      vtkDebugMacro(<< "ProcessMRMLEvents - VisibilityEvent");
      this->Internal->UpdateAxisLabelVisibility();
      this->Internal->UpdateAxisVisibility();
      }
    else if (event == vtkMRMLViewNode::StereoModeEvent)
      {
      vtkDebugMacro(<< "ProcessMRMLEvents - StereoModeEvent");
      this->Internal->UpdateStereoType();
      }
    else if (event == vtkMRMLViewNode::BackgroundColorEvent)
      {
      vtkDebugMacro(<< "ProcessMRMLEvents - BackgroundColorEvent");
      this->Internal->UpdateBackgroundColor();
      }
    }
  // Default MRML Event handler is NOT needed
//  else
//    {
//    this->Superclass::ProcessMRMLEvents(caller, event, callData);
//    }
}

