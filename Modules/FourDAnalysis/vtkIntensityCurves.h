/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkIntensityCurve.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.13 $

=========================================================================auto=*/

#ifndef __vtkIntensityCurves_h
#define __vtkIntensityCurves_h


#include "vtkObject.h"
#include "vtkFourDAnalysisWin32Header.h"

#include "vtkMRMLTimeSeriesBundleNode.h"
#include "vtkMRMLDoubleArrayNode.h"

class VTK_FourDAnalysis_EXPORT vtkIntensityCurves : public vtkObject
{
 public:
  //BTX
  enum {
    TYPE_MEAN,
    TYPE_MAX,
    TYPE_MIN
  };
  //ETX

 protected:
  //BTX
  typedef struct {
    int x;
    int y;
    int z;
  } CoordType;
  typedef std::vector<CoordType>         IndexTableType;
  typedef std::map<int, IndexTableType>  IndexTableMapType;  // IndexTableMapType[label]
  typedef std::map<int, vtkMRMLDoubleArrayNode*> IntensityCurveMapType;
  //ETX

 public:

  static vtkIntensityCurves *New();
  vtkTypeRevisionMacro(vtkIntensityCurves, vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro ( BundleNode, vtkMRMLTimeSeriesBundleNode );
  vtkSetObjectMacro ( BundleNode, vtkMRMLTimeSeriesBundleNode );
  vtkGetObjectMacro ( MaskNode,   vtkMRMLScalarVolumeNode );
  vtkSetObjectMacro ( MaskNode,   vtkMRMLScalarVolumeNode );
  vtkSetObjectMacro ( MRMLScene,  vtkMRMLScene );
  vtkGetObjectMacro ( MRMLScene,  vtkMRMLScene );

  vtkSetMacro ( ValueType,  int );
  vtkGetMacro ( ValueType,  int );

  //void            SetInterval(double interval) { this->Interval = interval; };
  int             Update();
  vtkIntArray*    GetLabelList();
  //vtkDoubleArray* GetCurve(int label);
  vtkMRMLDoubleArrayNode* GetCurve(int label);
  int             OutputDataInCSV(ostream& os, int label);

 protected:
  vtkIntensityCurves();
  virtual ~vtkIntensityCurves();

  //void   GenerateIntensityCurveMap();
  void   GenerateIntensityCurve();
  void   GenerateIndexMap(vtkImageData* mask, IndexTableMapType& indexTableMap);
  double GetMeanIntensity(vtkImageData* image, IndexTableType& indexTable);
  double GetSDIntensity(vtkImageData* image, double mean, IndexTableType& indexTable);
  void   GetMeanMaxMinIntensity(vtkImageData* image, IndexTableType& indexTable,
                                double& mean, double& max, double& min);
 private:

  vtkMRMLTimeSeriesBundleNode*     BundleNode;
  vtkMRMLScalarVolumeNode*         MaskNode;
  vtkMRMLTimeSeriesBundleNode*     PreviousBundleNode;
  vtkMRMLScalarVolumeNode*         PreviousMaskNode;
  IntensityCurveMapType            IntensityCurve;  // IntensityCurveMean[label]

  // Description:
  // CurveType specifies how to calculate the intensity value at given time point from
  // the  region. Can be one of TYPE_MEAN, TYPE_MAX and TYPE_MIN
  int                              ValueType;

  long                             PreviousUpdateTime;
  vtkMRMLScene*                    MRMLScene;

};


#endif //__vtkIntensityCurves_h
