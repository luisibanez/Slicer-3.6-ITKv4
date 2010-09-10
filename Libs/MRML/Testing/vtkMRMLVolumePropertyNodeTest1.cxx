/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLVolumePropertyNode.h"

#include <stdlib.h>
#include <iostream>

#include "TestingMacros.h"

int vtkMRMLVolumePropertyNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLVolumePropertyNode > node1 = vtkSmartPointer< vtkMRMLVolumePropertyNode >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  EXERCISE_BASIC_STORABLE_MRML_METHODS(vtkMRMLVolumePropertyNode, node1);

  return EXIT_SUCCESS;
}
