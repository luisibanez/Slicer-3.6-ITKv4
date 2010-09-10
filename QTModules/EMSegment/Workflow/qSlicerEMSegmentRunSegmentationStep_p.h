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

  This file was originally developed by Danielle Pace, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerEMSegmentRunSegmentationStep_p_h
#define __qSlicerEMSegmentRunSegmentationStep_p_h

// EMSegment includes
#include "qSlicerEMSegmentRunSegmentationStep.h"
#include "ui_qSlicerEMSegmentRunSegmentationStep.h"

// EMSegment/MRML includes
class vtkMRMLROINode;

//-----------------------------------------------------------------------------
class qSlicerEMSegmentRunSegmentationStepPrivate : public QObject,
                                                   public ctkPrivate<qSlicerEMSegmentRunSegmentationStep>,
                                                   public Ui_qSlicerEMSegmentRunSegmentationStep
{
  Q_OBJECT
  CTK_DECLARE_PUBLIC(qSlicerEMSegmentRunSegmentationStep)

public:
  qSlicerEMSegmentRunSegmentationStepPrivate();

  void setupUi(qSlicerEMSegmentWorkflowWidgetStep *step);

  void setMRMLROINode(vtkMRMLROINode* newROINode);

  // Delete any ROI nodes named "SegmentationROI", and create a new ROI node
  vtkMRMLROINode* createROINode();

  // Propagate changes in ROINode MRML to EMSegmentRunSegmentationStep ROI MRML
  void updateMRMLROINodeUsingInputVolume();

  void updateWidgetFromMRML();

  void updateMRMLFromWidget();

  vtkMRMLROINode* ROINode;

private slots:

  void display2DVOI(bool show);

};

#endif
