/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLPETProceduralColorNode.h"

#include <stdlib.h>
#include <iostream>

#include "TestingMacros.h"

int vtkMRMLPETProceduralColorNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLPETProceduralColorNode > node1 = vtkSmartPointer< vtkMRMLPETProceduralColorNode >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  EXERCISE_BASIC_TRANSFORMABLE_MRML_METHODS(vtkMRMLPETProceduralColorNode, node1);
  
  return EXIT_SUCCESS;
}
