#ifndef __vtkMRMLEMSWorkingDataNode_h
#define __vtkMRMLEMSWorkingDataNode_h

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLEMS.h"
#include "vtkMRMLEMSVolumeCollectionNode.h"

class vtkMRMLEMSTargetNode;
class vtkMRMLEMSAtlasNode;

class VTK_MRMLEMS_EXPORT vtkMRMLEMSWorkingDataNode : 
  public vtkMRMLEMSVolumeCollectionNode
{
public:
  static vtkMRMLEMSWorkingDataNode *New();
  vtkTypeMacro(vtkMRMLEMSWorkingDataNode,vtkMRMLEMSVolumeCollectionNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes(const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "EMSWorkingData";}

  // Description:
  // Updates this node if it depends on other nodes
  // when the node is deleted in the scene
  virtual void UpdateReferences();

  // Description:
  // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

  vtkGetStringMacro(InputTargetNodeID);
  void SetInputTargetNodeID(const char *input);
  vtkMRMLEMSTargetNode* GetInputTargetNode();

  vtkGetStringMacro(NormalizedTargetNodeID);
  void SetNormalizedTargetNodeID(const char *input);
  vtkMRMLEMSTargetNode* GetNormalizedTargetNode();

  vtkGetStringMacro(AlignedTargetNodeID);
  void SetAlignedTargetNodeID(const char *input);
  vtkMRMLEMSTargetNode* GetAlignedTargetNode();
  
  vtkGetStringMacro(InputAtlasNodeID);
  void SetInputAtlasNodeID(const char *input);
  vtkMRMLEMSAtlasNode* GetInputAtlasNode();

  vtkGetStringMacro(AlignedAtlasNodeID);
  void SetAlignedAtlasNodeID(const char *input);
  vtkMRMLEMSAtlasNode* GetAlignedAtlasNode();

  //  vtkMRMLEMSAtlasNode*  GetWorkingAtlasNode();
  //vtkMRMLEMSTargetNode* GetWorkingTargetNode();

  vtkGetMacro(InputTargetNodeIsValid, int);
  vtkSetMacro(InputTargetNodeIsValid, int);

  vtkGetMacro(NormalizedTargetNodeIsValid, int);
  vtkSetMacro(NormalizedTargetNodeIsValid, int);

  vtkGetMacro(AlignedTargetNodeIsValid, int);
  vtkSetMacro(AlignedTargetNodeIsValid, int);

  vtkGetMacro(InputAtlasNodeIsValid, int);
  vtkSetMacro(InputAtlasNodeIsValid, int);

  vtkGetMacro(AlignedAtlasNodeIsValid, int);
  vtkSetMacro(AlignedAtlasNodeIsValid, int);

protected:
  vtkMRMLEMSWorkingDataNode();
  ~vtkMRMLEMSWorkingDataNode();
  vtkMRMLEMSWorkingDataNode(const vtkMRMLEMSWorkingDataNode&);
  void operator=(const vtkMRMLEMSWorkingDataNode&);

  char*                InputTargetNodeID;
  char*                NormalizedTargetNodeID;
  char*                AlignedTargetNodeID;
  char*                InputAtlasNodeID;
  char*                AlignedAtlasNodeID;

  int                  InputTargetNodeIsValid;
  int                  NormalizedTargetNodeIsValid;  
  int                  AlignedTargetNodeIsValid;  
  int                  InputAtlasNodeIsValid;  
  int                  AlignedAtlasNodeIsValid;  
};

#endif
