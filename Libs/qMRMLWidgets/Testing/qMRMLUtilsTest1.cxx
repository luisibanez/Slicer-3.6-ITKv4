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

#include <qMRMLUtils.h>
#include <QApplication>

#include <vtkMRMLSliceNode.h>

// STD includes
#include <stdlib.h>
#include <iostream>

int qMRMLUtilsTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkMRMLSliceNode * mrmlNode = vtkMRMLSliceNode::New();

  const int numberOfChildren = qMRMLUtils::childCount( mrmlNode );

  std::cout << "number of children " << numberOfChildren << std::endl;

  //  FIXME
  //  FIXME
  //  FIXME
  //  FIXME   Many other methods to be exercised here...
  //  FIXME
  //  FIXME
  //  FIXME
  //  FIXME


  mrmlNode->Delete();
  
  return EXIT_SUCCESS;
}
