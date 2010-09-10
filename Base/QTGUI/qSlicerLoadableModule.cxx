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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerLoadableModule.h"

//-----------------------------------------------------------------------------
class qSlicerLoadableModulePrivate: public ctkPrivate<qSlicerLoadableModule>
{
public:
};

//-----------------------------------------------------------------------------
CTK_CONSTRUCTOR_1_ARG_CXX(qSlicerLoadableModule, QObject*);

//-----------------------------------------------------------------------------
void qSlicerLoadableModule::setup()
{
  Q_ASSERT(ctk_d() != 0);
}

//-----------------------------------------------------------------------------
QString qSlicerLoadableModule::helpText()const
{
  qDebug() << "WARNING: " << this->metaObject()->className()
           << "::helpText() is not implemented";
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerLoadableModule::acknowledgementText()const
{
  qDebug() << "WARNING: " << this->metaObject()->className()
           << "::acknowledgementText - Not implemented";
  return QString();
}
