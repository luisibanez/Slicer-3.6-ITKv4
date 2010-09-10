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

#ifndef __qSlicerMainWindowCorePrivate_p_h
#define __qSlicerMainWindowCorePrivate_p_h

// Qt includes
#include <QObject>
#include <QPointer>

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerMainWindowCore.h"
#include "qSlicerMainWindow.h"

class qSlicerAbstractModule;
class ctkPythonShell;

//-----------------------------------------------------------------------------
class qSlicerMainWindowCorePrivate: public QObject,
                                    public ctkPrivate<qSlicerMainWindowCore>
{
  Q_OBJECT
  CTK_DECLARE_PUBLIC(qSlicerMainWindowCore);

public:
  explicit qSlicerMainWindowCorePrivate();
  virtual ~qSlicerMainWindowCorePrivate();

public:
  QPointer<qSlicerMainWindow> ParentWidget;
  ctkPythonShell*             PythonShell;
};

#endif
