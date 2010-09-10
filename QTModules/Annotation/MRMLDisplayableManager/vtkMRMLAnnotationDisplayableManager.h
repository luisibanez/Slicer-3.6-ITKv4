/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

 Module:    $RCSfile: vtkMRMLAnnotationDisplayableManager.h,v $
 Date:      $Date: 2010/07/26 04:48:05 $
 Version:   $Revision: 1.2 $

 =========================================================================auto=*/

#ifndef __vtkMRMLAnnotationDisplayableManager_h
#define __vtkMRMLAnnotationDisplayableManager_h

// AnnotationModule includes
#include "qSlicerAnnotationModuleExport.h"
#include "vtkMRMLAnnotationClickCounter.h"

// MRMLDisplayableManager includes
#include <vtkMRMLAbstractDisplayableManager.h>
#include <vtkMRMLAbstractSliceViewDisplayableManager.h>
#include "MRMLDisplayableManager/vtkMRMLAnnotationDisplayableManagerHelper.h"

// VTK includes
#include <vtkSeedWidget.h>
#include <vtkHandleWidget.h>

class vtkMRMLAnnotationNode;
class vtkSlicerViewerWidget;
class vtkMRMLAnnotationDisplayNode;
class vtkMRMLAnnotationPointDisplayNode;
class vtkMRMLAnnotationLineDisplayNode;
class vtkAbstractWidget;

class Q_SLICER_QTMODULES_ANNOTATIONS_EXPORT vtkMRMLAnnotationDisplayableManager :
    public vtkMRMLAbstractDisplayableManager
{
public:

  static vtkMRMLAnnotationDisplayableManager *New();
  vtkTypeRevisionMacro(vtkMRMLAnnotationDisplayableManager, vtkMRMLAbstractDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ProcessMRMLEvents(vtkObject *caller, unsigned long event, void *callData);

  // the following functions must be public to be accessible by the callback
  /// Propagate properties of MRML node to widget.
  virtual void PropagateMRMLToWidget(vtkMRMLAnnotationNode* node, vtkAbstractWidget * widget);
  /// Propagate properties of widget to MRML node.
  virtual void PropagateWidgetToMRML(vtkAbstractWidget * widget, vtkMRMLAnnotationNode* node);


protected:

  vtkMRMLAnnotationDisplayableManager();
  virtual ~vtkMRMLAnnotationDisplayableManager();

  virtual void Create();

  /// Called after the corresponding MRML event is triggered, from AbstractDisplayableManager
  /// \sa ProcessMRMLEvents
  virtual void OnMRMLSceneAboutToBeClosedEvent();
  virtual void OnMRMLSceneClosedEvent();
  virtual void OnMRMLSceneAboutToBeImportedEvent();
  virtual void OnMRMLSceneImportedEvent();
  virtual void OnMRMLSceneNodeAddedEvent(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemovedEvent(vtkMRMLNode* node);

  /// Called after the corresponding MRML View container was modified
  virtual void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller);

  /// Handler for specific SliceView actions
  void OnMRMLSliceNodeModifiedEvent(vtkMRMLSliceNode * sliceNode);

  /// Observe all associated nodes.
  void SetAndObserveNodes();

  /// Preset functions for certain events.
  void OnMRMLAnnotationNodeModifiedEvent(vtkMRMLNode* node);
  void OnMRMLAnnotationNodeTransformModifiedEvent(vtkMRMLNode* node);
  void OnMRMLAnnotationNodeLockModifiedEvent(vtkMRMLNode* node);

  //
  // Handling of interaction within the RenderWindow
  //

  // Get the coordinates of a click in the RenderWindow
  void OnClickInRenderWindowGetCoordinates();
  /// Callback for click in RenderWindow
  virtual void OnClickInRenderWindow(double x, double y);
  /// Counter for clicks in Render Window
  vtkMRMLAnnotationClickCounter* m_ClickCounter;

  //
  // Seeds for widget placement
  //

  /// Place a seed for widgets
  virtual void PlaceSeed(double x, double y);
  /// Return the placed seeds
  vtkHandleWidget * GetSeed(int index);

  //
  // Widget functionality
  //

  /// Create a widget.
  virtual vtkAbstractWidget * CreateWidget(vtkMRMLAnnotationNode* node);
  /// Gets called when widget was created
  virtual void OnWidgetCreated(vtkAbstractWidget * widget, vtkMRMLAnnotationNode * node);
  /// Get the widget of a node.
  vtkAbstractWidget * GetWidget(vtkMRMLAnnotationNode * node);

  //
  // Coordinate Conversions
  //

  /// Convert display to world coordinates
  void GetDisplayToWorldCoordinates(double x, double y, double * worldCoordinates);

  /// Convert display to world coordinates
  void GetWorldToDisplayCoordinates(double r, double a, double s, double * displayCoordinates);
  void GetWorldToDisplayCoordinates(double * worldCoordinates, double * displayCoordinates);

  ///
  /// Check if it is the right displayManager
  virtual bool IsCorrectDisplayableManager();

  ///
  /// Focus of this displayableManager is set to a specific annotation type when inherited
  const char* m_Focus;

  ///
  /// Disable processing when updating is in progress.
  int m_Updating;

private:

  vtkMRMLAnnotationDisplayableManager(const vtkMRMLAnnotationDisplayableManager&); /// Not implemented
  void operator=(const vtkMRMLAnnotationDisplayableManager&); /// Not Implemented

  virtual void OnInteractorStyleEvent(int eventid);

  vtkMRMLAnnotationDisplayableManagerHelper * Helper;

  int m_DisableInteractorStyleEventsProcessing;

  bool m_SliceViewDisplayableManager;

};

#endif

