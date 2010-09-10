/*=========================================================================

  Program:   Diffusion Applications
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/DiffusionApplications/ResampleDTI/itkDiffusionTensor3DTransform.h $
  Language:  C++
  Date:      $Date: 2010-03-15 18:00:34 -0400 (Mon, 15 Mar 2010) $
  Version:   $Revision: 12353 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkDiffusionTensor3DTransform_h
#define __itkDiffusionTensor3DTransform_h

#include <itkObject.h>
#include "itkDiffusionTensor3DExtended.h"
#include <itkPoint.h>
#include "itkMatrixExtended.h"
#include <itkNumericTraits.h>
#include <itkTransform.h>

namespace itk
{
/** \class DiffusionTensor3DTransform
 * 
 * This is an abstract class to represent the diffusion tensor transform
 */
template< class TData >
class DiffusionTensor3DTransform : public Object
{
public:
  typedef TData DataType ;
  typedef double TransformType ;
  typedef DiffusionTensor3DTransform Self ; 
  typedef Point< TransformType , 3 > PointType ;
  typedef DiffusionTensor3D< DataType > TensorDataType ;
  typedef DiffusionTensor3DExtended< DataType > InternalTensorDataType ;
  typedef Matrix< TransformType , 3 , 3 > MatrixTransformType ;
  typedef Matrix< DataType , 3 , 3 > MatrixDataType ;
  typedef MatrixExtended< TransformType , 3 , 3 > InternalMatrixTransformType ;
  typedef MatrixExtended< DataType , 3 , 3 > InternalMatrixDataType ;
  typedef SmartPointer< Self > Pointer ;
  typedef SmartPointer< const Self > ConstPointer ;
  ///Evaluate the position of the transformed tensor
  virtual PointType EvaluateTensorPosition( const PointType &point ) = 0 ;
  ///Evaluate the transformed tensor
  virtual TensorDataType EvaluateTransformedTensor(  TensorDataType &tensor ,
                                               PointType &outputPosition) = 0 ;
  ///Set the measurement frame of the tensor
  itkSetMacro( MeasurementFrame , MatrixTransformType ) ;
  virtual typename Transform< TransformType , 3 , 3 >::Pointer GetTransform( ) = 0 ;
protected:
  DiffusionTensor3DTransform() ;
  InternalMatrixTransformType m_MeasurementFrame ;
};

}//end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiffusionTensor3DTransform.txx"
#endif


#endif
