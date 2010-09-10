/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLXYPlotManagerNode.h"

#include <stdlib.h>
#include <iostream>

#include "TestingMacros.h"

int vtkMRMLXYPlotManagerNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLXYPlotManagerNode > node1 = vtkSmartPointer< vtkMRMLXYPlotManagerNode >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  EXERCISE_BASIC_MRML_METHODS(vtkMRMLXYPlotManagerNode, node1);
  
  return EXIT_SUCCESS;
}
