//____________________________________________________________________________..
//
// This is the interface to the framework. You only need to define the parameters
// you use for your detector in the SetDefaultParameters() method here
// The place to do this is marked by //implement your own here//
// The parameters have no units, they need to be converted in the
// EICG4LumiDetector::ConstructMe() method
// but the convention is as mentioned cm and deg
//____________________________________________________________________________..
//
#include "EICG4LumiSubsystem.h"

#include "EICG4LumiDetector.h"
#include "EICG4LumiSteppingAction.h"

#include <phparameter/PHParameters.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4SteppingAction.h> 

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h> 
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

using namespace std;

//_______________________________________________________________________
EICG4LumiSubsystem::EICG4LumiSubsystem(const std::string &name, const int lyr)
  : PHG4DetectorSubsystem(name, lyr)
  , m_Detector(nullptr)
  , m_SteppingAction(nullptr)
{
  // call base class method which will set up parameter infrastructure
  // and call our SetDefaultParameters() method
  InitializeParameters();
}

//_______________________________________________________________________
int EICG4LumiSubsystem::InitRunSubsystem(PHCompositeNode *topNode)
{
  // create detector
  m_Detector = new EICG4LumiDetector(this, topNode, GetParams(), Name(), GetLayer());
  m_Detector->SuperDetector(SuperDetector());
  m_Detector->OverlapCheck(CheckOverlap());

  std::set<std::string> nodes;

  if ( GetParams()->get_int_param("active") )
  {
    PHNodeIterator iter(topNode);
    PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));

    std::string nodename_prefix;
    std::string nodename_CAL;
    std::string nodename_absorber;
    std::string nodename_support;
    std::string nodename_tracking;
    std::string nodename_virt;

    if ( SuperDetector() != "NONE" )
    {
      // create super detector subnodes
      PHNodeIterator iter_dst(dstNode);
      PHCompositeNode *superSubNode = dynamic_cast<PHCompositeNode *>(iter_dst.findFirst("PHCompositeNode", SuperDetector()));

      if ( ! superSubNode )
      {
        superSubNode = new PHCompositeNode(SuperDetector());
        dstNode->addNode(superSubNode);
      }
      dstNode = superSubNode;

      nodename_prefix = "G4HIT_" + SuperDetector();
    }
    else
    {
      nodename_prefix = "G4HIT_" + Name();
    }

    nodename_CAL = nodename_prefix + "_CAL";
    nodename_absorber = nodename_prefix + "_absorber";
    nodename_support = nodename_prefix + "_support";
    nodename_tracking = nodename_prefix + "_tracking";
    nodename_virt = nodename_prefix + "_virt";

    nodes.insert( nodename_CAL );
    nodes.insert( nodename_tracking );

    if ( GetParams()->get_int_param("AbsorberActive" ) ) {
      nodes.insert( nodename_absorber );
    }
    if ( GetParams()->get_int_param("SupportActive" ) ) {
      nodes.insert( nodename_support );
    }
    if ( GetParams()->get_int_param("VirtualActive" ) ) {
      nodes.insert( nodename_virt );
    }
    
    // Create nodes to store Hits
    for (auto thisnode : nodes)
    {
      PHG4HitContainer* g4_hits = findNode::getClass<PHG4HitContainer>(topNode, thisnode);

      if ( ! g4_hits )
      {
        g4_hits = new PHG4HitContainer(thisnode);
        dstNode->addNode(new PHIODataNode<PHObject>(g4_hits, thisnode, "PHObject"));
      }
    }

    // TODO: Is the AddLayer really needed ????
    //CAL_hits->AddLayer( GetLayer() );
    //Tracking_hits->AddLayer( GetLayer() );
    //Virt_hits->AddLayer( GetLayer() );
    
    // Create stepping action
    auto *tmp = new EICG4LumiSteppingAction( this, m_Detector, GetParams() );
    tmp->HitNodeNameCAL( nodename_CAL );
    tmp->HitNodeNameAbsorber( nodename_absorber );
    tmp->HitNodeNameSupport( nodename_support );
    tmp->HitNodeNameTracking( nodename_tracking );
    tmp->HitNodeNameVirt( nodename_virt);
    
    m_SteppingAction = tmp;

  }
  else if (GetParams()->get_int_param("blackhole"))
  {
    m_SteppingAction = new EICG4LumiSteppingAction(this, m_Detector, GetParams());
  }

  if (m_SteppingAction)
  {
    (dynamic_cast<EICG4LumiSteppingAction *>(m_SteppingAction))->SaveAllHits(m_SaveAllHitsFlag);
  }

  return 0;
}
//_______________________________________________________________________
int EICG4LumiSubsystem::process_event(PHCompositeNode *topNode)
{
  // pass top node to stepping action so that it gets
  // relevant nodes needed internally
  if (m_SteppingAction)
  {
    m_SteppingAction->SetInterfacePointers(topNode);
  }
  return 0;
}
//_______________________________________________________________________
void EICG4LumiSubsystem::Print(const string &what) const
{
  if (m_Detector)
  {
    m_Detector->Print(what);
  }
  return;
}

//_______________________________________________________________________
PHG4Detector *EICG4LumiSubsystem::GetDetector(void) const
{
  return m_Detector;
}

//_______________________________________________________________________
void EICG4LumiSubsystem::SetDefaultParameters()
{

  // defaults are in cm and rad
  set_default_double_param("FBenclosure_center", 0.);
  set_default_string_param("parameter_file", "");
  set_default_int_param("layerNumber", 1);
  set_default_double_param("detid", 0.);     //detector id
  set_default_int_param("lightyield", 1);
  set_default_int_param("use_g4steps", 0);
  set_default_double_param("tmin", NAN);
  set_default_double_param("tmax", NAN);

  set_default_int_param("Version", 1);	
  
  set_default_int_param("AbsorberActive", 0);	
  set_default_int_param("SupportActive", 0);	
  set_default_int_param("VirtualActive", 0);	

  set_default_double_param("LumiWin_X", 0.);
  set_default_double_param("LumiWin_Y", 0.);
  set_default_double_param("LumiWin_Z", -1850);
  set_default_double_param("LumiWin_Tilt", 0.25);
  set_default_double_param("LumiWin_Thickness", 0.26);
  set_default_double_param("LumiWin_Height", 7.4);
  set_default_double_param("LumiWin_Length", 29);
  set_default_string_param("LumiWin_Material", "G4_Al");

  set_default_double_param("LumiMag_Z", -2820);
  set_default_double_param("LumiMag_innerR", 10);
  set_default_double_param("LumiMag_outerR", 16);
  set_default_double_param("LumiMag_DZ", 60);
  set_default_double_param("LumiMag_B", 0.37);
  set_default_string_param("LumiMag_VesselMaterial", "G4_Fe");

  set_default_double_param("LumiSpec_Z", -3640);
  set_default_double_param("LumiSpec_XY", 20);
  set_default_double_param("LumiSpec_DZ", 35);
  set_default_double_param("LumiSpec_YS", 6);

  set_default_double_param("LumiPhotonCAL_Z", -3700);
  set_default_double_param("LumiPhotonCAL_XY", 16);
  set_default_double_param("LumiPhotonCAL_DZ", 35);

  set_default_double_param("LumiTracker2_XY", 0.06);
  set_default_double_param("LumiTracker2_DZ", 20);

  set_default_double_param("LumiTracker1Gap", 2.0);

  set_default_double_param("LumiSpecTower_XY", 2.5);
  set_default_double_param("LumiSpecTower_DZ", 17);
  set_default_int_param("TotalLumiSpecTower", 8);
}

//_______________________________________________________________________
void EICG4LumiSubsystem::SetParametersFromFile( std::string filename )
{
  set_string_param("parameter_file", filename );

  std::ifstream infile;
  std::string line;

  std::string paramFile = filename;   
  infile.open( paramFile );

  if(!infile.is_open()) {
    std::cout << PHWHERE << "ERROR in EICG4LumiSubsystem: Failed to open parameter file " << paramFile << std::endl;
    gSystem->Exit(1);
  }

  while( std::getline(infile, line) ) {

    std::string name;
    std::string value;

    std::istringstream iss( line );

    // skip comment lines
    if( line.find("#") != std::string::npos ) { continue; }
    if( line.empty() == true ) { continue; }

    // grab the line
    if( !(iss >> name >> value) ) {
      std::cout << PHWHERE << " Lumi parameters: could not decode " << line << std::endl;
      gSystem->Exit(1);
    }
    
    auto *params = GetParams(); // Get handle for PHParameter class

    if( params->exist_string_param( name ) ) {
      set_string_param(name, value);
    }
    else if( params->exist_double_param( name ) ) {
      set_double_param(name, std::stod(value) );
    }
    else if( params->exist_int_param( name ) ) {
      set_int_param(name, std::stoi(value) );
    }
    else { 
      std::cout << PHWHERE << " Lumi parameters: parameter doesn't exist " << line << std::endl;
      gSystem->Exit(1);
    }
  
  }

  infile.close();

}
