#ifndef __qMRMLWindowLevelWidget_h
#define __qMRMLWindowLevelWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

#include "qMRMLWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLScalarVolumeDisplayNode;
class vtkMRMLScalarVolumeNode;
class qMRMLWindowLevelWidgetPrivate;

class QMRML_WIDGETS_EXPORT qMRMLWindowLevelWidget : public QWidget
{
  Q_OBJECT
  QVTK_OBJECT

  Q_PROPERTY(ControlMode autoWindowLevel READ autoWindowLevel WRITE setAutoWindowLevel)
  Q_PROPERTY(double window READ window WRITE setWindow)
  Q_PROPERTY(double level READ level WRITE setLevel)
  Q_PROPERTY(double minimumValue READ minimumValue WRITE setMinimumValue)
  Q_PROPERTY(double maximumValue READ maximumValue WRITE setMaximumValue)
  Q_ENUMS(ControlMode)
public:
  /// Constructors
  typedef QWidget Superclass;
  explicit qMRMLWindowLevelWidget(QWidget* parent);
  virtual ~qMRMLWindowLevelWidget(){}

  enum ControlMode
  {
    Manual = 0,
    Auto
  };

  ///
  /// Set Auto/Manual mode
  void setAutoWindowLevel(ControlMode autoWindowLevel);
  ControlMode autoWindowLevel() const;

  /// 
  /// Get window
  double window()const;

  /// 
  /// Get level
  double level()const;

  /// 
  /// Get minimum of the range
  double minimumValue()const;

  /// 
  /// Get maximum of the range
  double maximumValue()const;

  /// 
  /// Return the current MRML node of interest
  vtkMRMLScalarVolumeNode* mrmlVolumeNode()const
    { return this->VolumeNode; };

signals:
  /// 
  /// Signal sent if the window/level value is updated
  void windowLevelValuesChanged(double window, double level);
  /// 
  /// Signal sent if the auto/manual value is updated
  void autoWindowLevelValueChanged(ControlMode value);

public slots:
  ///
  /// Set Auto/Manual mode
  void setAutoWindowLevel(int autoWindowLevel);

  /// 
  /// Set window
  void setWindow(double window);

  /// 
  /// Set level
  void setLevel(double level);

  /// 
  /// Set window/level in once
  void setWindowLevel(double window, double level);

  /// 
  /// Set min/max range
  void setMinMaxRangeValue(double min, double max);
  void setMinimumValue(double min);
  void setMaximumValue(double max);

  /// 
  /// Set the MRML node of interest
  void setMRMLVolumeNode(vtkMRMLScalarVolumeNode* displayNode);
  void setMRMLVolumeNode(vtkMRMLNode* node);


protected slots:

  /// update widget GUI from MRML node
  void updateWidgetFromMRML();

protected:
  /// 
  /// Return the current MRML display node
  vtkMRMLScalarVolumeDisplayNode* mrmlDisplayNode()const 
    { return this->VolumeDisplayNode;};

  /// 
  /// Set current MRML display node
  void setMRMLVolumeDisplayNode(vtkMRMLScalarVolumeDisplayNode* displayNode);


private:
  CTK_DECLARE_PRIVATE(qMRMLWindowLevelWidget);

  vtkMRMLScalarVolumeNode* VolumeNode;
  vtkMRMLScalarVolumeDisplayNode* VolumeDisplayNode;
};

#endif
