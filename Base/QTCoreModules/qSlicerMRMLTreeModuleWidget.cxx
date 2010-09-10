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

// SlicerQt includes
#include "qSlicerMRMLTreeModuleWidget.h"
#include "ui_qSlicerMRMLTreeModule.h"

//-----------------------------------------------------------------------------
class qSlicerMRMLTreeModuleWidgetPrivate: public ctkPrivate<qSlicerMRMLTreeModuleWidget>,
                                          public Ui_qSlicerMRMLTreeModule
{
public:
  qSlicerMRMLTreeModuleWidgetPrivate()
    {
    }
};

//-----------------------------------------------------------------------------
CTK_CONSTRUCTOR_1_ARG_CXX(qSlicerMRMLTreeModuleWidget, QWidget*);

//-----------------------------------------------------------------------------
void qSlicerMRMLTreeModuleWidget::setup()
{
  CTK_D(qSlicerMRMLTreeModuleWidget);
  d->setupUi(this);

}
