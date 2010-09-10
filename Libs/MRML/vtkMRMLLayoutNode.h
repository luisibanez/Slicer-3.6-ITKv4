#ifndef __vtkMRMLLayoutNode_h
#define __vtkMRMLLayoutNode_h


#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkObject.h"

class VTK_MRML_EXPORT vtkMRMLLayoutNode : public vtkMRMLNode
{
public:
  static vtkMRMLLayoutNode *New();
  vtkTypeMacro(vtkMRMLLayoutNode,vtkMRMLNode);
  virtual vtkMRMLNode* CreateNodeInstance();
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  /// MRML methods
  //--------------------------------------------------------------------------

  /// 
  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  /// 
  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  /// 
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);
  
  /// Get/Set for Current layout
  vtkGetMacro (ViewArrangement, int );
  virtual void SetViewArrangement ( int );

  vtkGetMacro (GUIPanelVisibility, int);
  vtkSetMacro (GUIPanelVisibility, int);
  
  vtkGetMacro ( BottomPanelVisibility, int );
  vtkSetMacro (BottomPanelVisibility, int );

  /// 0 is Leftside, 1 is right side
  vtkGetMacro (GUIPanelLR, int );
  vtkSetMacro (GUIPanelLR, int );

  /// 
  /// Control the collapse state of the SliceControllers
  vtkGetMacro (CollapseSliceControllers, int);
  vtkSetMacro (CollapseSliceControllers, int);

  /// 
  /// CompareView configuration Get/Set methods
  vtkGetMacro ( NumberOfCompareViewRows, int );
  vtkSetClampMacro ( NumberOfCompareViewRows, int, 1, 50);  
  vtkGetMacro ( NumberOfCompareViewColumns, int );
  vtkSetClampMacro ( NumberOfCompareViewColumns, int, 1, 50 );  

  /// 
  /// configure each compare viewer in lightbox mode.
  vtkGetMacro ( NumberOfCompareViewLightboxRows, int );
  vtkSetClampMacro ( NumberOfCompareViewLightboxRows, int, 1, 50 );  
  vtkGetMacro ( NumberOfCompareViewLightboxColumns, int );
  vtkSetClampMacro ( NumberOfCompareViewLightboxColumns, int, 1, 50 );  

  /// 
  /// Set/Get the size of the main and secondary panels (size of Frame1
  /// in each panel)
  vtkGetMacro( MainPanelSize, int);
  vtkSetMacro( MainPanelSize, int);
  vtkGetMacro( SecondaryPanelSize, int);
  vtkSetMacro( SecondaryPanelSize, int);


  /// 
  /// Set/Get the size of the last selected module
  vtkGetStringMacro( SelectedModule);
  vtkSetStringMacro( SelectedModule);
  
/// Description:
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Layout";};

  //BTX
  enum
    {
      SlicerLayoutInitialView = 0,
      SlicerLayoutDefaultView,
      SlicerLayoutConventionalView,
      SlicerLayoutFourUpView,
      SlicerLayoutOneUp3DView,
      SlicerLayoutOneUpSliceView,
      SlicerLayoutOneUpRedSliceView,
      SlicerLayoutOneUpYellowSliceView,
      SlicerLayoutOneUpGreenSliceView,
      SlicerLayoutTabbed3DView,
      SlicerLayoutTabbedSliceView,
      SlicerLayoutLightboxView,
      SlicerLayoutCompareView,
      SlicerLayoutSideBySideCompareView,
      SlicerLayoutNone,
      SlicerLayoutDual3DView
    };
  //ETX


protected:
  vtkMRMLLayoutNode();
  ~vtkMRMLLayoutNode();
  vtkMRMLLayoutNode(const vtkMRMLLayoutNode&);
  void operator=(const vtkMRMLLayoutNode&);

  int GUIPanelVisibility;
  int BottomPanelVisibility;
  int GUIPanelLR;
  int CollapseSliceControllers;
  int ViewArrangement;
  int NumberOfCompareViewRows;
  int NumberOfCompareViewColumns;
  int NumberOfCompareViewLightboxRows;
  int NumberOfCompareViewLightboxColumns;

  char *SelectedModule;

  int MainPanelSize;
  int SecondaryPanelSize;
};

#endif
