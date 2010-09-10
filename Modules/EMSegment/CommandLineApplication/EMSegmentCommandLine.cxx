#include <iostream>
#include <vector>
#include <string>

#include "vtkMRMLScene.h"
#include "vtkSlicerColorLogic.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkEMSegmentLogic.h"
#include "vtkEMSegmentMRMLManager.h"
#include "vtkMRMLEMSSegmenterNode.h"


#include "EMSegmentCommandLineCLP.h"

#include <vtksys/SystemTools.hxx>
#include <stdexcept>

#include "vtkImageMathematics.h"
#include "vtkImageAccumulate.h"
#include "vtkITKArchetypeImageSeriesReader.h"
#include "vtkITKArchetypeImageSeriesScalarReader.h"
#include "vtkImageData.h"


// -============================
// This is necessary to load EMSegment package in TCL interp.
// -============================

#include "vtkSlicerApplication.h"
#include "vtkKWTkUtilities.h"

extern "C" int Slicerbasegui_Init(Tcl_Interp *interp);
extern "C" int Emsegment_Init(Tcl_Interp *interp);
extern "C" int Vtkteem_Init(Tcl_Interp *interp);
extern "C" int Vtkitk_Init(Tcl_Interp *interp);
extern "C" int Slicerbaselogic_Init(Tcl_Interp *interp);
extern "C" int Mrml_Init(Tcl_Interp *interp);
extern "C" int Mrmlcli_Init(Tcl_Interp *interp); 
extern "C" int Commandlinemodule_Init(Tcl_Interp *interp);

#define tgVtkCreateMacro(name,type) \
  name  = type::New(); \
  name##Tcl = vtksys::SystemTools::DuplicateString(vtkKWTkUtilities::GetTclNameFromPointer(interp, name)); 

#define tgVtkDefineMacro(name,type) \
  type *name; \
  std::string name##Tcl;\
  tgVtkCreateMacro(name,type); 

#define tgSetDataMacro(name,matrix)               \
 virtual int Set##name(const char *fileName) { \
   if (strcmp(fileName,"None")) { \
    tgVtkCreateMacro(this->name,vtkImageData); \
    this->matrix = vtkMatrix4x4::New(); \
    return tgReadVolume(fileName,this->name,this->matrix);    \
   } \
   this->name = NULL; \
   this->matrix = NULL; \
   std::cout << "Here" << std::endl; \
   return 0; \
 }


vtksys_stl::string tgGetSLICER_HOME(char** argv)  
{ 
  vtksys_stl::string slicerHome = "";
  if ( !vtksys::SystemTools::GetEnv("Slicer3_HOME", slicerHome) )
  {
    std::string programPath;
    std::string errorMessage;
    if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMessage) ) return slicerHome;

    slicerHome = vtksys::SystemTools::GetFilenamePath(programPath.c_str()) + "/../../../";
  } 
  return slicerHome;
}

int tgSetSLICER_HOME(char** argv)  
{ 
  vtksys_stl::string slicerHome = "";
  if ( !vtksys::SystemTools::GetEnv("Slicer3_HOME", slicerHome) )
  {
    std::string programPath;
    std::string errorMessage;

    if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMessage) ) return 1;

    std::string homeEnv = "Slicer3_HOME=";
    homeEnv += vtksys::SystemTools::GetFilenamePath(programPath.c_str()) + "/../../../";
   
    cout << "Set environment: " << homeEnv.c_str() << endl;
    vtkKWApplication::PutEnv(const_cast <char *> (homeEnv.c_str()));
  } else {
    cout << "Slicer3_HOME found: " << slicerHome << endl;
  }
  return 0;
}


// -============================
// END OF TCL Specific Files 
// -============================



// does not acctually read an image from disk, this is intended for
// creating an image that you will later want to write to
vtkMRMLVolumeNode*
AddNewScalarArchetypeVolume(vtkMRMLScene* mrmlScene,
                            const char* filename, 
                            int centerImage, 
                            int labelMap, 
                            const char* volname)
{
  vtkMRMLScalarVolumeNode  *scalarNode   = vtkMRMLScalarVolumeNode::New();
  scalarNode->SetLabelMap(labelMap);
  vtkMRMLVolumeNode        *volumeNode   = scalarNode;

  // i/o mechanism
  vtkMRMLVolumeArchetypeStorageNode *storageNode = 
    vtkMRMLVolumeArchetypeStorageNode::New();
  storageNode->SetFileName(filename);
  storageNode->SetCenterImage(centerImage);

  // set the volume's name
  if (volname == NULL)
    {
    const vtksys_stl::string fname(filename);
    vtksys_stl::string name = vtksys::SystemTools::GetFilenameName(fname);
    volumeNode->SetName(name.c_str());
    }
  else
    {
    volumeNode->SetName(volname);
    }

  // add nodes to scene
  mrmlScene->AddNodeNoNotify(storageNode);  
  mrmlScene->AddNodeNoNotify(volumeNode);

  volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());

  if (scalarNode)
    {
    scalarNode->Delete();
    }
  if (storageNode)
    {
    storageNode->Delete();
    }
  return volumeNode;    
}

vtkMRMLVolumeNode* 
AddScalarArchetypeVolume(vtkMRMLScene* mrmlScene,
                         const char* filename, 
                         int centerImage, 
                         int labelMap, 
                         const char* volname)
{
  vtkMRMLVolumeNode        *volumeNode   = NULL;
  vtkMRMLScalarVolumeDisplayNode *displayNode  = NULL;
  vtkMRMLScalarVolumeNode  *scalarNode   = vtkMRMLScalarVolumeNode::New();

  // i/o mechanism
  vtkMRMLVolumeArchetypeStorageNode *storageNode = 
    vtkMRMLVolumeArchetypeStorageNode::New();
  storageNode->SetFileName(filename);
  storageNode->SetCenterImage(centerImage);

  // try to read the image
  if (storageNode->ReadData(scalarNode))
    {
    displayNode = vtkMRMLScalarVolumeDisplayNode::New();
    scalarNode->SetLabelMap(labelMap);
    volumeNode  = scalarNode;
    }

  if (volumeNode != NULL)
    {
    // set the volume's name
    if (volname == NULL)
      {
      const vtksys_stl::string fname(filename);
      vtksys_stl::string name = vtksys::SystemTools::GetFilenameName(fname);
      volumeNode->SetName(name.c_str());
      }
    else
      {
      volumeNode->SetName(volname);
      }

    // set basic display info
    double range[2];
    volumeNode->GetImageData()->GetScalarRange(range);
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]) );
    displayNode->SetAndObserveColorNodeID("vtkMRMLColorTableNodeGrey");

    // add nodes to scene
    mrmlScene->AddNodeNoNotify(storageNode);  
    mrmlScene->AddNodeNoNotify(displayNode);  
    mrmlScene->AddNodeNoNotify(volumeNode);

    volumeNode->SetAndObserveStorageNodeID(storageNode->GetID());
    volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
    }

  scalarNode->Delete();
  storageNode->Delete();
  if (displayNode)
    {
    displayNode->Delete();
    }
  return volumeNode;  
}

//
// This function checks to see if the image stored in standardFilename
// differs from resultData.  True is returned if the images differ,
// false is returned if they are identical.
bool ImageDiff(vtkImageData* resultData, std::string standardFilename)
{
  bool imagesDiffer = false;

  //
  // read segmentation result standard
  vtkITKArchetypeImageSeriesReader* standardReader = 
    vtkITKArchetypeImageSeriesScalarReader::New();
  standardReader->SetArchetype(standardFilename.c_str());
  standardReader->SetOutputScalarTypeToNative();
  standardReader->SetDesiredCoordinateOrientationToNative();
  standardReader->SetUseNativeOriginOn();
  try
    {
    standardReader->Update();
    }
  catch (...)
    {
    std::cerr << "Error reading standard image: " << std::endl;
    standardReader->Delete();
    return true;
    }

  //
  // compare image origin and spacing
  for (unsigned int i = 0; i < 3; ++i)
    {
    if (resultData->GetSpacing()[i] != 
        standardReader->GetOutput()->GetSpacing()[i] ||
        resultData->GetOrigin()[i] != 
        standardReader->GetOutput()->GetOrigin()[i])
      {
      //
      // display spacing and origin info for resultData
      std::cerr << "Image spacing and/or origin does not match standard!" 
                << std::endl;
      std::cerr << "result origin: " 
                << resultData->GetOrigin()[0] << " "
                << resultData->GetOrigin()[1] << " "
                << resultData->GetOrigin()[2] << std::endl;
      std::cerr << "result spacing: " 
                << resultData->GetSpacing()[0] << " "
                << resultData->GetSpacing()[1] << " "
                << resultData->GetSpacing()[2] << std::endl;

      std::cerr << "Standard origin: " 
                << standardReader->GetOutput()->GetOrigin()[0] << " "
                << standardReader->GetOutput()->GetOrigin()[1] << " "
                << standardReader->GetOutput()->GetOrigin()[2] << std::endl;
      std::cerr << "Standard spacing: " 
                << standardReader->GetOutput()->GetSpacing()[0] << " "
                << standardReader->GetOutput()->GetSpacing()[1] << " "
                << standardReader->GetOutput()->GetSpacing()[2] << std::endl;
      imagesDiffer = true;
      }
    }
  if (!imagesDiffer)
    {
    std::cerr << "Result image origin and spacing match." << std::endl;
    }

  //
  // compare image voxels
  vtkImageMathematics* imageDifference = vtkImageMathematics::New();
  imageDifference->SetOperationToSubtract();
  imageDifference->SetInput1(resultData);
  imageDifference->SetInput2(standardReader->GetOutput());

  vtkImageAccumulate* differenceAccumulator = vtkImageAccumulate::New();
  differenceAccumulator->SetInputConnection(imageDifference->GetOutputPort());
  //differenceAccumulator->IgnoreZeroOn();
  differenceAccumulator->Update();
  
  //imagesDiffer = differenceAccumulator->GetVoxelCount() > 0;
  imagesDiffer = 
    differenceAccumulator->GetMin()[0] != 0.0 ||
    differenceAccumulator->GetMax()[0] != 0.0;
    
  if (imagesDiffer)
    {
    std::cerr << "((temporarily not) ignoring zero) Num / Min / Max / Mean difference = " 
              << differenceAccumulator->GetVoxelCount()  << " / "
              << differenceAccumulator->GetMin()[0]      << " / "
              << differenceAccumulator->GetMax()[0]      << " / "
              << differenceAccumulator->GetMean()[0]     << std::endl;
    }
  else
    {
    std::cerr << "Result image voxels match." << std::endl;
    }

  standardReader->Delete();
  imageDifference->Delete();
  differenceAccumulator->Delete();

  return imagesDiffer;
}

void GenerateEmptyMRMLScene(const char* filename)
{
  // create scene, logic, and add parameters to scene
  vtkMRMLScene* mrmlScene = vtkMRMLScene::New();
  vtkMRMLScene::SetActiveScene(mrmlScene);
  mrmlScene->SetURL(filename);
  
  vtkEMSegmentLogic* emLogic             = vtkEMSegmentLogic::New();
  emLogic->SetModuleName("EMSegment");
  emLogic->SetAndObserveMRMLScene(mrmlScene);
  emLogic->RegisterMRMLNodesWithScene();

  vtkIntArray *emsEvents                 = vtkIntArray::New();
  emsEvents->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  emsEvents->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  emLogic->SetAndObserveMRMLSceneEvents(mrmlScene, emsEvents);
  emsEvents->Delete();    

  emLogic->GetMRMLManager()->CreateAndObserveNewParameterSet();

  // write the scene
  try 
    {
    mrmlScene->Commit();
    }
  catch (...)
    {
    std::cerr << "ERROR: could not write mrml scene." << std::endl;
    }

  // clean up
  mrmlScene->Clear(true);
  mrmlScene->Delete();
  emLogic->SetAndObserveMRMLScene(NULL);
  emLogic->Delete();  
}

std::string StripBackslashes(const std::string& s)
{
  std::string outString;
  for (unsigned int i = 0; i < s.size(); ++i)
    {
    if (s[i] != '\\')
      {
      outString.push_back(s[i]);
      }
    else if (i > 0 && s[i-1] == '\\')
      {
      outString.push_back(s[i]);
      }
    }
  return outString;
}

class ProgressReporter
{
public:
  void ReportProgress(const std::string& message,
                      float totalProgress = 0.0f,
                      float stageProgress = 0.0f)
  {
  // unused variable
  (void)(message);
  (void)(totalProgress);
  (void)(stageProgress);
  }
  
};


int main(int argc, char** argv)
{
  // parse arguments using the CLP system; this creates variables.
  PARSE_ARGS;

  ProgressReporter progressReporter;
  float currentStep = 0.0f;
  float totalSteps  = 6.0f;

  progressReporter.ReportProgress("Parsing Arguments...", 
                                   currentStep++ / totalSteps);

  bool useDefaultParametersNode = parametersMRMLNodeName.empty();
  bool useDefaultTarget         = targetVolumeFileNames.empty();
  bool useDefaultAtlas          = atlasVolumeFileNames.empty();
  bool useDefaultOutput         = resultVolumeFileName.empty();
  bool writeIntermediateResults = !intermediateResultsDirectory.empty();
  bool segmentationSucceeded    = true;

  if (verbose) std::cerr << "Starting EMSegment Command Line." << std::endl;

  if (!generateEmptyMRMLSceneAndQuit.empty())
    {
    GenerateEmptyMRMLScene(generateEmptyMRMLSceneAndQuit.c_str());
    return EXIT_SUCCESS;
    }

#ifdef WIN32
  //
  // strip backslashes from parameter node name (present if spaces were used)
  parametersMRMLNodeName = StripBackslashes(parametersMRMLNodeName);
#endif  

  //
  // make sure arguments are sufficient and unique
  bool argsOK = true;
  if (mrmlSceneFileName.empty())
    {
    std::cerr << "Error: mrmlSceneFileName must be specified." << std::endl;
    argsOK = false;
    }
  if (!argsOK)
    {
    std::cerr << "Try --help for usage..." << std::endl;
    return EXIT_FAILURE;
    }

  //
  // make sure files exist
  if (writeIntermediateResults &&
      !vtksys::SystemTools::FileExists(intermediateResultsDirectory.c_str()))
    {
    std::cerr << "Error: intermediate results directory does not exist." 
              << std::endl;
    std::cerr << intermediateResultsDirectory << std::endl;      
    return EXIT_FAILURE;
    }

  if (!vtksys::SystemTools::FileExists(mrmlSceneFileName.c_str()))
    {
    std::cerr << "Error: MRML scene file does not exist." << std::endl;
    std::cerr << mrmlSceneFileName << std::endl;      
    return EXIT_FAILURE;
    }

  if (!resultStandardVolumeFileName.empty() && 
      !vtksys::SystemTools::FileExists(resultStandardVolumeFileName.c_str()))
    {
    std::cerr << "Error: result standard volume file does not exist." 
              << std::endl;
    std::cerr << resultStandardVolumeFileName << std::endl;   
    return EXIT_FAILURE;
    }

  // the gui uses <image>, the command line uses actual files
  for (unsigned int i = 0; i < targetVolumeFileNames.size(); ++i)
    {
    if (!vtksys::SystemTools::
        FileExists(targetVolumeFileNames[i].c_str()))
      {
      std::cerr << "Error: target volume file " << i << " does not exist." 
                << std::endl;
      std::cerr << targetVolumeFileNames[i] << std::endl;      
      return EXIT_FAILURE;
      }
    }

  for (unsigned int i = 0; i < atlasVolumeFileNames.size(); ++i)
    {
    if (!vtksys::SystemTools::
        FileExists(atlasVolumeFileNames[i].c_str()))
      {
      std::cerr << "Error: atlas volume file " << i << " does not exist." 
                << std::endl;
      std::cerr << atlasVolumeFileNames[i] << std::endl;      
      return EXIT_FAILURE;
      }
    }

  progressReporter.ReportProgress("Loading Data...", 
                                   currentStep++ / totalSteps);

  //
  // create a mrml scene that will hold the parameters and data
  vtkMRMLScene* mrmlScene = vtkMRMLScene::New();
  vtkMRMLScene::SetActiveScene(mrmlScene);
  mrmlScene->SetURL(mrmlSceneFileName.c_str());

  vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New ( );
  colorLogic->SetMRMLScene(mrmlScene);
  colorLogic->AddDefaultColorNodes();
  colorLogic->SetMRMLScene(NULL);
  colorLogic->Delete();

  //
  // create an instance of vtkEMSegmentLogic and connect it with the
  // MRML scene
  vtkEMSegmentLogic* emLogic             = vtkEMSegmentLogic::New();
  emLogic->SetModuleName("EMSegment");
  emLogic->SetAndObserveMRMLScene(mrmlScene);
  emLogic->RegisterMRMLNodesWithScene();
  vtkIntArray *emsEvents                 = vtkIntArray::New();
  emsEvents->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  emsEvents->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  emLogic->SetAndObserveMRMLSceneEvents(mrmlScene, emsEvents);
  emsEvents->Delete();

  //
  // For the EMSegment logic, getting and setting of parameters in the
  // MRML scene is delegated to the EMSegment MRML manager.  Get a
  // shortcut to the manager.
  vtkEMSegmentMRMLManager* emMRMLManager = emLogic->GetMRMLManager();

  progressReporter.ReportProgress("Loading Data...", 
                                   currentStep / totalSteps,
                                   0.2f);

  //
  // global try block makes sure data is cleaned up if anything goes
  // wrong
  try 
    {
    //
    // read the mrml scene
    try 
      {
      if (verbose) std::cerr << "Reading MRML scene...";
      mrmlScene->Connect();
      if (verbose) std::cerr << "DONE" << std::endl;
      }
    catch (...)
      {
      throw std::runtime_error("ERROR: failed to import mrml scene.");
      }

    int numParameterSets = emMRMLManager->GetNumberOfParameterSets();
    if (verbose) std::cerr << "Imported: " << mrmlScene->GetNumberOfNodes()
                           << (mrmlScene->GetNumberOfNodes() == 1 
                               ? " node" : " nodes")
                           << ", including " << numParameterSets 
                           << " EM parameter "
                           << (numParameterSets == 1 ? "node." : "nodes.")
                           << std::endl;
  
    progressReporter.ReportProgress("Loading Data...", 
                                     currentStep / totalSteps,
                                     0.4f);

    //
    // make sure there is at least one parameter set
    if (numParameterSets < 1)
      {
      throw std::
        runtime_error("ERROR: no EMSegment parameter nodes in scene.");
      }

    //
    // find the parameter set in the MRML scene
    int parameterNodeIndex = 0;
    if (useDefaultParametersNode)
      {
      if (verbose) std::cerr << "Using default parameter set named: " 
                             << emMRMLManager->GetNthParameterSetName(0) 
                             << std::endl;    
      }
    else
      {
      // search for the named parameter set
      bool foundParameters = false;
      if (verbose) std::cerr << "Searching for an EM parameter node named: " 
                             << parametersMRMLNodeName << std::endl;
      
      for (int i = 0; i < numParameterSets; ++i)
        {
        std::string currentNodeName(emMRMLManager->GetNthParameterSetName(i)); 
        if (verbose) std::cerr << "Node " << i 
                               << " name: " << currentNodeName << std::endl;
        if (parametersMRMLNodeName == currentNodeName)
          {
          parameterNodeIndex = i;
          foundParameters = true;
          break;
          }
        else
          {
          if (verbose) std::cerr << "Found non-matching EM parameters node: " 
                                 << currentNodeName << std::endl;
          }
        }
      
      // make sure the parameters were found
      if (!foundParameters)
        {
        std::stringstream ss;
        ss << "ERROR: no EMSegment parameters found in scene with name "
           << parametersMRMLNodeName;
        throw std::
          runtime_error(ss.str());
        }
      }

    //
    // populate the mrmlManager with the parameters
    try
      {
      emMRMLManager->SetLoadedParameterSetIndex(parameterNodeIndex);
      }
    catch (...)
      {
      throw std::runtime_error("ERROR: failed to set EMSegment parameters.");
      }

    //
    // don't use manual sampling because the target images might change
    // this is a hack; do better !!!
    emMRMLManager->ChangeTreeNodeDistributionsFromManualSamplingToManual();

    //
    // make sure the basic parameters are available
    if (!emMRMLManager->GetSegmenterNode())
      {
      throw std::runtime_error("ERROR: MRML: Missing segmenter node.");
      }
    if (!emMRMLManager->GetTemplateNode())
      {
      throw std::runtime_error("ERROR: MRML: Missing template node.");
      }
    if (!emMRMLManager->GetGlobalParametersNode())
      {
      throw std::runtime_error("ERROR: MRML: Missing global parameters node.");
      }

    progressReporter.ReportProgress("Loading Data...", 
                                     currentStep / totalSteps,
                                     0.6f);

    //
    // set the target images
    if (useDefaultTarget)
      {
      if (!emMRMLManager->GetTargetInputNode())
        {
        throw std::runtime_error("ERROR: no default target node available.");
        }
      if (verbose) 
        std::cerr << "Using default target node named: " 
                  << emMRMLManager->GetTargetInputNode()->GetName()
                  << std::endl;
      }
    else
      {
      try 
        {
        if (verbose) 
          std::cerr << "Adding a target node..."; 

        // create target node
        vtkMRMLEMSTargetNode* targetNode = vtkMRMLEMSTargetNode::New();
        mrmlScene->AddNodeNoNotify(targetNode);        

        // remove default target node
        mrmlScene->RemoveNode(emMRMLManager->GetTargetInputNode());

        // connect target node to segmenter
        emMRMLManager->GetSegmenterNode()->GetWorkingDataNode()->
          SetInputTargetNodeID(targetNode->GetID());

        if (verbose) 
          std::cerr << targetNode->GetID() << " DONE" << std::endl;

        targetNode->Delete();

        if (verbose)
          std::cerr << "Segmenter's target node is now: " 
                    << emMRMLManager->GetTargetInputNode()->GetID()
                    << std::endl;
        }
      catch (...)
        {
        throw std::runtime_error("ERROR: failed to add target node.");
        }

      if (verbose)
        std::cerr << "Adding " << targetVolumeFileNames.size() 
                  << " target images..." << std::endl;
      for (unsigned int imageIndex = 0; 
           imageIndex < targetVolumeFileNames.size(); ++imageIndex)
        {
        if (verbose) std::cerr << "Loading target image " << imageIndex
                               << "..." << std::endl;
        try
          {
          // load image into scene
          vtkMRMLVolumeNode* volumeNode = 
            AddScalarArchetypeVolume(mrmlScene, 
                                     targetVolumeFileNames[imageIndex].c_str(),
                                     false,
                                     false,
                                     NULL);
          
          if (!volumeNode)
            {
            throw std::runtime_error("failed to load image.");
            }
       
          // set volume name and ID in map
          emMRMLManager->GetTargetInputNode()->AddVolume(volumeNode->GetID(), 
                                                         volumeNode->GetID());
          }
        catch(...)
          {
          vtkstd::stringstream ss;
          ss << "ERROR: failed to load target image "
             << targetVolumeFileNames[imageIndex];
          throw std::runtime_error(ss.str());
          }
        }
      }

    //
    // make sure the number of input channels matches the expected
    // value in the parameters
    if (emMRMLManager->GetGlobalParametersNode()->
        GetNumberOfTargetInputChannels() !=
        emMRMLManager->GetTargetInputNode()->GetNumberOfVolumes())
      {
      vtkstd::stringstream ss;
      ss << "ERROR: Number of input channels (" << 
        emMRMLManager->GetTargetInputNode()->GetNumberOfVolumes()
         << ") does not match expected value from parameters (" << 
        emMRMLManager->GetGlobalParametersNode()->
        GetNumberOfTargetInputChannels() 
         << ")";
      throw std::runtime_error(ss.str());
      }
    else
      {
      if (verbose)
        std::cerr << "Number of input channels (" <<
          emMRMLManager->GetTargetInputNode()->GetNumberOfVolumes()
                  << ") matches expected value from parameters (" <<
          emMRMLManager->GetGlobalParametersNode()->
          GetNumberOfTargetInputChannels() 
                  << ")" << std::endl;
      }

    progressReporter.ReportProgress("Loading Data...", 
                                     currentStep / totalSteps,
                                     0.8f);
    //
    // set the atlas images
    if (useDefaultAtlas)
      {
      if (!emMRMLManager->GetAtlasInputNode())
        {
        throw std::runtime_error("ERROR: no default atlas node available.");
        }
      if (verbose) 
        std::cerr << "Using default atlas node named: " 
                  << emMRMLManager->GetAtlasInputNode()->GetName()
                  << std::endl;
      }
    else
      {
      if (!emMRMLManager->GetAtlasInputNode())
        {
        throw std::runtime_error("ERROR: parameters must already "
                                 "contain an atlas node if you wish "
                                 "to speficy atlas volumes.");
        }
      vtkMRMLEMSAtlasNode* oldAtlasNode = emMRMLManager->GetAtlasInputNode();
      
      try 
        {
        if (verbose) 
          std::cerr << "Adding an atlas node..."; 

        // create atlas node
        vtkMRMLEMSAtlasNode* atlasNode = vtkMRMLEMSAtlasNode::New();
        atlasNode->SetNumberOfTrainingSamples
          (oldAtlasNode->GetNumberOfTrainingSamples());
        mrmlScene->AddNode(atlasNode);        

        // connect atlas node to segmenter
        emMRMLManager->GetSegmenterNode()->GetWorkingDataNode()->
          SetInputAtlasNodeID(atlasNode->GetID());

        if (verbose) 
          std::cerr << atlasNode->GetID() << " DONE" << std::endl;

        atlasNode->Delete();

        if (verbose)
          std::cerr << "Segmenter's atlas node is now: " 
                    << emMRMLManager->GetAtlasInputNode()->GetID()
                    << std::endl;
        }
      catch (...)
        {
        throw std::runtime_error("ERROR: failed to add atlas node.");
        }

      if (verbose)
        std::cerr << "Adding " << atlasVolumeFileNames.size() 
                  << " atlas images..." << std::endl;
      for (unsigned int imageIndex = 0; 
           imageIndex < atlasVolumeFileNames.size(); ++imageIndex)
        {
        if (verbose) std::cerr << "Loading atlas image " << imageIndex
                               << "..." << std::endl;
        try
          {
          // load image into scene
          vtkMRMLVolumeNode* volumeNode = 
            AddScalarArchetypeVolume(mrmlScene, 
                                     atlasVolumeFileNames[imageIndex].c_str(),
                                     false,
                                     false,
                                     NULL);
          
          if (!volumeNode)
            {
            throw std::runtime_error("failed to load image.");
            }
       
          // set volume name and ID in map
          emMRMLManager->GetAtlasInputNode()->
            AddVolume(oldAtlasNode->GetNthKey(imageIndex), 
                      volumeNode->GetID());
          }
        catch(...)
          {
          vtkstd::stringstream ss;
          ss << "ERROR: failed to load atlas image "
             << atlasVolumeFileNames[imageIndex];
          throw std::runtime_error(ss.str());
          }
        }

      //
      // make sure the number of atlas volumes matches the expected
      // value in the parameters
      if (oldAtlasNode->GetNumberOfVolumes() !=
          emMRMLManager->GetAtlasInputNode()->GetNumberOfVolumes())
        {
        vtkstd::stringstream ss;
        ss << "ERROR: Number of atlas volumes (" << 
          emMRMLManager->GetAtlasInputNode()->GetNumberOfVolumes()
           << ") does not match expected value from parameters (" << 
          oldAtlasNode->GetNumberOfVolumes() << ")";
        throw std::runtime_error(ss.str());
        }
      else
        {
        if (verbose)
          std::cerr << "Number of atlas volumes (" <<
            emMRMLManager->GetAtlasInputNode()->GetNumberOfVolumes()
                    << ") matches expected value from parameters (" <<
            oldAtlasNode->GetNumberOfVolumes() << ")" << std::endl;
        }

      // remove default atlas node
      mrmlScene->RemoveNode(oldAtlasNode);
      }

    //
    // set the result labelmap image
    if (useDefaultOutput)
      {
      if (!emMRMLManager->GetOutputVolumeNode())
        {
        throw std::
          runtime_error("ERROR: no default output volume node available.");
        }
      if (verbose) 
        std::cerr << "Using default output volume node named: " 
                  << 
          emMRMLManager->GetOutputVolumeNode()->GetName()
                  << std::endl;
      }
    else
      {
      try 
        {
        // create volume node
        if (verbose) std::cerr << "Creating output volume node...";

        vtkstd::string absolutePath = resultVolumeFileName;

        // the gui uses <image>, the command line uses actual files
        //
        // Set up the filename so that a relative filename will be
        // relative to the current directory, not relative to the mrml
        // scene's path.
        if (!vtksys::SystemTools::FileIsFullPath(resultVolumeFileName.c_str()))
          {
          absolutePath = vtksys::SystemTools::
            CollapseFullPath(resultVolumeFileName.c_str());
          }

        vtkMRMLVolumeNode* outputNode = 
          AddNewScalarArchetypeVolume(mrmlScene,
                                      absolutePath.c_str(),
                                      true,
                                      true,
                                      NULL);

        if (!outputNode)
          {
          throw std::runtime_error("failed to create output image");
          }

        // connect output volume node to segmenter
        emMRMLManager->GetSegmenterNode()->
          SetOutputVolumeNodeID(outputNode->GetID());

        if (verbose) 
          std::cerr << "DONE" << std::endl;
        }
      catch (...)
        {
        throw std::runtime_error("ERROR: failed to add result volume node.");
        }
      }

    progressReporter.ReportProgress("Updating Parameters...", 
                                     currentStep++ / totalSteps,
                                     0.0f);
    //
    // update logic parameters from command line
    emMRMLManager->SetEnableMultithreading(!disableMultithreading);
    if (verbose) 
      std::cerr << "Multithreading is " 
                << (disableMultithreading ? "disabled." : "enabled.")
                << std::endl;

    emMRMLManager->SetUpdateIntermediateData(!dontUpdateIntermediateData);
    if (verbose) 
      std::cerr << "Update intermediate data: " 
                << (dontUpdateIntermediateData ? "disabled." : "enabled.")
                << std::endl;

    int segmentationBoundaryMin[3];
    int segmentationBoundaryMax[3];
    emMRMLManager->GetSegmentationBoundaryMin(segmentationBoundaryMin);
    emMRMLManager->GetSegmentationBoundaryMax(segmentationBoundaryMax);
    if (verbose) std::cerr 
      << "Default ROI is [" 
      << segmentationBoundaryMin[0] << ", "
      << segmentationBoundaryMin[1] << ", "
      << segmentationBoundaryMin[2] << "] -->> ["
      << segmentationBoundaryMax[0] << ", "
      << segmentationBoundaryMax[1] << ", "
      << segmentationBoundaryMax[2] << "]" << std::endl;
    
    //
    // set intermediate results directory
    if (writeIntermediateResults)
      {
      emMRMLManager->SetSaveIntermediateResults(true);
      vtkstd::string absolutePath = vtksys::SystemTools::
        CollapseFullPath(intermediateResultsDirectory.c_str());
      emMRMLManager->
        SetSaveWorkingDirectory(absolutePath.c_str());
      std::cerr << "Intermediate results will be written to: "
                << absolutePath << std::endl;
      }
    else
      {
      emMRMLManager->SetSaveIntermediateResults(false);
      }

    //
    // check parameters' node structure
    if (!emMRMLManager->CheckMRMLNodeStructure())
      {
      throw std::
        runtime_error("ERROR: EMSegment invalid parameter node structure");
      }


    progressReporter.ReportProgress("Running Segmentation...", 
                                     currentStep++ / totalSteps);
    
    // 
    // Setting up everything for new semgnetation mode with sourcing tcl file 
    //
    vtkMRMLEMSTargetNode *inputNodes = emMRMLManager->GetTargetInputNode();
    int newVersionFlag =  inputNodes->GetNumberOfInputChannelNames() ;
    if (  newVersionFlag  ) 
       {
       // =======================================================================
       //
       //  NEW VERSION
       // 
       // =======================================================================

       cout << "===== New Version =======" << endl;
       Tcl_Interp *interp = vtkKWApplication::InitializeTcl(argc, argv, &cout);
       if (!interp)
       {
        cout << "Error: InitializeTcl failed" << endl;
        return EXIT_FAILURE; 
       }

       // This is necessary to load in EMSEgmenter package in TCL interp.
       Emsegment_Init(interp);
       Slicerbasegui_Init(interp);
       Slicerbaselogic_Init(interp);
       Mrml_Init(interp);
       Mrmlcli_Init(interp); 
       Vtkteem_Init(interp);
       Vtkitk_Init(interp);
       // Brainsfit_Init(interp);
       Commandlinemodule_Init(interp);

       // SLICER_HOME
       cout << "Setting SLICER home: " << endl;
       vtksys_stl::string slicerHome = tgGetSLICER_HOME(argv);
       if(!slicerHome.size())
       {
         cout << "Error: Cannot find executable" << endl;
         return EXIT_FAILURE; 
       }
       cout << "Slicer home is " << slicerHome << endl;


       tgVtkDefineMacro(app,vtkSlicerApplication);
       app->Script ("namespace eval slicer3 set Application %s", appTcl.c_str());

       tgVtkDefineMacro(appLogic,vtkSlicerApplicationLogic);
       app->Script ("namespace eval slicer3 set ApplicationLogic %s", appLogicTcl.c_str());
       appLogic->SetAndObserveMRMLScene(mrmlScene);

       std::string emLogicTcl = vtksys::SystemTools::DuplicateString(vtkKWTkUtilities::GetTclNameFromPointer(interp,emLogic));
       std::string emMRMLManagerTcl = vtksys::SystemTools::DuplicateString(vtkKWTkUtilities::GetTclNameFromPointer(interp,emMRMLManager));
       
       // -----------------------------------
       // Check Data 

       vtkMRMLEMSTargetNode* inputNode =  emMRMLManager->GetTargetInputNode(); 
       if (inputNode == NULL)
      {
        cout << "Input target node is null, aborting!" << endl;
          return EXIT_FAILURE; 
      }

       for (int i=0;  i < inputNode->GetNumberOfVolumes(); i++)
     {
       vtkMRMLVolumeNode* cNode = inputNode->GetNthVolumeNode(i);
       if (!cNode)
         {
           cout << "Input Channel Error: Please assign an volume to each input channel" << endl;
           return EXIT_FAILURE; 
         }
       if (!cNode->GetImageData()) 
         {
           cout << "Input Channel Error: Volume of " << i + 1 << "th Input channel is empty !" << endl;
                 return EXIT_FAILURE; 
         }
       if (cNode->GetImageData()->GetScalarRange()[0] < 0 )
         {
           cout << "Input Channel Error: Volume of " << i + 1 << "th is negative - we can only process non-negative volumes !" << endl;
           return EXIT_FAILURE; 
         }

     }
       // -----------------------------------
       // Start processing

       try
       {
         if (verbose) std::cerr << "Starting preprocessing ..." << std::endl;

         emMRMLManager->GetWorkingDataNode()->SetAlignedTargetNodeIsValid(0);
         emMRMLManager->GetWorkingDataNode()->SetAlignedAtlasNodeIsValid(0);
    
         if (emLogic->SourcePreprocessingTclFiles(app))
           {
             throw std::runtime_error("ERROR: could not source tcl files. "); 
           } 
    
         std::string CMD = "::EMSegmenterPreProcessingTcl::InitVariables " + emLogicTcl + " " + emMRMLManagerTcl + " NULL";


         if (atoi(app->Script(CMD.c_str())))
           {
             throw std::runtime_error("ERROR: could not init files. "); 
           }

         if (atoi(app->Script("::EMSegmenterPreProcessingTcl::Run")))
           {
             throw std::runtime_error("ERROR: Pre-processing did not execute correctly");
           }
     
      
         emMRMLManager->GetWorkingDataNode()->SetAlignedTargetNodeIsValid(1);
         emMRMLManager->GetWorkingDataNode()->SetAlignedAtlasNodeIsValid(1);

          if (verbose) std::cerr << "EMSEG: Preprocessing complete." << std::endl;

          if (verbose) std::cerr << "EMSEG: Start Segmentation." << std::endl;

          emLogic->StartSegmentationWithoutPreprocessing();

          if (verbose) std::cerr << "Segmentation complete." << std::endl;
          std::cerr << "============ End of New Pipeline =========================" << std::endl;
          
       }
     catch (...)
       {
         throw std::runtime_error("ERROR: failed to run segmentation.");
       }

     appLogic->Delete();
     appLogic = NULL;  
     app->Delete();
     app = NULL;

       
       }
    else 
      {
    //
    // OLD Version run the segmentation
    try
      {
        if (verbose) std::cerr << "Starting segmentation..." << std::endl;
        emLogic->StartSegmentation();
        if (verbose) std::cerr << "Segmentation complete." << std::endl;
      }
    catch (...)
      {
        throw std::runtime_error("ERROR: failed to run segmentation.");
      }
      }
    }
  catch (std::runtime_error& e)
    {
      std::cerr << e.what() << std::endl;
      std::cerr << "Errors detetected.  Segmentation failed." << std::endl;
      segmentationSucceeded = false;
    }
  catch (...)
    {
      std::cerr << "Unknown error detected.  Segmentation failed." << std::endl;
      segmentationSucceeded = false;
    }
   
  progressReporter.ReportProgress("Updating Results...", 
                                   currentStep++ / totalSteps);

  if (segmentationSucceeded && !dontWriteResults)
    {
    //
    // save the results
    if (verbose) std::cerr << "Saving segmentation results..." << std::endl;
    try
      {
      vtkstd::cerr << "Writing segmentation result: " << 
        emMRMLManager->GetOutputVolumeNode()->GetStorageNode()->GetFileName()
                   << vtkstd::endl;
      emMRMLManager->GetOutputVolumeNode()->GetStorageNode()->
        SetUseCompression(!disableCompression);
      emMRMLManager->GetOutputVolumeNode()->GetStorageNode()->
        WriteData(emMRMLManager->GetOutputVolumeNode());
      }
    catch (std::runtime_error& e)
      {
      std::cerr << e.what() << std::endl;
      std::cerr << "Errors detetected.  Writing failed." << std::endl;
      segmentationSucceeded = false;
      }
    catch (...)
      {
      std::cerr << "Unknown error detected.  Writing failed." << std::endl;
      segmentationSucceeded = false;
      }
    if (verbose) std::cerr << "DONE" << std::endl;
    }
  else
    {
    if (verbose)
      std::cerr << "Skipping over saving segmentation results." << std::endl;
    }

  //
  // compare results to standard image
  if (segmentationSucceeded && !resultStandardVolumeFileName.empty())
    {
    if (verbose) 
      cerr << "Comparing results with standard..." << std::endl;

    try
      {
      //
      // get a pointer to the results
      std::string resultMRMLID = emMRMLManager->GetOutputVolumeMRMLID();
      std::cerr << "Extracting results from mrml node: " 
                << resultMRMLID << std::endl;
      vtkImageData* resultImage = NULL;
      vtkMRMLVolumeNode* node   = vtkMRMLVolumeNode::
        SafeDownCast(mrmlScene->GetNodeByID(resultMRMLID.c_str()));
      resultImage = node->GetImageData();
      resultImage->SetSpacing(node->GetSpacing());
      resultImage->SetOrigin(node->GetOrigin());

      //
      // compare result with standard image
      bool imagesDiffer = ImageDiff(resultImage, resultStandardVolumeFileName);
      if (imagesDiffer)
        {
        if (verbose) 
          if (verbose) cerr << "Result DOES NOT match standard!" << std::endl;
        segmentationSucceeded = false;
        }
      else
        {
        if (verbose) cerr << "Result matches standard!" << std::endl;
        segmentationSucceeded = true;
        }
      }
    catch (std::runtime_error& e)
      {
      std::cerr << e.what() << std::endl;
      std::cerr << "Errors detetected.  Comparison failed." << std::endl;
      segmentationSucceeded = false;
      }
    catch (...)
      {
      std::cerr << "Unknown error detected.  Comparison failed." << std::endl;
      segmentationSucceeded = false;
      }
    }

  //
  // write the final mrml scene file
  if (segmentationSucceeded && !resultMRMLSceneFileName.empty())
    {
    if (verbose) std::cerr << "Writing mrml scene...";
    mrmlScene->Commit(resultMRMLSceneFileName.c_str());
    if (verbose) std::cerr << "DONE" << std::endl;
    }

  progressReporter.ReportProgress("Cleaning Up...", 
                                  currentStep++ / totalSteps);

  //
  // clean up
  if (verbose) std::cerr << "Cleaning up...";

  mrmlScene->Clear(true);
  mrmlScene->Delete();
  emLogic->SetAndObserveMRMLScene(NULL);
  emLogic->Delete();
  if (verbose) std::cerr << "DONE" << std::endl;

  return segmentationSucceeded ? EXIT_SUCCESS : EXIT_FAILURE;  
}
