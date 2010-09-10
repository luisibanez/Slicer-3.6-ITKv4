
// Qt includes
#include <QtPlugin>

// SlicerQt includes
#include <qSlicerCoreApplication.h>

// ModuleTemplate Logic includes
#include <vtkSlicerModuleTemplateLogic.h>

// ModuleTemplate includes
#include "qSlicerModuleTemplateModule.h"
#include "qSlicerModuleTemplateModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerModuleTemplateModule, qSlicerModuleTemplateModule);

//-----------------------------------------------------------------------------
class qSlicerModuleTemplateModulePrivate: public ctkPrivate<qSlicerModuleTemplateModule>
{
public:
};

//-----------------------------------------------------------------------------
qSlicerModuleTemplateModule::qSlicerModuleTemplateModule(QObject* _parent)
  :Superclass(_parent)
{
  CTK_INIT_PRIVATE(qSlicerModuleTemplateModule);
}

//-----------------------------------------------------------------------------
QString qSlicerModuleTemplateModule::helpText()const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerModuleTemplateModule::acknowledgementText()const
{
  return "This work was supported by ...";
}

//-----------------------------------------------------------------------------
void qSlicerModuleTemplateModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerModuleTemplateModule::createWidgetRepresentation()
{
  return new qSlicerModuleTemplateModuleWidget;
}

//-----------------------------------------------------------------------------
vtkSlicerLogic* qSlicerModuleTemplateModule::createLogic()
{
  return vtkSlicerModuleTemplateLogic::New();
}
