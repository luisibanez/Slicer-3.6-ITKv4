/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLBSplineTransformNode.h"

#include <stdlib.h>
#include <iostream>

#include "TestingMacros.h"

int vtkMRMLBSplineTransformNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLBSplineTransformNode > node1 = vtkSmartPointer< vtkMRMLBSplineTransformNode >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  EXERCISE_BASIC_TRANSFORM_MRML_METHODS(vtkMRMLBSplineTransformNode, node1);
  
  return EXIT_SUCCESS;
}
