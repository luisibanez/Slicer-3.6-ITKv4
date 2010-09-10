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

#ifndef __qMRMLWidget_h
#define __qMRMLWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "qMRMLWidgetsExport.h"

class vtkMRMLScene;
class qMRMLWidgetPrivate;

class QMRML_WIDGETS_EXPORT qMRMLWidget : public QWidget
{
  Q_OBJECT

public:

  typedef QWidget Superclass;
  explicit qMRMLWidget(QWidget *parent=0, Qt::WindowFlags f=0);
  virtual ~qMRMLWidget();

  /// 
  /// Return a pointer on the current MRML scene
  vtkMRMLScene* mrmlScene() const;

public slots:

  /// 
  /// Set the MRML \a scene associated with the widget
  virtual void setMRMLScene(vtkMRMLScene* scene);

signals:
  void mrmlSceneChanged(vtkMRMLScene*);

private:
  CTK_DECLARE_PRIVATE(qMRMLWidget);
};

#endif
