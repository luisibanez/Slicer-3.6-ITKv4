/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/SparseFieldLevelSetContour.cxx $
  Language:  C++
  Date:      $Date: 2010-07-29 14:35:25 -0400 (Thu, 29 Jul 2010) $
  Version:   $Revision: 14386 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#include "SparseFieldLevelSetContourCLP.h"
#include <iostream>
#include <vector>
#include "vtkXMLPolyDataReader.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkPluginFilterWatcher.h"
#include "ModuleEntry.h"

using namespace MeshContourEvolver;

int main(int argc, char* argv[] )
{
  std::cout<<"Starting...\n";
  PARSE_ARGS;

  std::cout<<"Output model: "<<OutputModel.c_str()<<"\n";
  std::cout<<"Length of contour seeds: "<<ContourSeedPts.size()<<"\n";
  std::cout << "Evolution iterations: " << evolve_its<<"\n";
  std::cout << "Mesh smoothing iterations: " << mesh_smooth_its<<"\n";
  std::cout << "Curvature averaging iterations: " << H_smooth_its << "\n";
  std::cout << "Adjacency tree levels " << adj_levels << "\n";
  std::cout << "Right handed mesh: " << rightHandMesh << "\n";
  
  vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
  std::string comment = "Reading input model " + InputSurface;
  vtkPluginFilterWatcher watchReader(reader,
                                     comment.c_str(),
                                     CLPProcessInformation);
  reader->SetFileName(InputSurface.c_str());
  reader->Update();

  InitParam init = {evolve_its, mesh_smooth_its, H_smooth_its, adj_levels, rightHandMesh };


  if (reader->GetOutput() == NULL)
    {
    std::cerr << "ERROR reading input surface file " << InputSurface.c_str();
    reader->Delete();
    return EXIT_FAILURE;
    }
  vtkSmartPointer<vtkPolyData> polyDataInput  = reader->GetOutput();

  vtkSmartPointer<vtkPolyData> polyDataOutput = vtkSmartPointer<vtkPolyData>::New();
  
  //vtkPolyData * polyDataOutput =
  entry_main( polyDataInput, ContourSeedPts, polyDataOutput, init );

  vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  std::string commentWrite = "Writing output model " + OutputModel;
  vtkPluginFilterWatcher watchWriter(writer,
                                     commentWrite.c_str(),
                                     CLPProcessInformation);
  polyDataOutput->Update( );
  writer->SetIdTypeToInt32();
  writer->SetInput( polyDataOutput );
  writer->SetFileName( OutputModel.c_str() );
  
  writer->Update( );
  writer->Write();

  // The result is contained in the scalar colormap of the output.

  return EXIT_SUCCESS;
}
