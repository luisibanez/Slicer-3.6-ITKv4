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

// SlicerQt includes
#include "qSlicerColorsModuleWidget.h"
#include "ui_qSlicerColorsModule.h"

//-----------------------------------------------------------------------------
class qSlicerColorsModuleWidgetPrivate: public ctkPrivate<qSlicerColorsModuleWidget>,
                                         public Ui_qSlicerColorsModule
{
public:
  CTK_DECLARE_PUBLIC(qSlicerColorsModuleWidget);
};

//-----------------------------------------------------------------------------
CTK_CONSTRUCTOR_1_ARG_CXX(qSlicerColorsModuleWidget, QWidget*);

//-----------------------------------------------------------------------------
void qSlicerColorsModuleWidget::setup()
{
  CTK_D(qSlicerColorsModuleWidget);
  d->setupUi(this);
}
