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

// Qt includes
#include <QDebug>
#include <QMenu>
#include <QActionGroup>

// CTK includes
#include <ctkLogger.h>
#include <ctkButtonGroup.h>

// qMRML includes
#include "qMRMLThreeDViewsControllerWidget.h"
#include "qMRMLThreeDViewsControllerWidget_p.h"
#include "qMRMLActionSignalMapper.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>


//--------------------------------------------------------------------------
static ctkLogger logger("org.slicer.libs.qmrmlwidgets.qMRMLThreeDViewsControllerWidget");
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// qMRMLThreeDViewsControllerWidgetPrivate methods

//---------------------------------------------------------------------------
qMRMLThreeDViewsControllerWidgetPrivate::qMRMLThreeDViewsControllerWidgetPrivate()
{
  this->ActiveMRMLThreeDViewNode = 0;
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::updateWidgetFromMRML()
{
  CTK_P(qMRMLThreeDViewsControllerWidget);
  Q_ASSERT(this->ActiveMRMLThreeDViewNode);

  p->setEnabled(this->ActiveMRMLThreeDViewNode != 0); // Enable/disable widget
  this->setOrthographicModeEnabled(
      this->ActiveMRMLThreeDViewNode->GetRenderMode() == vtkMRMLViewNode::Orthographic);
  this->setStereoType(this->ActiveMRMLThreeDViewNode->GetStereoType());
  this->set3DAxisVisible(this->ActiveMRMLThreeDViewNode->GetBoxVisible());
  this->set3DAxisLabelVisible(this->ActiveMRMLThreeDViewNode->GetAxisLabelsVisible());
  this->setAnimationMode(this->ActiveMRMLThreeDViewNode->GetAnimationMode());
}

//---------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setupUi(qMRMLWidget* widget)
{
  this->Ui_qMRMLThreeDViewsControllerWidget::setupUi(widget);

  // Pitch, Roll, Yaw buttons
  connect(this->PitchButton, SIGNAL(clicked()), SLOT(pitchActiveView()));
  connect(this->RollButton, SIGNAL(clicked()), SLOT(rollActiveView()));
  connect(this->YawButton, SIGNAL(clicked()), SLOT(yawActiveView()));

  // Orthographic/perspective button
  connect(this->OrthoButton, SIGNAL(toggled(bool)), SLOT(setOrthographicModeEnabled(bool)));

  // ZoomIn, ZoomOut button
  connect(this->ZoomInButton, SIGNAL(clicked()), SLOT(zoomIn()));
  connect(this->ZoomOutButton, SIGNAL(clicked()), SLOT(zoomOut()));

  // ResetFocalPoint button
  connect(this->CenterButton, SIGNAL(clicked()), SLOT(resetFocalPoint()));

  // StereoType actions
  this->StereoTypesMapper = new qMRMLActionSignalMapper(widget);
  this->StereoTypesMapper->setMapping(this->actionNoStereo,
                                      vtkMRMLViewNode::NoStereo);
  this->StereoTypesMapper->setMapping(this->actionSwitchToAnaglyphStereo,
                                      vtkMRMLViewNode::Anaglyph);
  this->StereoTypesMapper->setMapping(this->actionSwitchToCrystalEyesStereo,
                                      vtkMRMLViewNode::CrystalEyes);
  this->StereoTypesMapper->setMapping(this->actionSwitchToInterlacedStereo,
                                      vtkMRMLViewNode::Interlaced);
  this->StereoTypesMapper->setMapping(this->actionSwitchToRedBlueStereo,
                                      vtkMRMLViewNode::RedBlue);
  QActionGroup* stereoTypesActions = new QActionGroup(widget);
  stereoTypesActions->setExclusive(true);
  stereoTypesActions->addAction(this->actionNoStereo);
  stereoTypesActions->addAction(this->actionSwitchToRedBlueStereo);
  stereoTypesActions->addAction(this->actionSwitchToAnaglyphStereo);
  stereoTypesActions->addAction(this->actionSwitchToInterlacedStereo);
  //stereoTypesActions->addAction(this->actionSwitchToCrystalEyesStereo);
  QMenu* stereoTypesMenu = new QMenu("Stereo Modes", widget);
  stereoTypesMenu->addActions(stereoTypesActions->actions());
  this->StereoButton->setMenu(stereoTypesMenu);
  connect(this->StereoTypesMapper, SIGNAL(mapped(int)), SLOT(setStereoType(int)));
  connect(stereoTypesActions, SIGNAL(triggered(QAction*)),
          this->StereoTypesMapper, SLOT(map(QAction*)));

  QMenu* visibilityMenu = new QMenu("Visibility", widget);
  this->VisibilityButton->setMenu(visibilityMenu);

  // Show 3D Axis, 3D Axis label
  visibilityMenu->addAction(this->actionSet3DAxisVisible);
  visibilityMenu->addAction(this->actionSet3DAxisLabelVisible);
  connect(this->actionSet3DAxisVisible, SIGNAL(triggered(bool)),
          SLOT(set3DAxisVisible(bool)));
  connect(this->actionSet3DAxisLabelVisible, SIGNAL(triggered(bool)),
          SLOT(set3DAxisLabelVisible(bool)));

  // Background color
  QActionGroup* backgroundColorActions = new QActionGroup(widget);
  backgroundColorActions->setExclusive(true);
  visibilityMenu->addAction(this->actionSetLightBlueBackground);
  visibilityMenu->addAction(this->actionSetBlackBackground);
  visibilityMenu->addAction(this->actionSetWhiteBackground);
  backgroundColorActions->addAction(this->actionSetLightBlueBackground);
  backgroundColorActions->addAction(this->actionSetBlackBackground);
  backgroundColorActions->addAction(this->actionSetWhiteBackground);
  connect(this->actionSetLightBlueBackground, SIGNAL(triggered()), SLOT(setLightBlueBackground()));
  connect(this->actionSetWhiteBackground, SIGNAL(triggered()), SLOT(setWhiteBackground()));
  connect(this->actionSetBlackBackground, SIGNAL(triggered()), SLOT(setBlackBackground()));

  // SpinView, RockView buttons
  this->AnimateViewButtonGroup = new ctkButtonGroup(widget);
  this->AnimateViewButtonGroup->addButton(this->SpinButton, vtkMRMLViewNode::Spin);
  this->AnimateViewButtonGroup->addButton(this->RockButton, vtkMRMLViewNode::Rock);
  this->AnimateViewButtonGroup = new ctkButtonGroup(widget);
  this->AnimateViewButtonGroup->addButton(this->SpinButton);
  this->AnimateViewButtonGroup->addButton(this->RockButton);
  connect(this->SpinButton, SIGNAL(toggled(bool)), SLOT(onSpinViewButtonToggled(bool)));
  connect(this->RockButton, SIGNAL(toggled(bool)), SLOT(onRockViewButtonToggled(bool)));
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setOrthographicModeEnabled(bool enabled)
{
  this->OrthoButton->setChecked(enabled);
  this->ActiveMRMLThreeDViewNode->SetRenderMode(
      enabled ? vtkMRMLViewNode::Orthographic : vtkMRMLViewNode::Perspective);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::pitchActiveView()
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->ActiveMRMLThreeDViewNode->InvokeEvent(vtkMRMLViewNode::PitchViewRequestedEvent);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::rollActiveView()
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->ActiveMRMLThreeDViewNode->InvokeEvent(vtkMRMLViewNode::RollViewRequestedEvent);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::yawActiveView()
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->ActiveMRMLThreeDViewNode->InvokeEvent(vtkMRMLViewNode::YawViewRequestedEvent);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::zoomIn()
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->ActiveMRMLThreeDViewNode->InvokeEvent(vtkMRMLViewNode::ZoomInRequestedEvent);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::zoomOut()
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->ActiveMRMLThreeDViewNode->InvokeEvent(vtkMRMLViewNode::ZoomOutRequestedEvent);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::resetFocalPoint()
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->ActiveMRMLThreeDViewNode->InvokeEvent(vtkMRMLViewNode::ResetFocalPointRequestedEvent);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setStereoType(int newStereoType)
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }

  this->ActiveMRMLThreeDViewNode->SetStereoType(newStereoType);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::set3DAxisVisible(bool visible)
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->actionSet3DAxisVisible->setChecked(visible);
  this->ActiveMRMLThreeDViewNode->SetBoxVisible(visible);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::set3DAxisLabelVisible(bool visible)
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }
  this->actionSet3DAxisLabelVisible->setChecked(visible);
  this->ActiveMRMLThreeDViewNode->SetAxisLabelsVisible(visible);
}

// --------------------------------------------------------------------------
namespace
{
bool isWhiteColor(double color[3])
{
  return color[0] == 1.0 && color[1] == 1.0 && color[2] == 1.0;
}
bool isBlackColor(double color[3])
{
  return color[0] == 0.0 && color[1] == 0.0 && color[2] == 0.0;
}
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setLightBlueBackground()
{
  double lightBlue[3] = {0.70196, 0.70196, 0.90588};
  this->setBackgroundColor(lightBlue);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setBlackBackground()
{
  double black[3] = {0.0, 0.0, 0.0};
  this->setBackgroundColor(black);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setWhiteBackground()
{
  double white[3] = {1.0, 1.0, 1.0};
  this->setBackgroundColor(white);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setBackgroundColor(double newBackgroundColor[3])
{
  if (!this->ActiveMRMLThreeDViewNode)
    {
    return;
    }

  this->actionSetLightBlueBackground->setChecked(!isWhiteColor(newBackgroundColor)
                                                 && !isBlackColor(newBackgroundColor));
  this->actionSetWhiteBackground->setChecked(isWhiteColor(newBackgroundColor));
  this->actionSetBlackBackground->setChecked(isBlackColor(newBackgroundColor));

  this->ActiveMRMLThreeDViewNode->SetBackgroundColor(newBackgroundColor);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::onSpinViewButtonToggled(bool enabled)
{
  this->setAnimationMode(enabled ? vtkMRMLViewNode::Spin : vtkMRMLViewNode::Off);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::onRockViewButtonToggled(bool enabled)
{
  this->setAnimationMode(enabled ? vtkMRMLViewNode::Rock : vtkMRMLViewNode::Off);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidgetPrivate::setAnimationMode(int newAnimationMode)
{
  this->SpinButton->setChecked(newAnimationMode == vtkMRMLViewNode::Spin);
  this->RockButton->setChecked(newAnimationMode == vtkMRMLViewNode::Rock);
  this->ActiveMRMLThreeDViewNode->SetAnimationMode(newAnimationMode);
}

// --------------------------------------------------------------------------
// qMRMLThreeDViewsControllerWidget methods

// --------------------------------------------------------------------------
qMRMLThreeDViewsControllerWidget::qMRMLThreeDViewsControllerWidget(QWidget* _parent) : Superclass(_parent)
{
  CTK_INIT_PRIVATE(qMRMLThreeDViewsControllerWidget);
  CTK_D(qMRMLThreeDViewsControllerWidget);
  d->setupUi(this);
}

// --------------------------------------------------------------------------
void qMRMLThreeDViewsControllerWidget::setActiveMRMLThreeDViewNode(
    vtkMRMLViewNode * newActiveMRMLThreeDViewNode)
{
  CTK_D(qMRMLThreeDViewsControllerWidget);

  QList<int> events;
  events << vtkMRMLViewNode::AnimationModeEvent << vtkMRMLViewNode::StereoModeEvent
      << vtkMRMLViewNode::VisibilityEvent << vtkMRMLViewNode::RenderModeEvent;
  foreach(int event, events)
    {
    d->qvtkReconnect(d->ActiveMRMLThreeDViewNode, newActiveMRMLThreeDViewNode, event,
                     d, SLOT(updateWidgetFromMRML()));
    }

  d->ActiveMRMLThreeDViewNode = newActiveMRMLThreeDViewNode;

  d->updateWidgetFromMRML();
}

