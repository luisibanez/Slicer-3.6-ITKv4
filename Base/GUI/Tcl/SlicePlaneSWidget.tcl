
#########################################################
#
if {0} { ;# comment

  SlicePlaneSWidget  - manages PlaneWidget for slice 

# TODO : 

}
#
#########################################################


# ------------------------------------------------------------------
#                             SlicePlaneSWidget
# ------------------------------------------------------------------
#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class SlicePlaneSWidget] == "" } {

  itcl::class SlicePlaneSWidget {

    inherit SWidget

    variable _observations ""

    constructor {args} {}
    destructor {}
    
    # methods
    method updateViewer {} {}
    method processEvent {{caller ""} {event ""}} {}
    method updateWidgetFromNode {sliceNode planeRepresentation} {}
    method updateNodeFromWidget {sliceNode planeRepresentation} {}
    method getLinkedSliceLogics {} {}
    method isCompareView {} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body SlicePlaneSWidget::constructor {sliceGUI} {

  $this configure -sliceGUI $sliceGUI

  # create matrices to store transform state
  set o(storeXYToRAS) [$this vtkNew vtkMatrix4x4]
  set o(storeSliceToRAS) [$this vtkNew vtkMatrix4x4]
  set o(scratchMatrix) [$this vtkNew vtkMatrix4x4]

  # create constant matrices to store orthogonal transforms between slice views
  set o(redToYellowMatrix) [$this vtkNew vtkMatrix4x4]
  set o(redToGreenMatrix) [$this vtkNew vtkMatrix4x4]
  set o(yellowToRedMatrix) [$this vtkNew vtkMatrix4x4]
  set o(yellowToGreenMatrix) [$this vtkNew vtkMatrix4x4]
  set o(greenToRedMatrix) [$this vtkNew vtkMatrix4x4]
  set o(greenToYellowMatrix) [$this vtkNew vtkMatrix4x4]
  
  # fill these matrices
  $o(redToYellowMatrix) Zero
  $o(redToYellowMatrix) SetElement 1 0 -1
  $o(redToYellowMatrix) SetElement 2 1 1
  $o(redToYellowMatrix) SetElement 0 2 -1
  $o(redToYellowMatrix) SetElement 3 3 1

  $o(redToGreenMatrix) Zero
  $o(redToGreenMatrix) SetElement 0 0 1
  $o(redToGreenMatrix) SetElement 2 1 1
  $o(redToGreenMatrix) SetElement 1 2 1
  $o(redToGreenMatrix) SetElement 3 3 1

  $o(yellowToRedMatrix) Zero
  $o(yellowToRedMatrix) SetElement 2 0 -1
  $o(yellowToRedMatrix) SetElement 0 1 -1
  $o(yellowToRedMatrix) SetElement 1 2 1
  $o(yellowToRedMatrix) SetElement 3 3 1

  $o(yellowToGreenMatrix) Zero
  $o(yellowToGreenMatrix) SetElement 2 0 -1
  $o(yellowToGreenMatrix) SetElement 1 1 1
  $o(yellowToGreenMatrix) SetElement 0 2 -1
  $o(yellowToGreenMatrix) SetElement 3 3 1

  $o(greenToRedMatrix) Zero
  $o(greenToRedMatrix) SetElement 0 0 1
  $o(greenToRedMatrix) SetElement 2 1 1
  $o(greenToRedMatrix) SetElement 1 2 1
  $o(greenToRedMatrix) SetElement 3 3 1

  $o(greenToYellowMatrix) Zero
  $o(greenToYellowMatrix) SetElement 2 0 -1
  $o(greenToYellowMatrix) SetElement 1 1 1
  $o(greenToYellowMatrix) SetElement 0 2 -1
  $o(greenToYellowMatrix) SetElement 3 3 1

  #
  # set up observers on sliceNode
  # - track them so they can be removed in the destructor
  #
  set node [[$sliceGUI GetLogic] GetSliceNode]
  lappend _observations [$::slicer3::Broker AddObservation $node DeleteEvent "::SWidget::ProtectedDelete $this"]
  lappend _observations [$::slicer3::Broker AddObservation $node ModifiedEvent "::SWidget::ProtectedCallback $this processEvent $node ModifiedEvent"]


  set scene [$sliceGUI GetMRMLScene]
  lappend _observations [$::slicer3::Broker AddObservation $scene DeleteEvent "::SWidget::ProtectedDelete $this"]
  lappend _observations [$::slicer3::Broker AddObservation $scene ModifiedEvent "::SWidget::ProtectedCallback $this processEvent $scene ModifiedEvent"]


  lappend _observations [$::slicer3::Broker AddObservation $sliceGUI DeleteEvent "::SWidget::ProtectedDelete $this"]
  set events {  
    "KeyPressEvent" 
    }
  foreach event $events {
    lappend _observations [$::slicer3::Broker AddObservation $sliceGUI $event "::SWidget::ProtectedCallback $this processEvent $sliceGUI $event"]
  }

  set o(planeWidget) [vtkNew vtkImplicitPlaneWidget2]
  set o(planeRepresentation) [vtkNew vtkImplicitPlaneRepresentation]
  $o(planeWidget) SetRepresentation $o(planeRepresentation)
  
  $this updateViewer

  $o(planeRepresentation) SetDrawPlane 0
  $o(planeRepresentation) PlaceWidget -100 100 -100 100 -100 100

  $::slicer3::Broker AddObservation $o(planeWidget) InteractionEvent "::SWidget::ProtectedCallback $this processEvent $o(planeWidget) AnyEvent"
}


itcl::body SlicePlaneSWidget::destructor {} {
  foreach obs $_observations {
    if { [info command $obs] != "" } {
      $::slicer3::Broker RemoveObservation $obs
    }
  }
}



# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

#
# handle scene and slice node events
#

itcl::body SlicePlaneSWidget::updateViewer { } {
  set viewer [$::slicer3::ApplicationGUI GetActiveViewerWidget] 
  if { $viewer != "" } {
    set rwi [[$viewer GetMainViewer] GetRenderWindowInteractor]

    if { [$o(planeWidget) GetInteractor] != $rwi } {
      # Update the size of the render window interactor to match that of the render window
      set rw [[$viewer GetMainViewer] GetRenderWindow]
      $rwi UpdateSize [lindex [$rw GetSize] 0] [lindex [$rw GetSize] 1]
      # the problem is that it gets reset, so there's another call in the visibility toggle

      $o(planeWidget) SetInteractor $rwi
    }
  }
}


itcl::body SlicePlaneSWidget::processEvent { {caller ""} {event ""} } {

  if { $enabled != "true" } {
    return
  }

  if { [info command $caller] == ""} {
      return
  }

  if { [info command $sliceGUI] == "" || [$sliceGUI GetLogic] == "" } {
    # the sliceGUI was deleted behind our back, so we need to 
    # self destruct
    ::SWidget::ProtectedDelete $this
    return
  }

  set sliceNode [[$sliceGUI GetLogic] GetSliceNode]

  if { $caller == $sliceNode } {
    # if not visible, don't bother calculating anything
    if { ![$sliceNode GetWidgetVisible] } {
      $o(planeWidget) Off
      return
    }
  }

  # catch any chagnes to the active viewer
  $this updateViewer


  if { $caller == $sliceGUI } {

    switch $event {
      "KeyPressEvent" { 
        set key [$_interactor GetKeySym]
        set activeKeys "o"
        if { [lsearch $activeKeys $key] != -1 } {
          $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
          $sliceGUI SetGUICommandAbortFlag 1
          switch [$_interactor GetKeySym] {
            "o" {
              # toggle widget on off
              $this updateWidgetFromNode $sliceNode $o(planeRepresentation)
              set visible [$sliceNode GetWidgetVisible]
              $sliceNode SetWidgetVisible [expr !$visible]
              if { [expr !$visible] } {
                  set rwi [[[$::slicer3::ApplicationGUI GetActiveViewerWidget] GetMainViewer] GetRenderWindowInteractor]
                  # Update the size of the render window interactor to match that of the render window
                  set rw [[[$::slicer3::ApplicationGUI GetActiveViewerWidget] GetMainViewer] GetRenderWindow]
                  if {[$rw GetSize] != [$rwi GetSize]} { 
                      $rwi UpdateSize [lindex [$rw GetSize] 0] [lindex [$rw GetSize] 1]
                  }
              }
            }
          }
        }
      }
    }
  }


  #
  # scene changed
  #
  if { [$caller IsA "vtkMRMLScene"] } {
  }


  set scene [$sliceGUI GetMRMLScene]

  #
  # widget manipulated, so update the slice node to match
  #
  if { $caller == $o(planeWidget) } {
    $this updateNodeFromWidget $sliceNode $o(planeRepresentation)
  }

  #
  # slice node modified, so update the widget to match
  #
  if { $caller == $sliceNode } {
    if { [$sliceNode GetWidgetVisible] } {
      $o(planeWidget) On
    } else {
      $o(planeWidget) Off
    }
    $this updateWidgetFromNode $sliceNode $o(planeRepresentation)
  }

}

#-------------------------------------------------------------------------------
#- update the slice node from the representation (it holds the geometry)

itcl::body SlicePlaneSWidget::updateNodeFromWidget {sliceNode planeRepresentation} {

  set sliceToRAS [$sliceNode GetSliceToRAS]

  # zero out current translation
  $sliceToRAS SetElement 0 3  0
  $sliceToRAS SetElement 1 3  0
  $sliceToRAS SetElement 2 3  0

  #
  # rotate so slice normal matches widget normal
  #
  set sliceNormal [list \
    [$sliceToRAS GetElement 0 2] \
    [$sliceToRAS GetElement 1 2] \
    [$sliceToRAS GetElement 2 2] ]
  set widgetNormal [$planeRepresentation GetNormal]
 
  set radiansToDegrees 57.2957795131
  set xyz [SlicePlaneSWidget::Cross $sliceNormal $widgetNormal]
  
  set dotproduct [SlicePlaneSWidget::Dot $sliceNormal $widgetNormal]
  if { $dotproduct < -1.0 || $dotproduct > 1.0 } {
     if { $dotproduct < -1.0 } {
       set dotproduct -1.0
     } else {
       set dotproduct 1.0
     }
  }
  set w [expr  1. * $radiansToDegrees * acos($dotproduct)]

  set transform [vtkTransform New]
  $transform PostMultiply
  $transform SetMatrix $sliceToRAS
  eval $transform RotateWXYZ $w $xyz
  $transform GetMatrix $sliceToRAS
  $transform Delete

  # insert widget translation
  $sliceToRAS SetElement 0 3 [lindex [$o(planeRepresentation) GetOrigin] 0]
  $sliceToRAS SetElement 1 3 [lindex [$o(planeRepresentation) GetOrigin] 1]
  $sliceToRAS SetElement 2 3 [lindex [$o(planeRepresentation) GetOrigin] 2]
  $sliceNode UpdateMatrices

  # make the other slice planes orthogonal to the one being moved if they are linked
  if { [$_sliceCompositeNode GetLinkedControl] == 1  } {
    set sliceLogics [$this getLinkedSliceLogics]

    if { [$sliceNode GetLayoutName] == "Red" } {
      foreach logic $sliceLogics {
        set snode [$logic GetSliceNode]
        if { [$snode GetLayoutName] == "Yellow" } {
          $o(scratchMatrix) Multiply4x4 [$sliceNode GetSliceToRAS] $o(redToYellowMatrix) $o(scratchMatrix)
          [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
          $snode UpdateMatrices
        } elseif { [$snode GetLayoutName] == "Green" } {
          $o(scratchMatrix) Multiply4x4 [$sliceNode GetSliceToRAS] $o(redToGreenMatrix) $o(scratchMatrix)
          [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
          $snode UpdateMatrices
        }
      }
    } elseif { [$sliceNode GetLayoutName] == "Yellow" } {
      foreach logic $sliceLogics {
        set snode [$logic GetSliceNode]
        if { [$snode GetLayoutName] == "Red" } {
          $o(scratchMatrix) Multiply4x4 [$sliceNode GetSliceToRAS] $o(yellowToRedMatrix) $o(scratchMatrix)
          [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
          $snode UpdateMatrices
        } elseif { [$snode GetLayoutName] == "Green" } {
          $o(scratchMatrix) Multiply4x4 [$sliceNode GetSliceToRAS] $o(yellowToGreenMatrix) $o(scratchMatrix)
          [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
          $snode UpdateMatrices
        }
      }
    } elseif { [$sliceNode GetLayoutName] == "Green" } {
      foreach logic $sliceLogics {
        set snode [$logic GetSliceNode]
        if { [$snode GetLayoutName] == "Red" } {
          $o(scratchMatrix) Multiply4x4 [$sliceNode GetSliceToRAS] $o(greenToRedMatrix) $o(scratchMatrix)
          [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
          $snode UpdateMatrices
        } elseif { [$snode GetLayoutName] == "Yellow" } {
          $o(scratchMatrix) Multiply4x4 [$sliceNode GetSliceToRAS] $o(greenToYellowMatrix) $o(scratchMatrix)
          [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
          $snode UpdateMatrices
        }
      }
    }
  }
}

#-------------------------------------------------------------------------------
#- update the representation that holds the geometry from the slice node

itcl::body SlicePlaneSWidget::updateWidgetFromNode {sliceNode planeRepresentation} {

  set sliceToRAS [$sliceNode GetSliceToRAS]
  $planeRepresentation SetOrigin \
    [$sliceToRAS GetElement 0 3] \
    [$sliceToRAS GetElement 1 3] \
    [$sliceToRAS GetElement 2 3]
  $planeRepresentation SetNormal \
    [$sliceToRAS GetElement 0 2] \
    [$sliceToRAS GetElement 1 2] \
    [$sliceToRAS GetElement 2 2]
  set viewer [$::slicer3::ApplicationGUI GetActiveViewerWidget] 
  if { $viewer != "" } {
    $viewer RequestRender
  }
}


# Return the SliceLogics that are linked to the current 
# SliceNode/SliceCompositeNode.  
#
# The list of linked logics either contains the current slice (because 
# linking is off) or a list of logics (one for the current slice and others 
# for linked slices).

itcl::body SlicePlaneSWidget::getLinkedSliceLogics { } {
  set logic [$sliceGUI GetLogic]
  set sliceNode [$logic GetSliceNode]
  set orientString [$sliceNode GetOrientationString]

  set logics ""
  set link [$_sliceCompositeNode GetLinkedControl]
  if { [$this isCompareView] == 1 && $link == 1 } {
    set ssgui [[$::slicer3::ApplicationGUI GetApplication] GetModuleGUIByName "Slices"]
    set layout [$::slicer3::ApplicationGUI GetGUILayoutNode]
    set viewArrangement [$layout GetViewArrangement]

    set numsgui [$ssgui GetNumberOfSliceGUI]

    for { set i 0 } { $i < $numsgui } { incr i } {
      if { $i == 0} {
        set sgui [$ssgui GetFirstSliceGUI]
        set lname [$ssgui GetFirstSliceGUILayoutName]
          } else {
            set sgui [$ssgui GetNextSliceGUI $lname]
            set lname [$ssgui GetNextSliceGUILayoutName $lname]
          }

          if { [string first "Compare" $lname] != 0 } {
            continue
          } 

          set currSliceNode [$sgui GetSliceNode]
          set currOrientString [$currSliceNode GetOrientationString]
          if { [string compare $orientString $currOrientString] == 0 } {
            lappend logics [$sgui GetLogic]
          }
    }
  } elseif { [$this isCompareView] == 0 && $link == 1} {
    set ssgui [[$::slicer3::ApplicationGUI GetApplication] GetModuleGUIByName "Slices"]
    set layout [$::slicer3::ApplicationGUI GetGUILayoutNode]
    set viewArrangement [$layout GetViewArrangement]

    set numsgui [$ssgui GetNumberOfSliceGUI]

    for { set i 0 } { $i < $numsgui } { incr i } {
      if { $i == 0} {
        set sgui [$ssgui GetFirstSliceGUI]
        set lname [$ssgui GetFirstSliceGUILayoutName]
      } else {
        set sgui [$ssgui GetNextSliceGUI $lname]
        set lname [$ssgui GetNextSliceGUILayoutName $lname]
      }

      set currSliceNode [$sgui GetSliceNode]
      set currOrientString [$currSliceNode GetOrientationString]
      lappend logics [$sgui GetLogic]
    }
  } else {
    lappend logics [$sliceGUI GetLogic]
  }

  return $logics
}


# Are we in a compare view mode?
itcl::body SlicePlaneSWidget::isCompareView { } {
  set layout [$::slicer3::ApplicationGUI GetGUILayoutNode]
  set viewArrangement [$layout GetViewArrangement]

  if { $viewArrangement == 12 } {
    set lname [$_sliceNode GetSingletonTag]
    if { [string first "Compare" $lname] != 0 } {
      return 0
    } 
    return 1
  } else {
    return 0
  }
}

#-------------------------------------------------------------------------------
#- Helper procs

proc SlicePlaneSWidget::Cross { x y } {
  foreach {x0 x1 x2} $x {}
  foreach {y0 y1 y2} $y {}
  set z0 [expr $x1*$y2 - $x2*$y1]
  set z1 [expr $x2*$y0 - $x0*$y2]
  set z2 [expr $x0*$y1 - $x1*$y0]
  return [list $z0 $z1 $z2]
}

proc SlicePlaneSWidget::Dot { x y } {
  set sum 0.0
  foreach xx $x yy $y {
    set sum [expr $sum + $xx*$yy]
  }
  return $sum
}

proc TestSlicePlaneSWidget {} {
  itcl::delete class SlicePlaneSWidget
  source $::Slicer3_HOME/lib/Slicer3/SlicerBaseGUI/Tcl/SlicePlaneSWidget.tcl

  set sliceGUI [lindex [vtkSlicerSliceGUI ListInstances] 0]
  SlicePlaneSWidget #auto $sliceGUI
}