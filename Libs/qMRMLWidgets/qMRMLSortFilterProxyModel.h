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

#ifndef __qMRMLSortFilterProxyModel_h
#define __qMRMLSortFilterProxyModel_h

// Qt includes
#include <QSortFilterProxyModel>
#include <QStringList>

// CTK includes
#include <ctkPimpl.h>

// qMRML includes
#include "qMRMLWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLScene;
class qMRMLAbstractItemHelper;
class qMRMLSortFilterProxyModelPrivate;

class QMRML_WIDGETS_EXPORT qMRMLSortFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT
  Q_PROPERTY(QStringList nodeTypes READ nodeTypes WRITE setNodeTypes)
  Q_PROPERTY(bool showHidden READ showHidden WRITE setShowHidden)
  Q_PROPERTY(bool showChildNodeTypes READ showChildNodeTypes WRITE setShowChildNodeTypes)
  Q_PROPERTY(QStringList hideChildNodeTypes READ hideChildNodeTypes WRITE setHideChildNodeTypes)
public:
  typedef QSortFilterProxyModel Superclass;
  qMRMLSortFilterProxyModel(QObject *parent=0);
  virtual ~qMRMLSortFilterProxyModel();

  /// Retrive the associated vtkMRMLNode
  vtkMRMLScene* mrmlScene()const;

  /// Retrive the associated vtkMRMLNode
  vtkMRMLNode* mrmlNode(const QModelIndex& index)const;

  /// 
  /// Set/Get node types to display in the list
  /// NodeTypes are the class names, i.e. vtkMRMLViewNode, 
  /// vtkMRMLTransformNode
  QStringList nodeTypes()const;
  void setNodeTypes(const QStringList& nodeTypes);

  /// 
  /// If a vtkMRMLNode has the property HideFromEditors set to true,
  /// bypass the property and show the node anyway.
  void setShowHidden(bool);
  bool showHidden()const;

  ///
  /// Add node type attribute that filter the nodes to
  /// display
  void addAttribute(const QString& nodeType, 
                    const QString& attributeName,
                    const QVariant& attributeValue);

  ///
  /// Display or not the nodes that are excluded by
  /// the ExcludedChildNodeTypes list.
  /// true by default.
  void setShowChildNodeTypes(bool show);
  bool showChildNodeTypes()const;

  ///
  /// If a node is a nodeType, hide the node if it is also 
  /// a ExcludedChildNodeType. (this can happen if nodeType is a 
  /// mother class of ExcludedChildNodeType)
  void setHideChildNodeTypes(const QStringList& nodeTypes);
  QStringList hideChildNodeTypes()const;

  // TODO Add setMRMLScene()
protected:
  //virtual bool filterAcceptsColumn(int source_column, const QModelIndex & source_parent)const;
  virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent)const;
  virtual bool lessThan(const QModelIndex &left, const QModelIndex &right)const;
  
  qMRMLAbstractItemHelper* sourceItem(const QModelIndex& index)const;
private:
  CTK_DECLARE_PRIVATE(qMRMLSortFilterProxyModel);
};

#endif
