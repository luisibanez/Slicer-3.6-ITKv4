#include "vtkMRMLEMSTreeParametersLeafNode.h"
#include <sstream>
#include "vtkMRMLScene.h"
#include <algorithm>

#include <vtksys/ios/sstream>

//-----------------------------------------------------------------------------
vtkMRMLEMSTreeParametersLeafNode* 
vtkMRMLEMSTreeParametersLeafNode::
New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = 
    vtkObjectFactory::CreateInstance("vtkMRMLEMSTreeParametersLeafNode");
  if(ret)
    {
    return (vtkMRMLEMSTreeParametersLeafNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLEMSTreeParametersLeafNode;
}

//-----------------------------------------------------------------------------
vtkMRMLNode* 
vtkMRMLEMSTreeParametersLeafNode::
CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = 
    vtkObjectFactory::CreateInstance("vtkMRMLEMSTreeParametersLeafNode");
  if(ret)
    {
    return (vtkMRMLEMSTreeParametersLeafNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLEMSTreeParametersLeafNode;
}

//-----------------------------------------------------------------------------
vtkMRMLEMSTreeParametersLeafNode::vtkMRMLEMSTreeParametersLeafNode()
{
  this->NumberOfTargetInputChannels     = 0;
  this->PrintQuality                    = 0;
  this->IntensityLabel                  = 0;
  this->DistributionSpecificationMethod = 0;
}

//-----------------------------------------------------------------------------
vtkMRMLEMSTreeParametersLeafNode::~vtkMRMLEMSTreeParametersLeafNode()
{
}

//-----------------------------------------------------------------------------
void vtkMRMLEMSTreeParametersLeafNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
  vtkIndent indent(nIndent);

  of << indent << "PrintQuality=\""
     << this->PrintQuality << "\" ";

  of << indent << "IntensityLabel=\""
     << this->IntensityLabel << "\" ";

  of << indent << "LogMean=\"";
  for (unsigned int i = 0; i < this->GetNumberOfTargetInputChannels(); ++i)
    {
    of << this->LogMean[i] << " ";
    }
  of << "\" ";

  of << indent << "AutoLogMean=\"";
  for (unsigned int i = 0; i < this->GetNumberOfTargetInputChannels(); ++i)
    {
    of << this->AutoLogMean[i] << " ";
    }
  of << "\" ";



  of << indent << "LogCovariance=\"";
  for (unsigned int r = 0; r < this->GetNumberOfTargetInputChannels(); ++r)
    {
    for (unsigned int c = 0; c < this->GetNumberOfTargetInputChannels(); ++c)  
      {
      of << this->LogCovariance[r][c] << " ";
      }
    if (r < this->GetNumberOfTargetInputChannels() - 1)
      {
      of << "| ";
      }
    }
  of << "\" ";

  of << indent << "AutoLogCovariance=\"";
  for (unsigned int r = 0; r < this->GetNumberOfTargetInputChannels(); ++r)
    {
    for (unsigned int c = 0; c < this->GetNumberOfTargetInputChannels(); ++c)  
      {
      of << this->AutoLogCovariance[r][c] << " ";
      }
    if (r < this->GetNumberOfTargetInputChannels() - 1)
      {
      of << "| ";
      }
    }
  of << "\" ";


  of << indent << "DistributionSpecificationMethod=\""
     << this->DistributionSpecificationMethod << "\" ";

  of << indent << "DistributionSamplePointsRAS=\"";
  for (SamplePointListConstIterator i = 
         this->DistributionSamplePointsRAS.begin();
       i != this->DistributionSamplePointsRAS.end(); ++i)
    {
    of << (*i)[0] << " " << (*i)[1] << " " << (*i)[2] << " ";
    }
  of << "\" ";
}

//-----------------------------------------------------------------------------
void vtkMRMLEMSTreeParametersLeafNode::ReadXMLAttributes(const char** attrs)
{
  Superclass::ReadXMLAttributes(attrs);

  //
  // we assume an even number of attrs
  //
  unsigned int i;
  const char* key;
  const char* val;
  while (*attrs != NULL)
    {
    key = *attrs++;
    val = *attrs++;
    
    if (!strcmp(key, "PrintQuality"))
      {
      vtksys_stl::stringstream ss;
      ss << val;
      ss >> this->PrintQuality;
      }
    else if (!strcmp(key, "IntensityLabel"))
      {
      vtksys_stl::stringstream ss;
      ss << val;
      ss >> this->IntensityLabel;
      }
    else if (!strcmp(key, "LogMean"))
      {
      // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << val;
      double d;
      vtksys_stl::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }

      // update number of input channels
      if (this->GetNumberOfTargetInputChannels() != tmpVec.size())
        {
            this->SetNumberOfTargetInputChannels((int)tmpVec.size());
        }
      
      // copy data
      vtksys_stl::copy(tmpVec.begin(), tmpVec.end(), this->LogMean.begin());
      }
    else if (!strcmp(key, "LogCovariance"))
      {
      // remove visual row seperators
      std::string valStr(val);
      for (i = 0; i < valStr.size(); ++i)
        {
        if (valStr[i] == '|')
          {
          valStr[i] = ' ';
          }
        }

      // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << valStr;
      double d;
      vtksys_stl::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }

      // update number of input channels
      // assume square matrix
      unsigned int side = (unsigned int) sqrt((double)tmpVec.size());
      if (this->GetNumberOfTargetInputChannels() != side)
        {
        this->SetNumberOfTargetInputChannels(side);
        }

      // copy data
      i = 0;
      for (unsigned int r = 0; r < this->GetNumberOfTargetInputChannels(); ++r)
        {
        for (unsigned int c = 0; c<this->GetNumberOfTargetInputChannels(); ++c)
          {
          this->LogCovariance[r][c] = tmpVec[i++];
          }   
        }
      }
    else if (!strcmp(key, "AutoLogMean"))
      {
      // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << val;
      double d;
      vtksys_stl::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }

      // update number of input channels
      if (this->GetNumberOfTargetInputChannels() != tmpVec.size())
        {
            this->SetNumberOfTargetInputChannels((int)tmpVec.size());
        }
      
      // copy data
      vtksys_stl::copy(tmpVec.begin(), tmpVec.end(), this->AutoLogMean.begin());
      }
    else if (!strcmp(key, "AutoLogCovariance"))
      {
      // remove visual row seperators
      std::string valStr(val);
      for (i = 0; i < valStr.size(); ++i)
        {
        if (valStr[i] == '|')
          {
          valStr[i] = ' ';
          }
        }

      // read data into a temporary vector
      vtksys_stl::stringstream ss;
      ss << valStr;
      double d;
      vtksys_stl::vector<double> tmpVec;
      while (ss >> d)
        {
        tmpVec.push_back(d);
        }

      // update number of input channels
      // assume square matrix
      unsigned int side = (unsigned int) sqrt((double)tmpVec.size());
      if (this->GetNumberOfTargetInputChannels() != side)
        {
        this->SetNumberOfTargetInputChannels(side);
        }

      // copy data
      i = 0;
      for (unsigned int r = 0; r < this->GetNumberOfTargetInputChannels(); ++r)
        {
        for (unsigned int c = 0; c<this->GetNumberOfTargetInputChannels(); ++c)
          {
          this->AutoLogCovariance[r][c] = tmpVec[i++];
          }   
        }
      }
    else if (!strcmp(key, "DistributionSpecificationMethod"))
      {
      vtksys_stl::stringstream ss;
      ss << val;
      ss >> this->DistributionSpecificationMethod;
      }
    else if (!strcmp(key, "DistributionSamplePointsRAS"))
      {
      this->DistributionSamplePointsRAS.clear();
      vtksys_stl::stringstream ss;
      ss << val;
      vtkstd::vector<double> point(3);
      while (ss >> point[0] >> point[1] >> point[2])
        {
        this->DistributionSamplePointsRAS.push_back(point);
        }
      }
    }
}

//-----------------------------------------------------------------------------
void vtkMRMLEMSTreeParametersLeafNode::Copy(vtkMRMLNode *rhs)
{
  Superclass::Copy(rhs);
  vtkMRMLEMSTreeParametersLeafNode* node = 
    (vtkMRMLEMSTreeParametersLeafNode*) rhs;

  this->SetNumberOfTargetInputChannels(node->GetNumberOfTargetInputChannels());
  this->SetPrintQuality(node->PrintQuality);
  this->SetIntensityLabel(node->IntensityLabel);
  this->LogMean = node->LogMean;
  this->LogCovariance = node->LogCovariance;
  this->AutoLogMean = node->AutoLogMean;
  this->AutoLogCovariance = node->AutoLogCovariance;
  this->SetDistributionSpecificationMethod
    (node->DistributionSpecificationMethod);
  this->DistributionSamplePointsRAS = node->DistributionSamplePointsRAS;
}

//-----------------------------------------------------------------------------
void vtkMRMLEMSTreeParametersLeafNode::PrintSelf(ostream& os, 
                                                 vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PrintQuality: "
     << this->PrintQuality
     << "\n";

  os << indent << "IntensityLabel: "
     << this->IntensityLabel
     << "\n";

  os << indent << "LogMean: ";
  for (unsigned int i = 0; i < this->GetNumberOfTargetInputChannels(); ++i)
    {
    os << this->LogMean[i] << " ";
    }
  os << "\n";

  os << indent << "AutoLogMean: ";
  for (unsigned int i = 0; i < this->GetNumberOfTargetInputChannels(); ++i)
    {
    os << this->AutoLogMean[i] << " ";
    }
  os << "\n";

  os << indent << "LogCovariance: ";
  for (unsigned int r = 0; r < this->GetNumberOfTargetInputChannels(); ++r)
    {
    for (unsigned int c = 0; c < this->GetNumberOfTargetInputChannels(); ++c)
      {
      os << this->LogCovariance[r][c] << " ";
      }
    }
  os << "\n";

  os << indent << "AutoLogCovariance: ";
  for (unsigned int r = 0; r < this->GetNumberOfTargetInputChannels(); ++r)
    {
    for (unsigned int c = 0; c < this->GetNumberOfTargetInputChannels(); ++c)
      {
      os << this->AutoLogCovariance[r][c] << " ";
      }
    }
  os << "\n";

  os << indent << "DistributionSpecificationMethod: "
     << this->DistributionSpecificationMethod
     << "\n";

  os << indent << "DistributionSamplePointsRAS: \n";
  for (SamplePointListConstIterator i = 
         this->DistributionSamplePointsRAS.begin();
       i != this->DistributionSamplePointsRAS.end(); ++i)
    {
    os << (*i)[0] << " " << (*i)[1] << " " << (*i)[2] << "\n";
    }
}

//-----------------------------------------------------------------------------
void 
vtkMRMLEMSTreeParametersLeafNode::
AddChildNode(const char* vtkNotUsed(childNodeID))
{
  // nothing to do here...
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
RemoveNthChildNode(int vtkNotUsed(n))
{
  // nothing to do here...
}

//-----------------------------------------------------------------------------
void 
vtkMRMLEMSTreeParametersLeafNode::
MoveNthChildNode(int vtkNotUsed(fromIndex), int vtkNotUsed(toIndex))
{
  // nothing to do here...
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
SetNumberOfTargetInputChannels(unsigned int n)
{
  if (n != this->GetNumberOfTargetInputChannels())
    {
    this->NumberOfTargetInputChannels = n;
      
    // resize LogMean, don\"t preserve data!
    this->LogMean.resize(n);
    vtksys_stl::fill(this->LogMean.begin(), this->LogMean.end(), 0.0);
    this->AutoLogMean.resize(n);
    vtksys_stl::fill(this->AutoLogMean.begin(), this->AutoLogMean.end(), 0.0);

    // resize LogCovariance, don't preserve data!
    this->LogCovariance.clear();
    this->LogCovariance.resize(n);
    this->AutoLogCovariance.clear();
    this->AutoLogCovariance.resize(n);
    for (unsigned int i = 0; i < n; ++i)
      {
      this->LogCovariance[i].resize(n, 0.0);
      this->AutoLogCovariance[i].resize(n, 0.0);
      }
    }

}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
AddTargetInputChannel()
{
  this->LogMean.push_back(0.0);
  this->AutoLogMean.push_back(0.0);
  for (unsigned int i = 0; i < this->NumberOfTargetInputChannels; ++i)
    {
    this->LogCovariance[i].push_back(0.0);
    this->AutoLogCovariance[i].push_back(0.0);
    }
  ++this->NumberOfTargetInputChannels; 
  this->LogCovariance.push_back(vtkstd::vector<double>(this->NumberOfTargetInputChannels, 0.0));
  this->AutoLogCovariance.push_back(vtkstd::vector<double>(this->NumberOfTargetInputChannels, 0.0));
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
RemoveNthTargetInputChannel(int index)
{
  this->LogMean.erase(this->LogMean.begin() + index);
  this->AutoLogMean.erase(this->AutoLogMean.begin() + index);
  for (unsigned int i = 0; i < this->NumberOfTargetInputChannels; ++i)
    {
    this->LogCovariance[i].erase(this->LogCovariance[i].begin() + index);
    this->AutoLogCovariance[i].erase(this->AutoLogCovariance[i].begin() + index);
    }
  this->LogCovariance.erase(this->LogCovariance.begin() + index);
  this->AutoLogCovariance.erase(this->AutoLogCovariance.begin() + index);
  --this->NumberOfTargetInputChannels;
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
MoveNthTargetInputChannel(int fromIndex, int toIndex)
{
  double movingParam = this->LogMean[fromIndex];
  this->LogMean.erase(this->LogMean.begin() + fromIndex);
  this->LogMean.insert(this->LogMean.begin() + toIndex, movingParam);

  movingParam = this->AutoLogMean[fromIndex];
  this->AutoLogMean.erase(this->AutoLogMean.begin() + fromIndex);
  this->AutoLogMean.insert(this->AutoLogMean.begin() + toIndex, movingParam);

  for (unsigned int i = 0; i < this->NumberOfTargetInputChannels; ++i)
    {
    movingParam = this->LogCovariance[i][fromIndex];
    this->LogCovariance[i].erase(this->LogCovariance[i].begin() + fromIndex);
    this->LogCovariance[i].insert(this->LogCovariance[i].begin() + toIndex, movingParam);

    movingParam = this->AutoLogCovariance[i][fromIndex];
    this->AutoLogCovariance[i].erase(this->AutoLogCovariance[i].begin() + fromIndex);
    this->AutoLogCovariance[i].insert(this->AutoLogCovariance[i].begin() + toIndex, movingParam);
    }

  vtkstd::vector<double> movingVec = this->LogCovariance[fromIndex];
  this->LogCovariance.erase(this->LogCovariance.begin() + fromIndex);
  this->LogCovariance.insert(this->LogCovariance.begin() + toIndex, movingVec);

  movingVec = this->AutoLogCovariance[fromIndex];
  this->AutoLogCovariance.erase(this->AutoLogCovariance.begin() + fromIndex);
  this->AutoLogCovariance.insert(this->AutoLogCovariance.begin() + toIndex, movingVec);
}

//-----------------------------------------------------------------------------
double vtkMRMLEMSTreeParametersLeafNode::GetLogMean(int index) const
{
  return this->LogMean[index];
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
SetLogMean(int index, double value)
{
  if (value != this->LogMean[index] ) 
    {
      this->LogMean[index] = value;
      this->Modified();
    }
}


//-----------------------------------------------------------------------------
double vtkMRMLEMSTreeParametersLeafNode::GetAutoLogMean(int index) const
{
  return this->AutoLogMean[index];
}



//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
SetAutoLogMean(int index, double value)
{
  if (value != this->AutoLogMean[index] ) 
    {
      this->AutoLogMean[index] = value;
      this->Modified();
    }
}

//-----------------------------------------------------------------------------
double
vtkMRMLEMSTreeParametersLeafNode::
GetLogCovariance(int row, int column) const
{
  return this->LogCovariance[row][column];
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
SetLogCovariance(int row, int column, double value)
{
  if (value != this->LogCovariance[row][column])
    { 
      this->LogCovariance[row][column] = value;
      this->Modified();
    }
}

//-----------------------------------------------------------------------------
double
vtkMRMLEMSTreeParametersLeafNode::
GetAutoLogCovariance(int row, int column) const
{
  return this->AutoLogCovariance[row][column];
}

//-----------------------------------------------------------------------------
void
vtkMRMLEMSTreeParametersLeafNode::
SetAutoLogCovariance(int row, int column, double value)
{
  if (value != this->AutoLogCovariance[row][column])
    { 
      this->AutoLogCovariance[row][column] = value;
      this->Modified();
    }
}

void vtkMRMLEMSTreeParametersLeafNode::CopyAutoLogCovarianceToLogCovariance()
{
  int diffFlag = 0;
  if (this->AutoLogCovariance.size() != this->LogCovariance.size()) 
    {
      diffFlag = 1;
      this->LogCovariance.resize(this->AutoLogCovariance.size());
    } 

  for (unsigned int r = 0; r < this->AutoLogCovariance.size(); ++r)
    {
      if (this->AutoLogCovariance[r] != this->LogCovariance[r]) 
    {
       diffFlag = 1;
       this->LogCovariance[r] = this->AutoLogCovariance[r];
    }
    }
  if (diffFlag) 
    {
      this->Modified();
    }
}

void vtkMRMLEMSTreeParametersLeafNode::CopyAutoLogMeanToLogMean()
{
  if (this->AutoLogMean != this->LogMean) 
    {
      this->LogMean = this->AutoLogMean;
      this->Modified();
    }
}

//-----------------------------------------------------------------------------
int 
vtkMRMLEMSTreeParametersLeafNode::
GetNumberOfSamplePoints() const
{
  return (int) this->DistributionSamplePointsRAS.size();
}

//-----------------------------------------------------------------------------
void 
vtkMRMLEMSTreeParametersLeafNode::
AddSamplePoint(double xyz[3])
{
  PointType point(3);
  point[0] = xyz[0];
  point[1] = xyz[1];
  point[2] = xyz[2];
  this->DistributionSamplePointsRAS.push_back(point);
}

//-----------------------------------------------------------------------------
void 
vtkMRMLEMSTreeParametersLeafNode::
RemoveNthSamplePoint(int n)
{
  this->DistributionSamplePointsRAS.erase
    (this->DistributionSamplePointsRAS.begin()+n);
}

//-----------------------------------------------------------------------------
void 
vtkMRMLEMSTreeParametersLeafNode::
ClearSamplePoints()
{
  this->DistributionSamplePointsRAS.clear();
}

//-----------------------------------------------------------------------------
void 
vtkMRMLEMSTreeParametersLeafNode::
GetNthSamplePoint(int n, double xyz[3]) const
{
  if (n >= static_cast<int>(this->DistributionSamplePointsRAS.size()))
    {
    std::cerr << "ERROR: invalid sample number" << n << std::endl;
    }
  xyz[0] = this->DistributionSamplePointsRAS[n][0];
  xyz[1] = this->DistributionSamplePointsRAS[n][1];
  xyz[2] = this->DistributionSamplePointsRAS[n][2];
}

