
package require Itcl

#########################################################
#
if {0} { ;# comment

  Labeler is an abstract superclass for slicer Editor effects that
  use an outliner or applies a label map to a slice

  SWidget
  ^
  EffectSWidget
  ^
  Labeler
  ^^^
  DrawEffect
  PaintEffect
  LevelTracingEffect
  ...



# TODO : 

}
#
#########################################################

#
#########################################################
# ------------------------------------------------------------------
#                             Labeler
# ------------------------------------------------------------------
#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class Labeler] == "" } {

  itcl::class Labeler {

    inherit EffectSWidget

    constructor {sliceGUI} {EffectSWidget::constructor $sliceGUI} {}
    destructor {}

    public variable paintThreshold 0
    public variable paintThresholdMin 1
    public variable paintThresholdMax 1
    public variable paintOver 1
    public variable polygonDebugViewer 0

    # keep track of first application of the label since the 
    # class was instantiated - this is used to decide when to 
    # make a copy of the label map for the Editor's CheckPoint function
    variable _appliedSinceConstructed 0

    # methods
    method processEvent {{caller ""} {event ""}} {}
    method makeMaskImage {polyData} {}
    method applyPolyMask {polyData} {}
    method applyImageMask { maskIJKToRAS mask bounds } {}
    method undoLastApply {} {}
    method buildOptions {} {}
    method tearDownOptions {} {}
    method setMRMLDefaults {} {}
    method updateMRMLFromGUI {} {}
    method updateGUIFromMRML {} {}
    method rotateSliceToImage {} {}

  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body Labeler::constructor {sliceGUI} {
  set o(painter) [vtkNew vtkImageSlicePaint]
  $this rotateSliceToImage
}

itcl::body Labeler::destructor {} {
}


# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body Labeler::processEvent { {caller ""} {event ""} } {
  
  # don't chain this method - superclass method is meant 
  # to be overridden and not chained.
  set event [$sliceGUI GetCurrentGUIEvent] 
  if { $event == "MouseMoveEvent" && [$this getInAnySliceSWidget] } {
      $this requestDelayedAnnotation 
  }
  
  if { $event == "LeaveEvent" } {
      $this cancelDelayedAnnotation 
  }

  if { $caller != "" && [$caller IsA "vtkKWWidget"] } {
    $this updateMRMLFromGUI
  }

  if { $caller != "" && [$caller IsA "vtkMRMLNode"] } {
    $this updateGUIFromMRML
  }

}


itcl::body Labeler::makeMaskImage {polyData} {

  #
  # use the slicer2-based vtkImageFillROI filter
  #

  #
  # Need to know the mapping from RAS into polygon space
  # so the painter can use this as a mask
  # - need the bounds in RAS space
  # - need to get an IJKToRAS for just the mask area
  # - directions are the XYToRAS for this slice
  # - origin is the lower left of the polygon bounds
  # - TODO: need to account for the boundary pixels
  #
  set maskIJKToRAS [vtkMatrix4x4 New]
  $maskIJKToRAS DeepCopy [$_sliceNode GetXYToRAS]
  [$polyData GetPoints] Modified
  set bounds [$polyData GetBounds]
  foreach {xlo xhi ylo yhi zlo zhi} $bounds {}
  set xlo [expr $xlo - 1]
  set ylo [expr $ylo - 1]
  set originRAS [$this xyToRAS "$xlo $ylo"]
  $maskIJKToRAS SetElement 0 3  [lindex $originRAS 0]
  $maskIJKToRAS SetElement 1 3  [lindex $originRAS 1]
  $maskIJKToRAS SetElement 2 3  [lindex $originRAS 2]

  #
  # get a good size for the draw buffer 
  # - needs to include the full region of the polygon
  # - plus a little extra 
  #
  # round to int and add extra pixel for both sides
  # -- TODO: figure out why we need to add buffer pixels on each 
  #    side for the width in order to end up with a single extra
  #    pixel in the rasterized image map.  Probably has to 
  #    do with how boundary conditions are handled in the filler
  set w [expr int($xhi - $xlo) + 32]
  set h [expr int($yhi - $ylo) + 32]

  set imageData [vtkImageData New]
  $imageData SetDimensions $w $h 1

  if { $_layers(label,image) != "" } {
    $imageData SetScalarType [$_layers(label,image) GetScalarType]
  }
  $imageData AllocateScalars

  #
  # move the points so the lower left corner of the 
  # bounding box is at 1, 1 (to avoid clipping)
  #
  set translate [vtkTransform New]
  $translate Translate [expr -1. * $xlo] [expr -1. * $ylo] 0
  set drawPoints [vtkPoints New]
  $drawPoints Reset
  $translate TransformPoints [$polyData GetPoints] $drawPoints
  $translate Delete
  $drawPoints Modified

  set fill [vtkImageFillROI New]
  $fill SetInput $imageData
  $fill SetValue 1
  $fill SetPoints $drawPoints
  [$fill GetOutput] Update

  set mask [vtkImageData New]
  $mask DeepCopy [$fill GetOutput]

  if { $polygonDebugViewer } {
    #
    # make a little preview window for debugging pleasure
    #
    catch "viewer Delete"
    catch "viewerImage Delete"
    vtkImageViewer viewer
    vtkImageData viewerImage
    viewerImage DeepCopy [$fill GetOutput]
    viewer SetInput viewerImage
    viewer SetColorWindow 2
    viewer SetColorLevel 1
    viewer Render
  }


  #
  # clean up our local class instances
  #
  $fill Delete
  $imageData Delete
  $drawPoints Delete

  return [list $maskIJKToRAS $mask]
}

    
#
# rasterize a polyData (closed list of points) 
# into the label map layer
# - points are specified in current XY space
#
itcl::body Labeler::applyPolyMask {polyData} {

  foreach {x y} [$_interactor GetEventPosition] {}
  $this queryLayers $x $y

  if { $_layers(label,node) == "" } {
    # if there's no label, we can't draw
    return
  }

  set maskResult [$this makeMaskImage $polyData]
  foreach {maskIJKToRAS mask} $maskResult {}

  [$polyData GetPoints] Modified
  set bounds [$polyData GetBounds]

  $this applyImageMask $maskIJKToRAS $mask $bounds

}


#
# apply a pre-rasterized image to the current label layer
# - maskIJKToRAS tells the mapping from image pixels to RAS
# - mask is a vtkImageData
# - bounds are the xy extents of the mask (zlo and zhi ignored)
#
itcl::body Labeler::applyImageMask { maskIJKToRAS mask bounds } {

  #
  # at this point, the mask vtkImageData contains a rasterized
  # version of the polygon and now needs to be added to the label
  # image
  #
  
  if { $_appliedSinceConstructed == 0 } {
    # first application, so save the old label volume
    # - this means there will only be one checkpoint for each 'instance'
    #   of the effect (i.e. multiple brush strokes all undone by one click)
    EditorStoreCheckPoint $_layers(label,node)
    set _appliedSinceConstructed 1
  }

  #
  # get the brush bounding box in ijk coordinates
  # - get the xy bounds
  # - transform to ijk
  # - clamp the bounds to the dimensions of the label image
  #

  foreach {xlo xhi ylo yhi zlo zhi} $bounds {}
  set xyToIJK [[$_layers(label,logic) GetXYToIJKTransform] GetMatrix]
  set tlIJK [$xyToIJK MultiplyPoint $xlo $yhi 0 1]
  set trIJK [$xyToIJK MultiplyPoint $xhi $yhi 0 1]
  set blIJK [$xyToIJK MultiplyPoint $xlo $ylo 0 1]
  set brIJK [$xyToIJK MultiplyPoint $xhi $ylo 0 1]

  # do the clamping
  set dims [$_layers(label,image) GetDimensions]
  foreach v {i j k} c [lrange $tlIJK 0 2] d $dims {
    set tl($v) [expr int(round($c))]
    if { $tl($v) < 0 } { set tl($v) 0 }
    if { $tl($v) >= $d } { set tl($v) [expr $d - 1] }
  }
  foreach v {i j k} c [lrange $trIJK 0 2] d $dims {
    set tr($v) [expr int(round($c))]
    if { $tr($v) < 0 } { set tr($v) 0 }
    if { $tr($v) >= $d } { set tr($v) [expr $d - 1] }
  }
  foreach v {i j k} c [lrange $blIJK 0 2] d $dims {
    set bl($v) [expr int(round($c))]
    if { $bl($v) < 0 } { set bl($v) 0 }
    if { $bl($v) >= $d } { set bl($v) [expr $d - 1] }
  }
  foreach v {i j k} c [lrange $brIJK 0 2] d $dims {
    set br($v) [expr int(round($c))]
    if { $br($v) < 0 } { set br($v) 0 }
    if { $br($v) >= $d } { set br($v) [expr $d - 1] }
  }


  #
  # get the ijk to ras matrices 
  # (use the maskToRAS calculated above)
  #


  set backgroundIJKToRAS [vtkMatrix4x4 New]
  set labelIJKToRAS [vtkMatrix4x4 New]
  foreach layer {background label} {
    set ijkToRAS ${layer}IJKToRAS
    $_layers($layer,node) GetIJKToRASMatrix [set $ijkToRAS]
    set transformNode [$_layers($layer,node) GetParentTransformNode]
    if { $transformNode != "" } {
      if { [$transformNode IsTransformToWorldLinear] } {
        set rasToRAS [vtkMatrix4x4 New]
        $transformNode GetMatrixTransformToWorld $rasToRAS
        $rasToRAS Multiply4x4 $rasToRAS [set $ijkToRAS] [set $ijkToRAS]
        $rasToRAS Delete
      } else {
        error "Cannot handle non-linear transforms"
      }
    }
  }


  #
  # create an exract image for undo if it doesn't exist yet
  #
  if { ![info exists o(extractImage)] } {
    set o(extractImage) [vtkNew vtkImageData]
  }

  #
  # set up the painter class and let 'r rip!
  #
  $o(painter) SetBackgroundImage [$this getInputBackground]
  $o(painter) SetBackgroundIJKToWorld $backgroundIJKToRAS
  $o(painter) SetWorkingImage [$this getInputLabel]
  $o(painter) SetWorkingIJKToWorld $labelIJKToRAS
  $o(painter) SetMaskImage $mask
  $o(painter) SetExtractImage $o(extractImage)
  $o(painter) SetReplaceImage ""
  $o(painter) SetMaskIJKToWorld $maskIJKToRAS
  $o(painter) SetTopLeft $tl(i) $tl(j) $tl(k)
  $o(painter) SetTopRight $tr(i) $tr(j) $tr(k)
  $o(painter) SetBottomLeft $bl(i) $bl(j) $bl(k)
  $o(painter) SetBottomRight $br(i) $br(j) $br(k)
  $o(painter) SetPaintLabel [EditorGetPaintLabel]
  $o(painter) SetPaintOver $paintOver
  $o(painter) SetThresholdPaint $paintThreshold
  $o(painter) SetThresholdPaintRange $paintThresholdMin $paintThresholdMax

  $o(painter) Paint

  $labelIJKToRAS Delete
  $backgroundIJKToRAS Delete
  $maskIJKToRAS Delete
  $mask Delete

  $_layers(label,node) Modified

  return
}

# use the 'save under' information from last paint apply
# to restore the original value of the working volume
# - be careful can only call this after when the painter class
#   is valid (e.g. after an apply but before changing any of the volumes)
#   it should be crash-proof in any case, but may generated warnings
# - if extract image doesn't exist, failes silently
itcl::body Labeler::undoLastApply { } {
  if { [info exists o(extractImage)] } {
    $o(painter) SetReplaceImage $o(extractImage)
    $o(painter) Paint
  }
}


itcl::body Labeler::buildOptions { } {

  chain

  $this setMRMLDefaults

  set o(paintOver) [vtkNew vtkKWCheckButtonWithLabel]
  $o(paintOver) SetParent [$this getOptionsFrame]
  $o(paintOver) Create
  $o(paintOver) SetLabelText "Paint Over: "
  $o(paintOver) SetBalloonHelpString "Allow effect to overwrite non-zero labels."
  [$o(paintOver) GetLabel] SetWidth 22
  [$o(paintOver) GetLabel] SetAnchorToEast
  pack [$o(paintOver) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 2 

  set o(paintThreshold) [vtkNew vtkKWCheckButtonWithLabel]
  $o(paintThreshold) SetParent [$this getOptionsFrame]
  $o(paintThreshold) Create
  $o(paintThreshold) SetLabelText "Threshold Painting: "
  [$o(paintThreshold) GetLabel] SetWidth 22
  [$o(paintThreshold) GetLabel] SetAnchorToEast
  $o(paintThreshold) SetBalloonHelpString "Enable/Disable threshold mode for labeling."
  pack [$o(paintThreshold) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 2 


  set o(paintRange) [vtkNew vtkKWRange]
  $o(paintRange) SetParent [$this getOptionsFrame]
  $o(paintRange) Create
  $o(paintRange) SetLabelText "Threshold"
  $o(paintRange) SetReliefToGroove
  $o(paintRange) SetBalloonHelpString "In threshold mode, the label will only be set if the background value is within this range."
  pack [$o(paintRange) GetWidgetName] \
    -side top -anchor e -fill x -padx 2 -pady 2 

  if { [$this getInputBackground] != "" } {
    set range [[$this getInputBackground] GetScalarRange]
    eval $o(paintRange) SetWholeRange $range
  }

  #
  # event observers 
  #
  set SelectedStateChangedEvent 10000
  set RangeValueChangedEvent 10001
  $::slicer3::Broker AddObservation [$o(paintOver) GetWidget] $SelectedStateChangedEvent "$this processEvent $o(paintOver)"
  $::slicer3::Broker AddObservation [$o(paintThreshold) GetWidget] $SelectedStateChangedEvent "$this processEvent $o(paintThreshold)"
  $::slicer3::Broker AddObservation $o(paintRange) $RangeValueChangedEvent "$this processEvent $o(paintRange)"

}

itcl::body Labeler::tearDownOptions { } {
  chain
  if { [info exists o(paintOver)] } {
    foreach w "paintOver paintThreshold paintRange" {
      if { [info exists o($w)] } {
        $o($w) SetParent ""
        pack forget [$o($w) GetWidgetName] 
      }
    }
  }
}

itcl::body Labeler::updateMRMLFromGUI { } {
  #
  # set the node to the current value of the GUI
  # - this will be saved/restored with the scene
  # - all instances of the effect are observing the node,
  #   so changes will propogate automatically
  #
  chain
  set node [EditorGetParameterNode]
  $this configure -paintOver [[$o(paintOver) GetWidget] GetSelectedState]
  $node SetParameter "Labeler,paintOver" $paintOver
  $this configure -paintThreshold [[$o(paintThreshold) GetWidget] GetSelectedState]
  $node SetParameter "Labeler,paintThreshold" $paintThreshold
  foreach {Min Max} [$o(paintRange) GetRange] {}
  $this configure -paintThresholdMin $Min
  $node SetParameter "Labeler,paintThresholdMin" $Min
  $this configure -paintThresholdMax $Max
  $node SetParameter "Labeler,paintThresholdMax" $Max
}

itcl::body Labeler::setMRMLDefaults { } {
  chain
  set node [EditorGetParameterNode]
  foreach {param default} {
    paintOver 1
    paintThreshold 0
    paintThresholdMin 0
    paintThresholdMax 1000
  } {
    set pvalue [$node GetParameter Labeler,$param] 
    if { $pvalue == "" } {
      $node SetParameter Labeler,$param $default
    } 
  }
}

itcl::body Labeler::updateGUIFromMRML { } {

  #
  # get the gui to the parameter from the node
  #
  chain

  set node [EditorGetParameterNode]

  # set the GUI and effect parameters to match node
  # (only if this is the instance that "owns" the GUI
  set paintOver [$node GetParameter "Labeler,paintOver"]
  $this configure -paintOver $paintOver
  if { [info exists o(paintOver)] } {
    if { [[$o(paintOver) GetWidget] GetSelectedState] != $paintOver } {
      [$o(paintOver) GetWidget] SetSelectedState $paintOver
    }
  }
  set paintThreshold [$node GetParameter "Labeler,paintThreshold"]
  $this configure -paintThreshold $paintThreshold
  if { [info exists o(paintThreshold)] } {
    if { [[$o(paintThreshold) GetWidget] GetSelectedState] != $paintThreshold } {
      [$o(paintThreshold) GetWidget] SetSelectedState $paintThreshold
    }
  }

  set paintThresholdMin [$node GetParameter "Labeler,paintThresholdMin"]
  $this configure -paintThresholdMin $paintThresholdMin
  set paintThresholdMax [$node GetParameter "Labeler,paintThresholdMax"]
  $this configure -paintThresholdMax $paintThresholdMax
  if { [info exists o(paintRange)] } {
    if { [$o(paintRange) GetRange] != "$paintThresholdMin $paintThresholdMax" } {
      $o(paintRange) SetRange $paintThresholdMin $paintThresholdMax
    }
    $o(paintRange) SetEnabled $paintThreshold
  }
}

#
# rotateSliceToImage:
# - adjusts the slice node to align with the 
#   native space of the image data so that paint
#   operations can happen cleanly between image
#   space and screen space
#
itcl::body Labeler::rotateSliceToImage { } {

  $this queryLayers 0 0

  if { [$_sliceNode GetOrientationString] == "Reformat" } {
    puts "Warning: All slice views must be Axial, Sagittal or Coronal."
  } 

  if { $_layers(background,node) == "" ||
        [$_layers(background,node) GetParentTransformNode] != "" ||
        $_layers(label,node) == "" ||
        [$_layers(label,node) GetParentTransformNode] != "" } {
    error "Background and Label Volumes must be selected and cannot be inside transforms."
  } 

  set ijkToRAS [vtkNew vtkMatrix4x4]

  set volumeNode $_layers(background,node)
  if { $volumeNode == "" } {
    return
  }
  $volumeNode GetIJKToRASMatrix $ijkToRAS

  # apply the transform 
  set transformNode [$volumeNode GetParentTransformNode]
  if { $transformNode != "" } {
    if { [$transformNode IsTransformToWorldLinear] } {
      set rasToRAS [vtkMatrix4x4 New]
      $transformNode GetMatrixTransformToWorld $rasToRAS
      $rasToRAS Multiply4x4 $rasToRAS $ijkToRAS $ijkToRAS
      $rasToRAS Delete
    } else {
      error "Cannot handle non-linear transforms"
    }
  }

  foreach col {0 1 2} ijk {i j k} {
    set len$col 0
    foreach row {0 1 2} {
      set ele [$ijkToRAS GetElement $row $col]
      set len$col [expr [set len$col] + $ele * $ele]
    }
    set len$col [expr [set len$col] / sqrt( [set len$col] )]
    foreach row {0 1 2} {
      lappend ${ijk}ToRAS [expr [$ijkToRAS GetElement $row $col] / [set len$col]]
    }
  }

  foreach index {i j k} {
    foreach element {0 1 2} {
      lappend neg${index}ToRAS [expr -1. * [lindex [set ${index}ToRAS] $element]]
    }
  }

  # define major direction
  set dir(directions) {R L A P S I}
  set dir(R) {  1  0  0 }
  set dir(L) { -1  0  0 }
  set dir(A) {  0  1  0 }
  set dir(P) {  0 -1  0 }
  set dir(S) {  0  0  1 }
  set dir(I) {  0  0 -1 }

  # find the dot products for each index vector and max
  set maxdot -1
  set principleIndex -1
  set principleDirection -1
  foreach index {i j k} {
    set dir($index,closestAxisDot) -1
    foreach direction $dir(directions) {
      set dot 0
      foreach comp0 [set ${index}ToRAS] comp1 $dir($direction) {
        set dot [expr $dot + $comp0 * $comp1] 
      }
      set dir(dot,$index,$direction) $dot
      if { $dot > $maxdot } {
        set maxdot $dot
        set principleIndex $index
        set principleDirection $direction
      }
    }
  }
  set dir($principleIndex,closestAxisDot) $maxdot
  set dir($principleIndex,closestAxis) $principleDirection

  # find the closest major direction for each index vector
  foreach index {i j k} {
    if { $index == $principleIndex } {
      continue
    }
    set dir($index,closestAxisDot) -1
    foreach direction $dir(directions) {
      if { $direction == $principleDirection } {
        continue
      }
      if { $dot > $dir($index,closestAxisDot) } {
        set dir($index,closestAxis) $direction
        set dir($index,closestAxisDot) $dot
      }
    }
  }

  set minorAxes [lremove {i j k} $principleIndex]
  foreach {m0 m1} $minorAxes {}
  if { $dir($m0,closestAxis) == $dir($m1,closestAxis) } {
    if { $dir($m0,closestAxisDot) > $dir($m1,closestAxisDot) } {
      set minor $m1
      set middle $m0
    } else {
      set minor $m0
      set middle $m1
    }
    set candidateAxes [lremove $dir(directions) $dir($principleIndex,closestAxis) $dir($middle,closestAxis)]
    # of the remaining axes not 'claimed' by the principle and middle indices, find the best fit to minor
    set maxdot -1
    set dir($minor,closestAxisDot) -1
    foreach direction $candidateAxes {
      if { $dir(dot,$minor,$direction) > $dir($minor,closestAxisDot) } {
        set dir($minor,closestAxis) $direction
        set dir($minor,closestAxisDot) $dir(dot,$minor,$direction)
      }
    }
  }

  # define the index vector that is closest for each major direction
  foreach index {i j k} {
    switch $dir($index,closestAxis) {
      "R" {
        set dir(plane,R) [set ${index}ToRAS]
        set dir(plane,L) [set neg${index}ToRAS]
      }
      "L" {
        set dir(plane,R) [set neg${index}ToRAS]
        set dir(plane,L) [set ${index}ToRAS]
      }
      "A" {
        set dir(plane,A) [set ${index}ToRAS]
        set dir(plane,P) [set neg${index}ToRAS]
      }
      "P" {
        set dir(plane,A) [set neg${index}ToRAS]
        set dir(plane,P) [set ${index}ToRAS]
      }
      "S" {
        set dir(plane,S) [set ${index}ToRAS]
        set dir(plane,I) [set neg${index}ToRAS]
      }
      "I" {
        set dir(plane,S) [set neg${index}ToRAS]
        set dir(plane,I) [set ${index}ToRAS]
      }
    }
  }

  #
  # plug vectors into slice matrix to best approximate requested orientation
  #
  set sliceToRAS [$_sliceNode GetSliceToRAS]

  switch [$_sliceNode GetOrientationString] {
    "Axial" {
      # first column is 'Left'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 0 [lindex $dir(plane,L) $index]
      }
      # second column is 'Anterior'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 1 [lindex $dir(plane,A) $index]
      }
      # third column is 'Superior'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 2 [lindex $dir(plane,S) $index]
      }
    }
    "Sagittal" {
      # first column is 'Posterior'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 0 [lindex $dir(plane,P) $index]
      }
      # second column is 'Superior'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 1 [lindex $dir(plane,S) $index]
      }
      # third column is 'Right'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 2 [lindex $dir(plane,R) $index]
      }
    }
    "Coronal" {
      # first column is 'Left'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 0 [lindex $dir(plane,L) $index]
      }
      # second column is 'Superior'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 1 [lindex $dir(plane,S) $index]
      }
      # third column is 'Anterior'
      foreach index {0 1 2} {
        $sliceToRAS SetElement $index 2 [lindex $dir(plane,A) $index]
      }
    }
  }

  # make sure the slice plane does not lie on an index boundary (to avoid rounding issues)
  [$sliceGUI GetLogic] SnapSliceOffsetToIJK

  after idle $_sliceNode Modified
}

namespace eval Labler {
  proc SetPaintRange {min max} {
    set node [EditorGetParameterNode]
    $node SetParameter Labeler,paintThresholdMin $min
    $node SetParameter Labeler,paintThresholdMax $max
  }
}
