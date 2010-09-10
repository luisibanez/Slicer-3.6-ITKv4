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

#ifndef __qSlicerIO_h
#define __qSlicerIO_h

// Qt includes
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

// CTK includes
#include <ctkPimpl.h>

// QtCore includes
#include "qSlicerBaseQTCoreExport.h"

class vtkMRMLScene;
class qSlicerIOOptions;
class qSlicerIOPrivate;

class Q_SLICER_BASE_QTCORE_EXPORT qSlicerIO : public QObject
{
  Q_OBJECT
public:
  explicit qSlicerIO(QObject* parent = 0);
  virtual ~qSlicerIO();

  typedef int IOFileType; 
  enum IOFileTypes
  {
    NoFile = 0,
    SceneFile = 1, 
    VolumeFile = 2,
    TransformFile = 3,
    ModelFile = 4,
    ScalarOverlayFile = 5,
    ColorTableFile = 7,
    FiducialListFile = 8,
    DTIFile = 9,
    UserFile = 32,
  };

  typedef QMap<QString, QVariant> IOProperties;

  virtual QString description()const = 0;
  virtual IOFileType fileType()const = 0;
  /// Return  a list (separated by " " ) of the supported extensions
  /// Example: "*.jpg *.png *.tiff"
  virtual QString extensions()const;
  /// Based on the file extensions, returns true if the file can be read,
  /// false otherwise.
  /// This function is relatively fast as it doesn't try to access the file.
  bool canLoadFile(const QString& file)const;
  /// Returns a list of options for the reader. qSlicerIOOptions can be
  /// derived and have a UI associated to it (i.e. qSlicerIOOptionsWidget).
  /// Warning: you are responsible for freeing the memory of the returned
  /// options
  virtual qSlicerIOOptions* options()const;

  // TBD: Derive from qSlicerObject instead of reimplementing setMRMLScene ?
  void setMRMLScene(vtkMRMLScene* scene);
  vtkMRMLScene* mrmlScene()const;
  
  virtual bool load(const IOProperties& properties);
  virtual bool save(const IOProperties& properties);

  QStringList loadedNodes()const;
  QStringList savedNodes()const;
  
protected:
  void setLoadedNodes(const QStringList& nodes);
  void setSavedNodes(const QStringList& nodes);
  
private:
  CTK_DECLARE_PRIVATE(qSlicerIO);
};

#endif
