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

// SlicerQt includes
#include "qSlicerCamerasModule.h"
#include "qSlicerCamerasModuleWidget.h"

//-----------------------------------------------------------------------------
class qSlicerCamerasModulePrivate: public ctkPrivate<qSlicerCamerasModule>
{
public:
  CTK_DECLARE_PUBLIC(qSlicerCamerasModule);
};

//-----------------------------------------------------------------------------
CTK_CONSTRUCTOR_1_ARG_CXX(qSlicerCamerasModule, QObject*);

//-----------------------------------------------------------------------------
QString qSlicerCamerasModule::category()const
{
  return "Developer Tools";
}

//-----------------------------------------------------------------------------
QIcon qSlicerCamerasModule::icon()const
{
  return QIcon(":/Icons/Cameras.png");
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerCamerasModule::createWidgetRepresentation()
{
  return new qSlicerCamerasModuleWidget;
}

//-----------------------------------------------------------------------------
vtkSlicerLogic* qSlicerCamerasModule::createLogic()
{
  return 0;
}

//-----------------------------------------------------------------------------
QString qSlicerCamerasModule::helpText()const
{
  QString help =
    "Create new views and cameras.<br>"
    "The view pulldown menu below can be used to create new views and select "
    "the active view. Switch the layout to \"Tabbed 3D Layout\" from the "
    "layout icon in the toolbar to access multiple views. The view selected in "
    "\"Tabbed 3D Layout\" becomes the active view and replaces the 3D view in "
    "all other layouts. The camera pulldown menu below can be used to set the "
    "active camera for the selected view. <br>"
    "WARNING: this is rather experimental at the moment (fiducials, IO/data, "
    "closing the scene are probably broken for new views).";
  return help;
}

//-----------------------------------------------------------------------------
QString qSlicerCamerasModule::acknowledgementText()const
{
  QString about =
    "To be updated %1";

  return about.arg(this->slicerWikiUrl());
}