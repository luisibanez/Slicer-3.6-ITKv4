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

/// Qt includes
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QUrl>

/// CTK includes
#include <ctkCheckableHeaderView.h>
#include <ctkDirectoryButton.h>

/// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerSaveDataDialog.h"
#include "qSlicerSaveDataDialog_p.h"
#include "qSlicerIOManager.h"
#include "qSlicerIOOptionsWidget.h"

/// Logic includes
#include "vtkMRMLLogic.h"

/// MRML includes
#include <vtkDataFileFormatHelper.h> // for GetFileExtensionFromFormatString()
#include <vtkMRMLScene.h>
#include <vtkMRMLStorableNode.h>
#include <vtkMRMLStorageNode.h>
#include <vtkMRMLSceneSnapshotNode.h>

/// VTK includes
#include <vtkStringArray.h>

//-----------------------------------------------------------------------------
qSlicerSaveDataDialogPrivate::qSlicerSaveDataDialogPrivate(QWidget* _parent)
  :QDialog(_parent)
{
  this->MRMLScene = 0;

  this->setupUi(this);

  // checkable headers.
  this->FileWidget->model()->setHeaderData(NodeNameColumn, Qt::Horizontal,
                                           Qt::Unchecked, Qt::CheckStateRole);
  QHeaderView* previousHeaderView = this->FileWidget->horizontalHeader();
  ctkCheckableHeaderView* headerView = new ctkCheckableHeaderView(Qt::Horizontal, this->FileWidget);
  headerView->setClickable(previousHeaderView->isClickable());
  headerView->setMovable(previousHeaderView->isMovable());
  headerView->setHighlightSections(previousHeaderView->highlightSections());
  headerView->setStretchLastSection(previousHeaderView->stretchLastSection());
  headerView->setPropagateToItems(true);
  this->FileWidget->setHorizontalHeader(headerView);

  connect(this->DirectoryButton, SIGNAL(directoryChanged(const QString&)),
          this, SLOT(setDirectory(const QString&)));
  connect(this->SelectSceneDataButton, SIGNAL(clicked()), this, SLOT(selectModifiedSceneData()));
  connect(this->SelectDataButton, SIGNAL(clicked()), this, SLOT(selectModifiedData()));
}

//-----------------------------------------------------------------------------
qSlicerSaveDataDialogPrivate::~qSlicerSaveDataDialogPrivate()
{
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::setMRMLScene(vtkMRMLScene* scene)
{
  this->MRMLScene = scene;
  this->populateItems();
}

//-----------------------------------------------------------------------------
vtkMRMLScene* qSlicerSaveDataDialogPrivate::mrmlScene()const
{
  return this->MRMLScene;
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::setDirectory(const QString& newDirectory)
{
  QDir newDir(newDirectory);
  Q_ASSERT(newDir.exists());

  const int rowCount = this->FileWidget->rowCount();
  for( int row = 0; row < rowCount; ++row)
    {
    QTableWidgetItem* fileItem = this->FileWidget->item(row, NodeNameColumn);
    Q_ASSERT(fileItem);
    if (fileItem->checkState() == Qt::Unchecked)
      {
      continue;
      }
    ctkDirectoryButton* directoryItemButton = qobject_cast<ctkDirectoryButton*>(
      this->FileWidget->cellWidget(row, FileDirectoryColumn));
    Q_ASSERT(directoryItemButton);
    directoryItemButton->setDirectory(newDir.path());
    }
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::populateItems()
{
  // clear the list
  this->FileWidget->setRowCount(0);
  if (this->MRMLScene == 0)
    {
    return;
    }

  // sorting the table while doing insertions is dangerous
  bool sortingEnabled = this->FileWidget->isSortingEnabled();
  this->FileWidget->setSortingEnabled(false);

  this->DirectoryButton->setDirectory(this->MRMLScene->GetRootDirectory());

  this->populateScene();

  const int nodeCount = this->MRMLScene->GetNumberOfNodesByClass("vtkMRMLStorableNode");
  for (int i = 0 ; i < nodeCount; ++i)
    {
    vtkMRMLStorableNode* node = vtkMRMLStorableNode::SafeDownCast(
      this->MRMLScene->GetNthNodeByClass(i, "vtkMRMLStorableNode"));
    Q_ASSERT(node);
    this->populateNode(node);
    }
  // restore the sorting flag because the insertions are over
  this->FileWidget->setSortingEnabled(sortingEnabled);
  // let's try to resize the columns according to their new contents
  this->FileWidget->resizeColumnsToContents();
  // let's try to show the whole table on screen
  // TODO: find a function in Qt that does it automatically.
  this->resize(this->layout()->contentsMargins().left()
               + this->FileWidget->frameWidth()
               + this->FileWidget->verticalHeader()->sizeHint().width()
               + this->FileWidget->horizontalHeader()->length()
               + this->FileWidget->frameWidth()
               + this->layout()->contentsMargins().right(),
               this->sizeHint().height());
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::populateScene()
{
  int row = this->FileWidget->rowCount();
  this->FileWidget->insertRow(row);

  // Scene Name
  QTableWidgetItem* sceneNameItem = new QTableWidgetItem("(Scene Description)");
  sceneNameItem->setFlags((sceneNameItem->flags() | Qt::ItemIsUserCheckable) & ~Qt::ItemIsEditable);
  sceneNameItem->setCheckState(Qt::Checked);
  this->FileWidget->setItem(row, NodeNameColumn, sceneNameItem);

  // Scene Type
  QTableWidgetItem* sceneTypeItem = new QTableWidgetItem("(SCENE)");
  sceneTypeItem->setFlags(sceneTypeItem->flags() & ~Qt::ItemIsEditable);
  this->FileWidget->setItem(row, NodeTypeColumn, sceneTypeItem);

  // Scene Status
  QTableWidgetItem* sceneModifiedItem = new QTableWidgetItem("Modified");
  sceneModifiedItem->setFlags(sceneModifiedItem->flags() & ~Qt::ItemIsEditable);
  this->FileWidget->setItem(row, NodeStatusColumn, sceneModifiedItem);

  // Scene Format
  QComboBox* sceneComboBoxWidget = new QComboBox(this->FileWidget);
  sceneComboBoxWidget->addItem("MRML (.mrml)");
  sceneComboBoxWidget->setEnabled(false);
  this->FileWidget->setCellWidget(row, FileFormatColumn, sceneComboBoxWidget);

  // Scene FileName
  if (this->MRMLScene->GetURL() != 0)
    {
    QFileInfo sceneFileInfo;
    sceneFileInfo = QFileInfo( QDir(this->MRMLScene->GetRootDirectory()),
                               this->MRMLScene->GetURL());
    this->FileWidget->setItem( row, FileNameColumn,
                               new QTableWidgetItem(
                                 sceneFileInfo.completeBaseName()));
    }
  else
    {
    this->FileWidget->setItem(row, FileNameColumn,
                              new QTableWidgetItem("SlicerScene1"));

    }
  // Scene Directory
  ctkDirectoryButton* sceneDirectoryButton =
    new ctkDirectoryButton(this->MRMLScene->GetRootDirectory(), this->FileWidget);
  this->FileWidget->setCellWidget(row, FileDirectoryColumn, sceneDirectoryButton);
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::populateNode(vtkMRMLStorableNode* node)
{
  // Don't show if the node doesn't want to (internal node)
  if (node->GetHideFromEditors())
    {
    return;
    }

  // get absolute filename
  QFileInfo fileInfo = this->nodeFileInfo(node);
  if (fileInfo == QFileInfo())
    {
    return;
    }

  // Create a new entry
  int row = this->FileWidget->rowCount();
  this->FileWidget->insertRow(row);

  // Node name
  QTableWidgetItem *nodeNameItem = this->createNodeNameItem(node);
  this->FileWidget->setItem(row, NodeNameColumn, nodeNameItem);

  // Node type
  QTableWidgetItem *nodeTypeItem = this->createNodeTypeItem(node);
  this->FileWidget->setItem(row, NodeTypeColumn, nodeTypeItem);

  // Node status
  QTableWidgetItem *nodeModifiedItem = this->createNodeStatusItem(node, fileInfo);
  this->FileWidget->setItem(row, NodeStatusColumn, nodeModifiedItem);

  // Select modified nodes by default
  nodeNameItem->setCheckState(
    node->GetModifiedSinceRead() ? Qt::Checked : Qt::Unchecked);

  // File format
  QWidget* fileFormatsWidget = this->createFileFormatsWidget(node, fileInfo);
  this->FileWidget->setCellWidget(row, FileFormatColumn, fileFormatsWidget);

  // File name
  QTableWidgetItem *fileNameItem = this->createFileNameItem(fileInfo);
  this->FileWidget->setItem(row, FileNameColumn, fileNameItem);

  // File Directory
  QWidget* directoryWidget = this->createFileDirectoryWidget(fileInfo);
  this->FileWidget->setCellWidget(row, FileDirectoryColumn, directoryWidget);
}

//-----------------------------------------------------------------------------
QFileInfo qSlicerSaveDataDialogPrivate::nodeFileInfo(vtkMRMLStorableNode* node)
{
  vtkMRMLStorageNode* storageNode = node->CreateDefaultStorageNode();
  if (storageNode == 0)
    {
    return QFileInfo();
    }

  vtkMRMLStorageNode* snode = node->GetStorageNode();
  if (snode == 0)
    {
    storageNode->SetScene(this->MRMLScene);
    this->MRMLScene->AddNode(storageNode);
    node->SetAndObserveStorageNodeID(storageNode->GetID());
    storageNode->Delete();
    snode = storageNode;
    }

  if (snode->GetFileName() == 0 && !this->DirectoryButton->directory().isEmpty())
    {
    QString fileExtension = snode->GetDefaultWriteFileExtension();
    if (!fileExtension.isEmpty())
      {
      fileExtension = QString(".") + fileExtension;
      }
    QFileInfo fileName(QDir(this->DirectoryButton->directory()),
                       QString(node->GetName()) + fileExtension);
    snode->SetFileName(fileName.absoluteFilePath().toLatin1().data());
    }

  QFileInfo fileInfo;
  if (this->MRMLScene->IsFilePathRelative(snode->GetFileName()))
    {
    fileInfo = QFileInfo(QDir(this->MRMLScene->GetRootDirectory()),
                         snode->GetFileName());
    }
  else
    {
    fileInfo = QFileInfo(snode->GetFileName());
    }
  return fileInfo;
}

//-----------------------------------------------------------------------------
QTableWidgetItem* qSlicerSaveDataDialogPrivate::createNodeNameItem(vtkMRMLStorableNode* node)
{
  QTableWidgetItem* nodeNameItem = new QTableWidgetItem(node->GetName());
  nodeNameItem->setFlags( (nodeNameItem->flags() | Qt::ItemIsUserCheckable) & ~Qt::ItemIsEditable);
  // the tooltip is used to store the id of the nodes
  Q_ASSERT(node->GetStorageNode());
  nodeNameItem->setData(Qt::ToolTipRole, QString(node->GetID()) + " " + node->GetStorageNode()->GetID() );
  return nodeNameItem;
}

//-----------------------------------------------------------------------------
QTableWidgetItem* qSlicerSaveDataDialogPrivate::createNodeTypeItem(vtkMRMLStorableNode* node)
{
  QTableWidgetItem* nodeTypeItem = new QTableWidgetItem(node->GetNodeTagName());
  nodeTypeItem->setFlags( nodeTypeItem->flags() & ~Qt::ItemIsEditable);
  return nodeTypeItem;
}

//-----------------------------------------------------------------------------
QTableWidgetItem* qSlicerSaveDataDialogPrivate::createNodeStatusItem(vtkMRMLStorableNode* node, const QFileInfo& fileInfo)
{
  // Node status (modified / not modified)
  // As a safety measure:
  // If the data is sitting in cache, it's vulnerable to overwriting or deleting.
  // Mark the node as modified since read so that a user will be more likely
  // to save it to a reliable location on local (or remote) disk.
  if ( this->MRMLScene->GetCacheManager() )
    {
    if ( this->MRMLScene->GetCacheManager()->GetRemoteCacheDirectory() )
      {
      QString cacheDir = this->MRMLScene->GetCacheManager()->GetRemoteCacheDirectory();
      int pos = fileInfo.absoluteFilePath().indexOf(cacheDir);
      if ( pos != -1)
        {
        node->ModifiedSinceReadOn();
        }
      }
    else
      {
      qWarning() << "Warning saving data: cannot get a default cache "
        "directory, so not able to check whether any datafiles are residing in cache "
        "and should be marked for save by default. Please take care when saving data.";
      }
    }
  else
    {
    qWarning() << "Warning saving data: cannot get a default cache "
      "directory, so not able to check whether any datafiles are residing in cache "
      "and should be marked for save by default. Please take care when saving data.";
    }

  QTableWidgetItem *nodeModifiedItem =
    new QTableWidgetItem(node->GetModifiedSinceRead() ?
                         tr("Modified") : tr("Not Modified"));
  nodeModifiedItem->setFlags( nodeModifiedItem->flags() & ~Qt::ItemIsEditable);
  return nodeModifiedItem;
}

//-----------------------------------------------------------------------------
QWidget* qSlicerSaveDataDialogPrivate::createFileFormatsWidget(vtkMRMLStorableNode* node, const QFileInfo& fileInfo)
{
  vtkMRMLStorageNode* snode = node->GetStorageNode();
  Q_ASSERT(snode);
  QComboBox* fileFormats = new QComboBox(this->FileWidget);
  const int formatCount = snode->GetSupportedWriteFileTypes()->GetNumberOfValues();
  int currentFormat = -1;
  for (int formatIt = 0; formatIt < formatCount; ++formatIt)
    {
    vtkStdString format =
      snode->GetSupportedWriteFileTypes()->GetValue(formatIt);
    fileFormats->addItem(format.c_str());
    if (QString(vtkDataFileFormatHelper::GetFileExtensionFromFormatString(
                  format.c_str())) == (QString(".") + fileInfo.suffix()))
      {
      currentFormat = formatIt;
      }
    }
  fileFormats->setCurrentIndex(currentFormat);
  if (currentFormat == -1)
    {
    fileFormats->setEditText("Pick format for saving");
    }

  // TODO: use QSignalMapper
  QObject::connect(fileFormats, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(formatChanged()));
  return fileFormats;
}

//-----------------------------------------------------------------------------
QTableWidgetItem* qSlicerSaveDataDialogPrivate::createFileNameItem(const QFileInfo& fileInfo)
{
  return new QTableWidgetItem(fileInfo.fileName());
}

//-----------------------------------------------------------------------------
QWidget* qSlicerSaveDataDialogPrivate::createFileDirectoryWidget(const QFileInfo& fileInfo)
{
  // TODO: use QSignalMapper
  return new ctkDirectoryButton(fileInfo.absolutePath(),this->FileWidget);
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::accept()
{
  if (!this->save())
    {
    return;
    }
  this->done(QDialog::Accepted);
}

//-----------------------------------------------------------------------------
bool qSlicerSaveDataDialogPrivate::save()
{
  // Set the root directory to the scene so that the nodes paths are points
  // to the new scene path. It's important to have the right node paths
  // relatively to the scene in the saved mrml scene file.
  if (!this->prepareForSaving())
    {
    return false;
    }
  // Save the nodes first then the scene
  // Saving the nodes first ensures that the saved scene will points to the
  // potentially new path of the nodes.
  if (!this->saveNodes())
    {
    return false;
    }
  if (!this->saveScene())
    {
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool qSlicerSaveDataDialogPrivate::saveNodes()
{
  QMessageBox::StandardButton forceOverwrite = QMessageBox::Ignore;
  QList<qSlicerIO::IOProperties> files;
  for (int row = 0; row < this->FileWidget->rowCount(); ++row)
    {
    QTableWidgetItem* nodeNameItem = this->FileWidget->item(row, NodeNameColumn);
    QTableWidgetItem* nodeTypeItem = this->FileWidget->item(row, NodeTypeColumn);
    QTableWidgetItem* nodeStatusItem = this->FileWidget->item(row, NodeStatusColumn);
    QComboBox* fileFormatComboBox = qobject_cast<QComboBox*>(
      this->FileWidget->cellWidget(row, FileFormatColumn));
    QTableWidgetItem* fileNameItem = this->FileWidget->item(row, FileNameColumn);
    ctkDirectoryButton* fileDirectoryButton = qobject_cast<ctkDirectoryButton*>(
      this->FileWidget->cellWidget(row, FileDirectoryColumn));

    Q_ASSERT(nodeNameItem);
    Q_ASSERT(nodeTypeItem);
    Q_ASSERT(fileFormatComboBox);
    Q_ASSERT(fileNameItem);
    Q_ASSERT(fileDirectoryButton);

    // don't save unchecked nodes
    if (nodeNameItem->checkState() != Qt::Checked)
      {
      continue;
      }

    // only save nodes here
    if (nodeTypeItem->text() == "(SCENE)")
      {
      continue;
      }

    if (fileNameItem->text().isEmpty())
      {
      QMessageBox::warning(this, tr("Saving node..."),
                           tr("Node %1 not saved, file name is empty.").arg(nodeNameItem->text()));
      continue;
      }

    // file properties
    QDir directory = fileDirectoryButton->directory();
    QFileInfo file = QFileInfo(directory, fileNameItem->text());
    QString format = fileFormatComboBox->currentText();

    // node
    QStringList nodeIDs = nodeNameItem->data(Qt::ToolTipRole).toString().split(" ");
    vtkMRMLNode *node = this->MRMLScene->GetNodeByID(nodeIDs[0].toLatin1().data());
    vtkMRMLStorageNode* snode = vtkMRMLStorageNode::SafeDownCast(
      this->MRMLScene->GetNodeByID(nodeIDs[1].toLatin1().data()));

    // check if the file already exists
    if (file.exists())
      {
      if (forceOverwrite == QMessageBox::NoToAll)
        {
        continue;
        }
      if (forceOverwrite == QMessageBox::Ignore)
        {
        QMessageBox::StandardButton answer =
          QMessageBox::question(this, tr("Saving node..."),
                                tr("The file: %1 already exists."
                                   " Do you want to replace it ?").arg(file.absoluteFilePath()),
                                QMessageBox::Yes | QMessageBox::No |
                                QMessageBox::YesToAll | QMessageBox::NoToAll,
                                QMessageBox::Yes);
        if (answer == QMessageBox::YesToAll)
          {
          forceOverwrite = QMessageBox::YesToAll;
          }
        else if (answer == QMessageBox::NoToAll)
          {
          forceOverwrite = QMessageBox::NoToAll;
          }
        if (answer == QMessageBox::No || answer == QMessageBox::NoToAll)
          {
          continue;
          }
        }
      }

    // save the node
    snode->SetFileName(file.absoluteFilePath().toLatin1().data());
    snode->SetWriteFileFormat(fileFormatComboBox->currentText().toLatin1().data());
    snode->SetURI(0);
    int res = snode->WriteData(node);

    // node has failed to be written
    if (!res)
      {
      QMessageBox::StandardButton answer =
        QMessageBox::question(this, tr("Saving node..."),
                              tr("Cannot write data file: %1.\n"
                                 "Do you want to continue saving?").arg(file.absoluteFilePath()),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
      if (answer == QMessageBox::No)
        {
        return false;
        }
      }

    // clean up node after saving
    node->SetModifiedSinceRead(0);
    nodeNameItem->setCheckState(Qt::Unchecked);
    nodeStatusItem->setText("Not Modified");
    }
  return true;
}

//-----------------------------------------------------------------------------
QFileInfo qSlicerSaveDataDialogPrivate::sceneFile()const
{
  // search the scene
  QAbstractItemModel* model = this->FileWidget->model();
  QModelIndexList found = model->match(
    model->index(0, NodeTypeColumn),
    Qt::DisplayRole, QString("(SCENE)"), 1, Qt::MatchExactly);
  if (!found[0].isValid())
    {
    return QFileInfo(QString(this->MRMLScene->GetURL()));
    }

  QTableWidgetItem* fileNameItem = this->FileWidget->item(found[0].row(), FileNameColumn);
  ctkDirectoryButton* fileDirectoryButton = qobject_cast<ctkDirectoryButton*>(
    this->FileWidget->cellWidget(found[0].row(), FileDirectoryColumn));

  QDir directory = fileDirectoryButton->directory();
  QFileInfo file = QFileInfo(directory, fileNameItem->text());
  if (file.suffix() != "mrml")
    {
    file = QFileInfo(directory, fileNameItem->text() + QString(".mrml"));
    }
  if (file.fileName().isEmpty())
    {
    file = QFileInfo(QString(this->MRMLScene->GetURL()));
    }
  return file;
}

//-----------------------------------------------------------------------------
bool qSlicerSaveDataDialogPrivate::prepareForSaving()
{
  QFileInfo file = this->sceneFile();
  if (file.exists())
    {
    QMessageBox::StandardButton answer =
      QMessageBox::question(this, "Saving scene", "Scene file \""
                            + file.absoluteFilePath() +"\" already exists. "
                            "Do you want to replace it ?");
    if (answer != QMessageBox::Ok)
      {
      return false;
      }
    }
  this->MRMLScene->SetRootDirectory(file.absoluteDir().absolutePath().toLatin1().data());

  // update the root directory of scene snapshot nodes (not sure why)
  const int nnodes = this->MRMLScene->GetNumberOfNodesByClass("vtkMRMLSceneSnapshotNode");
  for (int n=0; n<nnodes; n++)
    {
    vtkMRMLNode* node = this->MRMLScene->GetNthNodeByClass(n, "vtkMRMLSceneSnapshotNode");
    vtkMRMLSceneSnapshotNode *snode = vtkMRMLSceneSnapshotNode::SafeDownCast(node);
    snode->GetNodes()->SetRootDirectory(this->MRMLScene->GetRootDirectory());
    }
  return true;
}

//-----------------------------------------------------------------------------
bool qSlicerSaveDataDialogPrivate::saveScene()
{
  QFileInfo file = this->sceneFile();
  // remove unreferenced nodes
  vtkMRMLLogic *mrmlLogic = vtkMRMLLogic::New();
  mrmlLogic->SetScene(this->MRMLScene);
  mrmlLogic->RemoveUnreferencedDisplayNodes();
  mrmlLogic->RemoveUnreferencedStorageNodes();
  mrmlLogic->Delete();

  this->MRMLScene->SetURL(file.absoluteFilePath().toLatin1().data());
  // TODO
  this->MRMLScene->SetVersion("Slicer4");
  bool res = this->MRMLScene->Commit();
  return res;
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::selectModifiedData()
{
  const int rowCount = this->FileWidget->rowCount();
  for (int row = 0; row < rowCount; ++row)
    {
    QTableWidgetItem* statusItem = this->FileWidget->item(row, NodeStatusColumn);
    QTableWidgetItem* typeItem = this->FileWidget->item(row, NodeTypeColumn);
    Q_ASSERT(statusItem);
    Q_ASSERT(typeItem);
    QTableWidgetItem* nodeNameItem = this->FileWidget->item(row, NodeNameColumn);
    Q_ASSERT(nodeNameItem);
    nodeNameItem->setCheckState(
      statusItem->text() == tr("Modified") &&
      typeItem->text() != tr("(SCENE)") ? Qt::Checked : Qt::Unchecked);
    }
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::selectModifiedSceneData()
{
  // Select all the "modified" nodes.
  const int rowCount = this->FileWidget->rowCount();
  for (int row = 0; row < rowCount; ++row)
    {
    QTableWidgetItem* statusItem = this->FileWidget->item(row, NodeStatusColumn);
    Q_ASSERT(statusItem);
    if (statusItem->text() != tr("Modified"))
      {
      continue;
      }
    QTableWidgetItem* nodeNameItem = this->FileWidget->item(row, NodeNameColumn);
    Q_ASSERT(nodeNameItem);
    nodeNameItem->setCheckState(
      statusItem->text() == tr("Modified") ? Qt::Checked : Qt::Unchecked);
    }
}

//-----------------------------------------------------------------------------
void qSlicerSaveDataDialogPrivate::formatChanged()
{
  // Search for the item whose format has changed
  QComboBox* formatComboBox = qobject_cast<QComboBox*>(this->sender());
  Q_ASSERT(formatComboBox);
  const int rowCount = this->FileWidget->rowCount();
  int row = 0;
  for (; row < rowCount; ++row)
    {
    if (formatComboBox == this->FileWidget->cellWidget(row, FileFormatColumn))
      {
      break;
      }
    }
  Q_ASSERT(row < rowCount);

  // Set the new selected extension to the file name
  QString extension = vtkDataFileFormatHelper::GetFileExtensionFromFormatString(
    formatComboBox->currentText().toStdString().c_str());
  QTableWidgetItem* fileNameItem = this->FileWidget->item(row, FileNameColumn);
  Q_ASSERT(fileNameItem);
  fileNameItem->setText(QFileInfo(fileNameItem->text()).baseName() + extension);

  // If the user changed the format, that means he wants to save the node
  // Select the row to mark the node to be saved.
  QTableWidgetItem* nodeNameItem = this->FileWidget->item(row, NodeNameColumn);
  Q_ASSERT(nodeNameItem);
  nodeNameItem->setCheckState(Qt::Checked);
}

//-----------------------------------------------------------------------------
qSlicerSaveDataDialog::qSlicerSaveDataDialog(QObject* _parent)
  :qSlicerFileDialog(_parent)
{
  CTK_INIT_PRIVATE(qSlicerSaveDataDialog);
}

//-----------------------------------------------------------------------------
qSlicerSaveDataDialog::~qSlicerSaveDataDialog()
{
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerSaveDataDialog::fileType()const
{
  // FIXME: not really a nofile, but more a collection of files
  return qSlicerIO::NoFile;
}

//-----------------------------------------------------------------------------
qSlicerFileDialog::IOAction qSlicerSaveDataDialog::action()const
{
  return qSlicerFileDialog::Write;
}

//-----------------------------------------------------------------------------
bool qSlicerSaveDataDialog::exec(const qSlicerIO::IOProperties& readerProperties)
{
  CTK_D(qSlicerSaveDataDialog);
  Q_ASSERT(!readerProperties.contains("fileName"));
#ifdef Slicer3_USE_KWWIDGETS
  d->setWindowFlags(d->windowFlags() | Qt::WindowStaysOnTopHint);
#endif
  d->setMRMLScene(qSlicerCoreApplication::application()->mrmlScene());
  if (d->exec() != QDialog::Accepted)
    {
    return false;
    }
  return true;
}
