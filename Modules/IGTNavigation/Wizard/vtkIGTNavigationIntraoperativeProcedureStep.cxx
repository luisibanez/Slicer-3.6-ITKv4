#include "vtkIGTNavigationIntraoperativeProcedureStep.h"

#include "vtkIGTNavigationGUI.h"
#include "vtkIGTNavigationMRMLManager.h"

#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWLabel.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"
#include "vtkKWPushButton.h"
#include "vtkKWMessageDialog.h"




//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTNavigationIntraoperativeProcedureStep);
vtkCxxRevisionMacro(vtkIGTNavigationIntraoperativeProcedureStep, "$Revision: 1.4 $");

//----------------------------------------------------------------------------
vtkIGTNavigationIntraoperativeProcedureStep::vtkIGTNavigationIntraoperativeProcedureStep()
{
  this->SetName("5/5. IntraoperativeProcedure");
  this->SetDescription("Intraoperative procedure");

  this->TrackingFrame = NULL;
  this->TrackingRadioButtonSet = NULL;

  this->EndoscopeCameraFrame = NULL;
}

//----------------------------------------------------------------------------
vtkIGTNavigationIntraoperativeProcedureStep::~vtkIGTNavigationIntraoperativeProcedureStep()
{
  if( this->TrackingFrame)
    {
    this->TrackingFrame->Delete();
    this->TrackingFrame = NULL;
    }

  if( this->EndoscopeCameraFrame)
    {
    this->EndoscopeCameraFrame->Delete();
    this->EndoscopeCameraFrame = NULL;
    }

  if( this->TrackingRadioButtonSet)
    {
    this->TrackingRadioButtonSet->Delete();
    this->TrackingRadioButtonSet = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkIGTNavigationIntraoperativeProcedureStep::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  vtkKWWizardWidget *wizard_widget = this->GetGUI()->GetWizardWidget();

  wizard_widget->GetCancelButton()->SetEnabled(0);

  if (!this->TrackingFrame)
    {
    this->TrackingFrame = vtkKWFrameWithLabel::New();
    }

  if (!this->TrackingFrame->IsCreated())
    {
    this->TrackingFrame->SetParent(
      wizard_widget->GetClientArea());
    this->TrackingFrame->Create();
    this->TrackingFrame->SetLabelText("Tracking Device");
    this->TrackingFrame->SetHeight(100);
    }

  this->Script("pack %s -side top -expand n -fill both -padx 0 -pady 2", 
               this->TrackingFrame->GetWidgetName());

  if (!this->EndoscopeCameraFrame)
    {
    this->EndoscopeCameraFrame = vtkKWFrameWithLabel::New();
    }

  if (!this->EndoscopeCameraFrame->IsCreated())
    {
    this->EndoscopeCameraFrame->SetParent(
      wizard_widget->GetClientArea());
    this->EndoscopeCameraFrame->Create();
    this->EndoscopeCameraFrame->SetLabelText("Endoscope Camera");
    this->EndoscopeCameraFrame->SetHeight(100);
    }

  this->Script("pack %s -side top -expand n -fill both -padx 0 -pady 2", 
               this->EndoscopeCameraFrame->GetWidgetName());

  //Add a help to the step
  vtkKWPushButton * helpButton =  wizard_widget->GetHelpButton();

  vtkKWMessageDialog *msg_dlg1 = vtkKWMessageDialog::New();
  msg_dlg1->SetParent(wizard_widget->GetClientArea());
  msg_dlg1->SetStyleToOkCancel();
  msg_dlg1->Create();
  msg_dlg1->SetTitle("Intraoperative Procedure Step");
  msg_dlg1->SetText( "This is the main step in the IGT application. In this step, the user "
                     "performs the intraoperative procedure");

  helpButton->SetCommand(msg_dlg1, "Invoke");

  msg_dlg1->Delete();


}
//----------------------------------------------------------------------------
void vtkIGTNavigationIntraoperativeProcedureStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
