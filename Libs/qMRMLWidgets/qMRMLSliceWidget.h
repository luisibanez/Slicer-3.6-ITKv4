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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qMRMLSliceWidget_h
#define __qMRMLSliceWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

// qMRMLWidget includes
#include "qMRMLWidget.h"

#include "qMRMLWidgetsExport.h"


class ctkVTKSliceView;
class qMRMLSliceWidgetPrivate;
class qMRMLSliceControllerWidget;
class vtkCollection;
class vtkMRMLScene;
class vtkMRMLNode;
class vtkMRMLSliceLogic;
class vtkMRMLSliceNode;
class vtkMRMLSliceCompositeNode;

class vtkImageData;
class vtkInteractorObserver;

class QMRML_WIDGETS_EXPORT qMRMLSliceWidget : public qMRMLWidget
{
  Q_OBJECT
  Q_PROPERTY(QString sliceOrientation READ sliceOrientation WRITE setSliceOrientation)
public:
  /// Superclass typedef
  typedef qMRMLWidget Superclass;
  
  /// Constructors
  explicit qMRMLSliceWidget(QWidget* parent = 0);
  virtual ~qMRMLSliceWidget(){}

  /// Register DisplayableManagers
  /// \a scriptedDisplayableManagerDirectory is the based directory from which
  /// scripted DisplayableManager should be sourced from.
  void registerDisplayableManagers(const QString& scriptedDisplayableManagerDirectory);

  /// Get slice controller
  qMRMLSliceControllerWidget* sliceController()const;

  /// \sa qMRMLSliceControllerWidget::mrmlSliceNode()
  /// \sa setMRMLSliceNode()
  vtkMRMLSliceNode* mrmlSliceNode()const;

  // \sa qMRMLSliceControllerWidget::sliceLogic()
  vtkMRMLSliceLogic* sliceLogic()const;

  /// \sa qMRMLSliceControllerWidget::sliceOrientation()
  /// \sa setSliceOrientation()
  QString sliceOrientation()const;

  /// \sa qMRMLSliceControllerWidget::imageData()
  /// \sa setImageData();
  vtkImageData* imageData()const;

  /// \sa qMRMLSliceControllerWidget::mrmlSliceCompositeNode()
  vtkMRMLSliceCompositeNode* mrmlSliceCompositeNode()const;

  /// \sa qMRMLSliceControllerWidget::sliceViewName()
  /// \sa setSliceViewName()
  QString sliceViewName()const;

  /// \sa qMRMLSliceControllerWidget::sliceViewName()
  /// \sa sliceViewName()
  void setSliceViewName(const QString& newSliceViewName);

  /// Returns the interactor style of the view
  vtkInteractorObserver* interactorStyle()const;

  /// propagates the logics to the qMRMLSliceControllerWidget
  void setSliceLogics(vtkCollection* logics);

public slots:

  void setMRMLScene(vtkMRMLScene * newScene);

  /// \sa qMRMLSliceControllerWidget::setMRMLSliceNode()
  /// \sa mrmlSliceNode()
  void setMRMLSliceNode(vtkMRMLSliceNode* newSliceNode);

  /// \sa qMRMLSliceControllerWidget::setImageData()
  /// \sa imageData()
  void setImageData(vtkImageData* newImageData);
  
  /// \sa qMRMLSliceControllerWidget::setSliceOrientation()
  /// \sa sliceOrientation()
  void setSliceOrientation(const QString& orienation);

  /// Fit slices to background
  void fitSliceToBackground();

protected:

  /// Get a reference to the underlying Slice View
  ctkVTKSliceView * sliceView()const;

private:
  CTK_DECLARE_PRIVATE(qMRMLSliceWidget);
};

#endif
