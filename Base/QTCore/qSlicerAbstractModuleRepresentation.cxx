/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH)
 All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

=========================================================================auto=*/

// SlicerQt includes
#include "qSlicerAbstractCoreModule.h"
#include "qSlicerAbstractModuleRepresentation.h"

// Slicer includes
#include "vtkSlicerLogic.h"

// VTK includes
#include <vtkWeakPointer.h>

//-----------------------------------------------------------------------------
class qSlicerAbstractModuleRepresentationPrivate: public ctkPrivate<qSlicerAbstractModuleRepresentation>
{
public:
  qSlicerAbstractModuleRepresentationPrivate();
  vtkWeakPointer<vtkSlicerLogic>  Logic;

private:
  CTK_DECLARE_PUBLIC(qSlicerAbstractModuleRepresentation);
  qSlicerAbstractCoreModule*          Module;
};

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentationPrivate
::qSlicerAbstractModuleRepresentationPrivate()
{
  this->Module = 0;
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation::qSlicerAbstractModuleRepresentation()
{
  CTK_INIT_PRIVATE(qSlicerAbstractModuleRepresentation);
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation::~qSlicerAbstractModuleRepresentation()
{
  CTK_D(qSlicerAbstractModuleRepresentation);
  if (d->Module)
    {
    d->Module->representationDeleted();
    }
}

//-----------------------------------------------------------------------------
QAction* qSlicerAbstractModuleRepresentation::createAction()
{
  return 0;
}

//-----------------------------------------------------------------------------
QString qSlicerAbstractModuleRepresentation::moduleName()const
{
  CTK_D(const qSlicerAbstractModuleRepresentation);
  return d->Module->name();
}

//-----------------------------------------------------------------------------
vtkSlicerLogic* qSlicerAbstractModuleRepresentation::logic()const
{
  CTK_D(const qSlicerAbstractModuleRepresentation);
  return d->Logic.GetPointer();
}

//-----------------------------------------------------------------------------
const qSlicerAbstractCoreModule* qSlicerAbstractModuleRepresentation::module()const
{
  CTK_D(const qSlicerAbstractModuleRepresentation);
  return d->Module;
}

//-----------------------------------------------------------------------------
void qSlicerAbstractModuleRepresentation::setModule(qSlicerAbstractCoreModule* module)
{
  CTK_D(qSlicerAbstractModuleRepresentation);
  d->Module = module;
  d->Logic = module ? module->logic() : 0;
}
