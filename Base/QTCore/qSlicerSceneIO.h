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

#ifndef __qSlicerSceneIO_h
#define __qSlicerSceneIO_h

// QtCore includes
#include "qSlicerIO.h"

///
/// qSlicerSceneIO is the IO class that handle MRML scene
/// It internally call vtkMRMLScene::Connect() or vtkMRMLScene::Import() 
/// depending on the clear flag.
class qSlicerSceneIO: public qSlicerIO
{
  Q_OBJECT
public: 
  qSlicerSceneIO(QObject* _parent = 0);
  
  virtual QString description()const;
  ///
  /// Support qSlicerIO::SceneFile
  virtual qSlicerIO::IOFileType fileType()const;

  ///
  /// Support only .mrml files
  virtual QString extensions()const;

  ///
  /// the supported properties are:
  /// QString fileName: the path of the mrml scene to load
  /// bool clear: wether the current should be cleared or not
  virtual bool load(const qSlicerIO::IOProperties& properties);
};


#endif
