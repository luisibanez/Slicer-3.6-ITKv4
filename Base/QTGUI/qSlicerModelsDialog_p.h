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

#ifndef __qSlicerModelsDialog_p_h
#define __qSlicerModelsDialog_p_h

// Qt includes
#include <QDialog>
#include <QStringList>

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerModelsDialog.h"
#include "ui_qSlicerModelsDialog.h"

//-----------------------------------------------------------------------------
class qSlicerModelsDialogPrivate
  : public QDialog
  , public ctkPrivate<qSlicerModelsDialog>
  , public Ui_qSlicerModelsDialog
{
  Q_OBJECT
  CTK_DECLARE_PUBLIC(qSlicerModelsDialog);
public:
  explicit qSlicerModelsDialogPrivate(QWidget* parentWidget = 0);
  virtual ~qSlicerModelsDialogPrivate();

  void init();
  QStringList selectedFiles()const;

public slots:
  void openAddModelFileDialog();
  void openAddModelDirectoryDialog();

protected:
  QStringList SelectedFiles;
};

#endif
