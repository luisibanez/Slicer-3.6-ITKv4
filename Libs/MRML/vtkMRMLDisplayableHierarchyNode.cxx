/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLDisplayableHierarchyNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.3 $

=========================================================================auto=*/
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLDisplayableHierarchyNode.h"
#include "vtkMRMLScene.h"
#include "vtkSmartPointer.h"

//------------------------------------------------------------------------------
vtkMRMLDisplayableHierarchyNode* vtkMRMLDisplayableHierarchyNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDisplayableHierarchyNode");
  if(ret)
    {
    return (vtkMRMLDisplayableHierarchyNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDisplayableHierarchyNode;
}

//-----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLDisplayableHierarchyNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLDisplayableHierarchyNode");
  if(ret)
    {
    return (vtkMRMLDisplayableHierarchyNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLDisplayableHierarchyNode;
}


//----------------------------------------------------------------------------
vtkMRMLDisplayableHierarchyNode::vtkMRMLDisplayableHierarchyNode()
{
  this->DisplayableNodeID = NULL;
  this->DisplayNodeID = NULL;
  this->DisplayNode = NULL;
  this->HideFromEditors = 1;
  this->Expanded = 1;
}

//----------------------------------------------------------------------------
vtkMRMLDisplayableHierarchyNode::~vtkMRMLDisplayableHierarchyNode()
{
  if (this->DisplayableNodeID) 
    {
    delete [] this->DisplayableNodeID;
    this->DisplayableNodeID = NULL;
    }
  this->SetAndObserveDisplayNodeID( NULL);
}

//----------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

   if (this->DisplayableNodeID != NULL) 
    {
    of << indent << " displayableNodeID=\"" << this->DisplayableNodeID << "\"";
    }
  if (this->DisplayNodeID != NULL) 
    {
    of << indent << " displayNodeID=\"" << this->DisplayNodeID << "\"";
    }

  of << indent << " expanded=\"" << (this->Expanded ? "true" : "false") << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
  if (this->DisplayableNodeID == NULL || !strcmp(oldID, this->DisplayableNodeID))
    {
    this->SetDisplayableNodeID(newID);
    }
  if (this->DisplayNodeID == NULL || !strcmp(oldID, this->DisplayNodeID))
    {
    this->SetDisplayNodeID(newID);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "displayableNodeID")) 
      {
      this->SetDisplayableNodeID(attValue);
      //this->Scene->AddReferencedNodeID(this->DisplayableNodeID, this);
      }
    else if (!strcmp(attName, "displayNodeRef") ||
             !strcmp(attName, "displayNodeID")) 
      {
      this->SetDisplayNodeID(attValue);
      //this->Scene->AddReferencedNodeID(this->DisplayNodeID, this);
      }
    else if (!strcmp(attName, "expanded")) 
        {
        if (!strcmp(attValue,"true")) 
          {
          this->Expanded = 1;
          }
        else
          {
          this->Expanded = 0;
          }
        }
    }

  this->EndModify(disabledModify);
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ID
void vtkMRMLDisplayableHierarchyNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLDisplayableHierarchyNode *node = (vtkMRMLDisplayableHierarchyNode *) anode;

  this->SetDisplayableNodeID(node->DisplayableNodeID);
  this->SetDisplayNodeID(node->DisplayNodeID);
  this->SetExpanded(node->Expanded);
  this->EndModify(disabledModify);

}

//----------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  Superclass::PrintSelf(os,indent);

  os << indent << "DisplayableNodeID: " <<
    (this->DisplayableNodeID ? this->DisplayableNodeID : "(none)") << "\n";

  os << indent << "DisplayNodeID: " <<
    (this->DisplayNodeID ? this->DisplayNodeID : "(none)") << "\n";
  os << indent << "Expanded:        " << this->Expanded << "\n";

  vtkSmartPointer<vtkCollection> col =  vtkSmartPointer<vtkCollection>::New();
  this->GetChildrenDisplayableNodes(col);
  unsigned int numChildren = col->GetNumberOfItems();
  os << indent << "Number of children displayable nodes = " << numChildren << "\n";
  for (unsigned int i = 0; i < numChildren; i++)
    {
    vtkMRMLDisplayableNode *child = vtkMRMLDisplayableNode::SafeDownCast(col->GetItemAsObject(i));
    if (child)
      {
      os << indent.GetNextIndent() << i << "th child id = " << (child->GetID() ? child->GetID() : "NULL") << "\n";
      }
    }
}

//-----------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::UpdateScene(vtkMRMLScene *scene)
{
  Superclass::UpdateScene(scene);
  this->SetAndObserveDisplayNodeID(this->GetDisplayNodeID());

}

//-----------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (this->Scene == NULL)
    {
    return;
    }
  if (this->DisplayNodeID != NULL && this->Scene->GetNodeByID(this->DisplayNodeID) == NULL)
    {
    this->SetAndObserveDisplayNodeID(NULL);
    }
  if (this->DisplayableNodeID != NULL && this->Scene->GetNodeByID(this->DisplayableNodeID) == NULL)
    {
    this->SetDisplayableNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
vtkMRMLDisplayableNode* vtkMRMLDisplayableHierarchyNode::GetDisplayableNode()
{
  vtkMRMLDisplayableNode* node = NULL;
  if (this->GetScene() && this->GetDisplayableNodeID() )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->DisplayableNodeID);
    node = vtkMRMLDisplayableNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
vtkMRMLDisplayNode* vtkMRMLDisplayableHierarchyNode::GetDisplayNode()
{
  vtkMRMLDisplayNode* node = NULL;
  if (this->GetScene() && this->GetDisplayNodeID() )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->DisplayNodeID);
    node = vtkMRMLDisplayNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::SetAndObserveDisplayNodeID(const char *displayNodeID)
{
  vtkSetAndObserveMRMLObjectMacro(this->DisplayNode, NULL);

  this->SetDisplayNodeID(displayNodeID);

  vtkMRMLDisplayNode *dnode = this->GetDisplayNode();

  vtkSetAndObserveMRMLObjectMacro(this->DisplayNode, dnode);

}


//---------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, 
                                           void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  vtkMRMLDisplayNode *dnode = this->GetDisplayNode();
  if (dnode != NULL && dnode == vtkMRMLDisplayNode::SafeDownCast(caller) &&
      event ==  vtkCommand::ModifiedEvent)
    {
    this->InvokeEvent(vtkCommand::ModifiedEvent, NULL);
    }
  return;
}

//----------------------------------------------------------------------------
vtkMRMLDisplayableHierarchyNode* vtkMRMLDisplayableHierarchyNode::GetUnExpandedParentNode()
{
  vtkMRMLDisplayableHierarchyNode *node = NULL;
  if (!this->GetExpanded()) 
    {
    node = this;
    }
  else 
    {
    vtkMRMLDisplayableHierarchyNode *parent = vtkMRMLDisplayableHierarchyNode::SafeDownCast(this->GetParentNode());
    if (parent)
      {
      node =  parent->GetUnExpandedParentNode();
      }
    else
      {
      node =  NULL;
      }
    }
  return node;
}

//----------------------------------------------------------------------------
vtkMRMLDisplayableHierarchyNode* vtkMRMLDisplayableHierarchyNode::GetTopParentNode()
{
  vtkMRMLDisplayableHierarchyNode *node = NULL;
  vtkMRMLDisplayableHierarchyNode *parent = vtkMRMLDisplayableHierarchyNode::SafeDownCast(this->GetParentNode());
  if (parent == NULL) 
    {
    node = this;
    }
  else 
    {
    node =  parent->GetTopParentNode();
    }
  return node;
}


  
//---------------------------------------------------------------------------
void vtkMRMLDisplayableHierarchyNode::GetChildrenDisplayableNodes(vtkCollection *children)
{
  if (children == NULL)
    {
    return;
    }
  vtkMRMLScene *scene = this->GetScene();
  if (scene == NULL)
    {
    vtkErrorMacro("GetChildrenDisplayableNodes: scene is null, cannot find children of this node");
    return;
    }
  vtkMRMLNode *mnode = NULL;
  vtkMRMLDisplayableHierarchyNode *hnode = NULL;
  for (int n=0; n < scene->GetNumberOfNodes(); n++) 
    {
    mnode = scene->GetNthNode(n);
    if (mnode->IsA("vtkMRMLDisplayableNode"))
      {
      hnode = vtkMRMLDisplayableHierarchyNode::GetDisplayableHierarchyNode(scene, mnode->GetID());
      while (hnode)
        {
        if (hnode == this) 
          {
          children->AddItem(mnode);
          break;
          }
          hnode = vtkMRMLDisplayableHierarchyNode::SafeDownCast(this->GetParentNode());
        }// end while
      }// end if
    }// end for
}


//---------------------------------------------------------------------------
vtkMRMLDisplayableHierarchyNode* vtkMRMLDisplayableHierarchyNode::GetDisplayableHierarchyNode(vtkMRMLScene *scene,
                                                                            const char *displayableNodeID)
{
  if (scene == NULL ||
      displayableNodeID == NULL)
    {
    return NULL;
    }

  int nnodes = scene->GetNumberOfNodesByClass("vtkMRMLDisplayableHierarchyNode");
  for (int i=0; i<nnodes; i++)
    {
    vtkMRMLDisplayableHierarchyNode *node =  vtkMRMLDisplayableHierarchyNode::SafeDownCast(scene->GetNthNodeByClass(i, "vtkMRMLDisplayableHierarchyNode"));
    if (node)
      {
      vtkMRMLDisplayableNode *mnode = node->GetDisplayableNode();
      if (mnode && !strcmp(mnode->GetID(), displayableNodeID))
        {
        return node;
        }
      }
    }
  return NULL;
}
