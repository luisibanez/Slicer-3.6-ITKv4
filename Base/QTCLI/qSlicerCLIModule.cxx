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

#include "qSlicerCLIModule.h"

// Qt includes
#include <QDebug>

// SlicerQT includes
#include "vtkSlicerCLIModuleLogic.h"
#include "qSlicerCLIModuleWidget.h"

// ModuleDescriptionParser includes
#include <ModuleDescription.h>
#include <ModuleDescriptionParser.h>

//-----------------------------------------------------------------------------
class qSlicerCLIModulePrivate: public ctkPrivate<qSlicerCLIModule>
{
public:
  CTK_DECLARE_PUBLIC(qSlicerCLIModule);
  typedef qSlicerCLIModulePrivate Self;
  qSlicerCLIModulePrivate();

  QString           Title;
  QString           Acknowledgement;
  QString           Help;
  QString           Category;
  QString           Contributor;
  QString           TempDirectory; 

  ModuleDescription                 Desc;
  ModuleProcessInformation*         ProcessInformation;
};

//-----------------------------------------------------------------------------
// qSlicerCLIModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCLIModulePrivate::qSlicerCLIModulePrivate()
{
  this->ProcessInformation = 0;
}

//-----------------------------------------------------------------------------
// qSlicerCLIModule methods

//-----------------------------------------------------------------------------
qSlicerCLIModule::qSlicerCLIModule(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(qSlicerCLIModule);
}

//-----------------------------------------------------------------------------
void qSlicerCLIModule::setup()
{
  CTK_D(qSlicerCLIModule);
  
  // Temporary directory should be set before the module is initialized
  Q_ASSERT(!d->TempDirectory.isEmpty());

  // Set temp directory 
  vtkSlicerCLIModuleLogic* myLogic = vtkSlicerCLIModuleLogic::SafeDownCast(this->logic());
  myLogic->SetTemporaryDirectory(d->TempDirectory.toLatin1());
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerCLIModule::createWidgetRepresentation()
{
  CTK_D(qSlicerCLIModule);
  return new qSlicerCLIModuleWidget(&d->Desc);
}

//-----------------------------------------------------------------------------
vtkSlicerLogic* qSlicerCLIModule::createLogic()
{
  return vtkSlicerCLIModuleLogic::New();
}

//-----------------------------------------------------------------------------
CTK_GET_CXX(qSlicerCLIModule, QString, title, Title);
CTK_GET_CXX(qSlicerCLIModule, QString, category, Category);
CTK_GET_CXX(qSlicerCLIModule, QString, contributor, Contributor);
CTK_GET_CXX(qSlicerCLIModule, QString, acknowledgementText, Acknowledgement);
CTK_GET_CXX(qSlicerCLIModule, QString, helpText, Help);
CTK_SET_CXX(qSlicerCLIModule, const QString&, setTempDirectory, TempDirectory);

//-----------------------------------------------------------------------------
void qSlicerCLIModule::setXmlModuleDescription(const char* xmlModuleDescription)
{
  CTK_D(qSlicerCLIModule);
  //qDebug() << "xmlModuleDescription:" << xmlModuleDescription;
  Q_ASSERT(!this->entryPoint().isEmpty());

  // Parse module description
  ModuleDescription desc;
  ModuleDescriptionParser parser;
  if (parser.Parse(std::string(xmlModuleDescription), desc) != 0)
    {
    qWarning() << "Failed to parse xml module description";
    return;
    }

  // Set properties
  d->Title = QString::fromStdString(desc.GetTitle());
  d->Acknowledgement = QString::fromStdString(desc.GetAcknowledgements());
  d->Contributor = QString::fromStdString(desc.GetContributor());
  d->Category = QString::fromStdString(desc.GetCategory());

  d->ProcessInformation = desc.GetProcessInformation();

  QString help =
    "%1<br>"
    "For more detailed documentation see:<br>"
    "%2";

  d->Help = help.arg(
    QString::fromStdString(desc.GetDescription())).arg(
    QString::fromStdString(desc.GetDocumentationURL()));

  // Set module type
  desc.SetType(this->moduleType().toStdString());
  
  // Set module entry point
  desc.SetTarget(this->entryPoint().toStdString());

  d->Desc = desc; 
}
