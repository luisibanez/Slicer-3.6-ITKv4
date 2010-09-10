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

// QT includes
#include <QApplication>

// qMRML includes
#include "qMRMLColorTableComboBox.h"
#include "qMRMLSceneFactoryWidget.h"

// MRML includes
#include "vtkMRMLScene.h"

// STD includes
#include <stdlib.h>
#include <iostream>

int qMRMLColorTableComboBoxTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  qMRMLColorTableComboBox nodeSelector;
  qMRMLSceneFactoryWidget sceneFactory;

  sceneFactory.generateScene();

  int currentCount = nodeSelector.nodeCount();
  if (currentCount != 0)
    {
    std::cerr << __LINE__ << " - Error in count() - Expected: 0, current:" << currentCount << std::endl;
    return EXIT_FAILURE;
    }

  // Test: setMRMLScene()/mrmlScene()
  nodeSelector.setMRMLScene(sceneFactory.mrmlScene());
  if (nodeSelector.mrmlScene() != sceneFactory.mrmlScene())
    {
    std::cerr << __LINE__ << " - qMRMLNodeSelector::setMRMLScene() failed." << std::endl;
    return EXIT_FAILURE;
    }
  // test a second time, just to make sure methods are well reinit.
  sceneFactory.generateScene();
  nodeSelector.setMRMLScene(sceneFactory.mrmlScene());
  if (nodeSelector.mrmlScene() != sceneFactory.mrmlScene())
    {
    std::cerr << __LINE__ << " - qMRMLNodeSelector::setMRMLScene() failed." << std::endl;
    return EXIT_FAILURE;
    }

  // no type has been given yet -> no item shoud be listed
  if (nodeSelector.nodeCount())
    {
    std::cerr << __LINE__ << " - qMRMLNodeSelector::count() failed: " << nodeSelector.nodeCount() << std::endl;
    return EXIT_FAILURE;
    }

  sceneFactory.deleteScene();

  return EXIT_SUCCESS;
}