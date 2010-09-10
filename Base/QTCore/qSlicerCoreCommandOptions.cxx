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
#include <QSettings>
#include <QDir>
#include <QDebug>

// SlicerQt includes
#include "qSlicerCoreCommandOptions.h"
#include "qSlicerCoreApplication.h" // For disableCurrentSettings()

//-----------------------------------------------------------------------------
class qSlicerCoreCommandOptionsPrivate: public ctkPrivate<qSlicerCoreCommandOptions>
{
public:
  qSlicerCoreCommandOptionsPrivate();

  QHash<QString, QVariant> ParsedArgs;
  QSettings * Settings;
};

//-----------------------------------------------------------------------------
// qSlicerCoreCommandOptionsPrivate methods

//-----------------------------------------------------------------------------
qSlicerCoreCommandOptionsPrivate::qSlicerCoreCommandOptionsPrivate()
{
  this->Settings = 0;
}

//-----------------------------------------------------------------------------
// qSlicerCoreCommandOptions methods

//-----------------------------------------------------------------------------
qSlicerCoreCommandOptions::qSlicerCoreCommandOptions(QSettings* _settings):Superclass(_settings)
{
  CTK_INIT_PRIVATE(qSlicerCoreCommandOptions);
  CTK_D(qSlicerCoreCommandOptions);
  d->Settings = _settings;
  // Use Unix-style argument names
  this->setArgumentPrefix("--", "-");
  // Enable QSettings support
  this->enableSettings("disable-settings");
}

//-----------------------------------------------------------------------------
qSlicerCoreCommandOptions::~qSlicerCoreCommandOptions()
{
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::parse(const QStringList& arguments)
{
  CTK_D(qSlicerCoreCommandOptions);

  this->addArguments();

  bool ok = false;
  d->ParsedArgs = this->parseArguments(arguments, &ok);
  if (!ok)
    {
    return false;
    }

  if (d->ParsedArgs.value("disable-settings").toBool())
    {
    qSlicerCoreApplication::application()->disableSettings();
    }

  return true;
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::displayHelpAndExit()const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("help").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::ignoreRest() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("ignore-rest").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::disableCLIModule() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("disable-cli-module").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::disableLoadableModule() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("disable-loadable-module").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::displayVersionAndExit() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("version").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::displayProgramPathAndExit() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("program-path").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::displayHomePathAndExit() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("home").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::displaySettingsPathAndExit() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("settings-path").toBool();
}

//-----------------------------------------------------------------------------
bool qSlicerCoreCommandOptions::verboseModuleDiscovery() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs.value("verbose-module-discovery").toBool();
}

//-----------------------------------------------------------------------------
QString qSlicerCoreCommandOptions::tempDirectory() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  if (d->Settings)
    {
    return d->Settings->value("temp-directory", QVariant(QDir::tempPath())).toString();
    }
  return QDir::tempPath();
}

//-----------------------------------------------------------------------------
void qSlicerCoreCommandOptions::addArguments()
{
  this->addArgument("ignore-rest", "-", QVariant::Bool,
                    "Ignores the rest of the labeled arguments following this flag.",
                    QVariant(false), true);

  this->addArgument("help", "h", QVariant::Bool,
                    "Display available command line arguments.");

  this->addArgument("disable-cli-module", "", QVariant::Bool,
                    "Disables the loading of Command Line Modules.");

  this->addArgument("disable-loadable-module", "", QVariant::Bool,
                    "Disables the loading of Loadable Modules.");

  this->addArgument("version", "", QVariant::Bool,
                    "Displays version information and exits.");

  this->addArgument("program-path", "", QVariant::Bool,
                    "Displays application program path and exits.");

  this->addArgument("home", "", QVariant::Bool,
                    "Displays home path and exits.");

  this->addArgument("settings-path", "", QVariant::Bool,
                    "Displays settings path and exits.");

  this->addArgument("verbose-module-discovery", "", QVariant::Bool,
                    "Enable verbose output during module discovery process.");

  this->addArgument("disable-settings", "", QVariant::Bool,
                    "Start application ignoring user settings.");
}

//-----------------------------------------------------------------------------
QHash<QString, QVariant> qSlicerCoreCommandOptions::parsedArgs() const
{
  CTK_D(const qSlicerCoreCommandOptions);
  return d->ParsedArgs;
}
