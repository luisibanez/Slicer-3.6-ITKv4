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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QDebug>

// qMRML includes
#include "qMRMLSceneFactoryWidget.h"
#include "qMRMLNodeFactory.h"
#include "ui_qMRMLSceneFactoryWidget.h"

// MRML includes 
#include <vtkMRMLScene.h>

class qMRMLSceneFactoryWidgetPrivate: public ctkPrivate<qMRMLSceneFactoryWidget>,
                                      public Ui_qMRMLSceneFactoryWidget
{
public:
  CTK_DECLARE_PUBLIC(qMRMLSceneFactoryWidget);
  qMRMLSceneFactoryWidgetPrivate();
  void init();
  void setNodeActionsEnabled(bool enable);
 
  vtkMRMLScene*  MRMLScene;
};

// --------------------------------------------------------------------------
qMRMLSceneFactoryWidgetPrivate::qMRMLSceneFactoryWidgetPrivate()
{
  this->MRMLScene = 0;
}

// --------------------------------------------------------------------------
void qMRMLSceneFactoryWidgetPrivate::init()
{
  CTK_P(qMRMLSceneFactoryWidget);
  this->setupUi(p);
  QObject::connect(this->NewSceneButton, SIGNAL(clicked()), p, SLOT(generateScene()));
  QObject::connect(this->DeleteSceneButton, SIGNAL(clicked()), p, SLOT(deleteScene()));
  QObject::connect(this->NewNodeButton, SIGNAL(clicked()), p, SLOT(generateNode()));
  QObject::connect(this->DeleteNodeButton, SIGNAL(clicked()), p, SLOT(deleteNode()));
}

// --------------------------------------------------------------------------
void qMRMLSceneFactoryWidgetPrivate::setNodeActionsEnabled(bool enable)
{
  this->NewNodeButton->setEnabled(enable);
  this->NewNodeLineEdit->setEnabled(enable);
  this->DeleteNodeButton->setEnabled(enable);
  this->DeleteNodeLineEdit->setEnabled(enable);
}

// --------------------------------------------------------------------------
// qMRMLSceneFactoryWidget methods

// --------------------------------------------------------------------------
qMRMLSceneFactoryWidget::qMRMLSceneFactoryWidget(QWidget* _parent)
  :QWidget(_parent)
{
  CTK_INIT_PRIVATE(qMRMLSceneFactoryWidget);
  ctk_d()->init();
}

// --------------------------------------------------------------------------
qMRMLSceneFactoryWidget::~qMRMLSceneFactoryWidget()
{
  this->deleteScene();
}

// --------------------------------------------------------------------------
void qMRMLSceneFactoryWidget::generateScene()
{
  CTK_D(qMRMLSceneFactoryWidget);
  
  if (d->MRMLScene)
    {
    d->MRMLScene->Delete();
    }
  d->MRMLScene = vtkMRMLScene::New();
  d->setNodeActionsEnabled(true);
  d->DeleteSceneButton->setEnabled(true);
  emit mrmlSceneChanged(d->MRMLScene);
}

// --------------------------------------------------------------------------
void qMRMLSceneFactoryWidget::deleteScene()
{
  CTK_D(qMRMLSceneFactoryWidget);
  if (!d->MRMLScene)
    {
    return;
    }
  d->setNodeActionsEnabled(false);
  d->DeleteSceneButton->setEnabled(false);
  emit this->mrmlSceneChanged(0);
  d->MRMLScene->Delete();
  d->MRMLScene = 0;
}

// --------------------------------------------------------------------------
vtkMRMLScene* qMRMLSceneFactoryWidget::mrmlScene()const
{
  CTK_D(const qMRMLSceneFactoryWidget);
  return d->MRMLScene;
}

// --------------------------------------------------------------------------
vtkMRMLNode* qMRMLSceneFactoryWidget::generateNode()
{
  CTK_D(qMRMLSceneFactoryWidget);
  Q_ASSERT(d->MRMLScene != 0);
  QString nodeClassName = d->NewNodeLineEdit->text();
  if (nodeClassName.isEmpty())
    {
    int numClasses = d->MRMLScene->GetNumberOfRegisteredNodeClasses();
    int classNumber = rand() % numClasses; 
    vtkMRMLNode* node = d->MRMLScene->GetNthRegisteredNodeClass(classNumber);
    Q_ASSERT(node);
    while (node->GetSingletonTag())
      {
      classNumber = rand() % numClasses;
      node = d->MRMLScene->GetNthRegisteredNodeClass(classNumber);
      Q_ASSERT(node);
      }
    nodeClassName = QLatin1String(node->GetClassName()); 
    if (nodeClassName.isEmpty())
      {
      qWarning() << "Class registered (#" << classNumber << "):"
                 << node << " has an empty classname";
      }
    }
  return this->generateNode(nodeClassName);
}

// --------------------------------------------------------------------------
vtkMRMLNode* qMRMLSceneFactoryWidget::generateNode(const QString& className)
{
  CTK_D(qMRMLSceneFactoryWidget);
  Q_ASSERT(!className.isEmpty());
  Q_ASSERT(d->MRMLScene != 0);
  vtkMRMLNode* node = qMRMLNodeFactory::createNode(d->MRMLScene, className);
  Q_ASSERT(node);
  if (node)
    {
    emit mrmlNodeAdded(node);
    }
  return node;
}

// --------------------------------------------------------------------------
void qMRMLSceneFactoryWidget::deleteNode()
{
  CTK_D(qMRMLSceneFactoryWidget);
  Q_ASSERT(d->MRMLScene != 0);
  QString nodeClassName = d->DeleteNodeLineEdit->text();
  if (!nodeClassName.isEmpty())
    {
    this->deleteNode(nodeClassName);
    return;
    }
  int numNodes = d->MRMLScene->GetNumberOfNodes();
  if (numNodes == 0)
    {
    return;
    }
  vtkMRMLNode* node = d->MRMLScene->GetNthNode(rand() % numNodes);
  d->MRMLScene->RemoveNode(node);
  // FIXME: disable delete button when there is no more nodes in the scene to delete
  emit mrmlNodeRemoved(node);
}

// --------------------------------------------------------------------------
void qMRMLSceneFactoryWidget::deleteNode(const QString& className)
{
  CTK_D(qMRMLSceneFactoryWidget);
  Q_ASSERT(!className.isEmpty());
  Q_ASSERT(d->MRMLScene != 0);
  int numNodes = d->MRMLScene->GetNumberOfNodesByClass(className.toLatin1().data());
  if (numNodes == 0)
    {
    qDebug() << "qMRMLSceneFactoryWidget::deleteNode(" <<className <<") no node";
    return;
    }
  vtkMRMLNode* node = d->MRMLScene->GetNthNodeByClass(rand() % numNodes, className.toLatin1().data());
  qDebug() << "qMRMLSceneFactoryWidget::deleteNode(" <<className <<") ==" << node->GetClassName();
  d->MRMLScene->RemoveNode(node);
  // FIXME: disable delete button when there is no more nodes in the scene to delete
  emit mrmlNodeRemoved(node);
}
