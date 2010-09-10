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

#ifndef __qSlicerSlicer2SceneReader
#define __qSlicerSlicer2SceneReader

// CTK includes
#include <ctkPimpl.h>

// SlicerQt includes
#include "qSlicerIO.h"

class qSlicerSlicer2SceneReaderPrivate;

//-----------------------------------------------------------------------------
class qSlicerSlicer2SceneReader: public qSlicerIO
{
  Q_OBJECT
public: 
  qSlicerSlicer2SceneReader(QObject* parent = 0);
  virtual QString description()const;
  virtual IOFileType fileType()const;
  virtual QString extensions()const;

  virtual bool load(const IOProperties& properties);
private:
  CTK_DECLARE_PRIVATE(qSlicerSlicer2SceneReader);
};

#endif
