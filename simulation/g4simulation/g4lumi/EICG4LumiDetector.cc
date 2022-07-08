//____________________________________________________________________________..
//
// Far-Backward detector class
//      includes: Luminosity exit window, direct photon CAL, e+e- conversion CAL, 
//                electron taggers
//
// Written by Dhevan Gangadharan, Aranya Giri
// Based on designs from Jaroslav Adam and Zhiwan Xu
//
// template generated by CreateG4Subsystem.pl
//____________________________________________________________________________..

#include "EICG4LumiDetector.h"


#include <phparameter/PHParameters.h>

#include <g4main/PHG4Detector.h>
#include <g4main/PHG4DisplayAction.h>  // for PHG4DisplayAction
#include <g4main/PHG4Subsystem.h>

#include <Geant4/G4Color.hh>
#include <Geant4/G4Box.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Material.hh>
#include <Geant4/G4PVPlacement.hh>
#include <Geant4/G4SubtractionSolid.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4Tubs.hh>
#include <Geant4/G4Box.hh>
#include <Geant4/G4Para.hh>
#include <Geant4/G4RotationMatrix.hh>
#include <Geant4/G4SystemOfUnits.hh>
#include <Geant4/G4ThreeVector.hh>      // for G4ThreeVector
#include <Geant4/G4Transform3D.hh>      // for G4Transform3D
#include <Geant4/G4Types.hh>            // for G4double, G4int
#include <Geant4/G4VPhysicalVolume.hh>  // for G4VPhysicalVolume
#include <TSystem.h>
#include <Geant4/G4UnionSolid.hh>
#include <Geant4/G4VisAttributes.hh>
#include <Geant4/G4TwoVector.hh>
#include <Geant4/G4ExtrudedSolid.hh>
#include <Geant4/G4Trd.hh>


#include <phool/recoConsts.h> //For rc WorldMaterial

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <utility>
//#include <stdio>
#include <TMath.h>

//#include <Geant4/G4Trap.h>

class G4VSolid;
class PHCompositeNode;

using namespace std;

//____________________________________________________________________________..
EICG4LumiDetector::EICG4LumiDetector(PHG4Subsystem *subsys,
                                         PHCompositeNode *Node,
                                         PHParameters *parameters,
                                         const std::string &dnam, const int lyr)
  : PHG4Detector(subsys, Node, dnam)
  , m_Params(parameters)
  , m_Layer(lyr)
  , m_Name("Lumi")
{
}

int EICG4LumiDetector::IsInDetector(G4VPhysicalVolume *volume) const
{

  if( m_ActivePhysicalVolumesSet.find( volume ) != m_ActivePhysicalVolumesSet.end() )
  {
    return 1;
  }

  return 0;

}

//_______________________________________________________________
int EICG4LumiDetector::IsInVirtualDetector(G4VPhysicalVolume *volume) const
{

  if( m_VirtualPhysicalVolumesMap.find( volume ) != m_VirtualPhysicalVolumesMap.end() )
  {
    return 1;
  }
  return 0;

}

//_______________________________________________________________
int EICG4LumiDetector::GetDetId(G4VPhysicalVolume *volume) const
{
  
  if (IsInDetector(volume))
  {
    return 1;
  }

  return -1;
}

//_______________________________________________________________
void EICG4LumiDetector::ConstructMe(G4LogicalVolume *logicWorld)
{
 //begin implement your own here://
 // Do not forget to multiply the parameters with their respe	Gctive CLHEP/G4 unit !
  if (Verbosity() > 0)
  {
    std::cout << "EICG4LumiDetector: Begin Construction" << std::endl;
  }

  //________________________________________PARAMETER INITIALISATION__________________________________________________________

  double enclosureCenter = m_Params->get_double_param( "FBenclosure_center" ) * cm;
  
  // Version # of the Luminosity Monitor Design.
  int Version = m_Params->get_int_param( "Version" );
  
  // Primary Lumi Exit Window
  double LumiWin_X = m_Params->get_double_param( "LumiWin_X" ) * cm;
  double LumiWin_Y = m_Params->get_double_param( "LumiWin_Y" ) * cm;
  double LumiWin_Z = m_Params->get_double_param( "LumiWin_Z" ) * cm - enclosureCenter;
  double LumiWin_Tilt = m_Params->get_double_param( "LumiWin_Tilt" ) * rad;
  double LumiWin_Thickness = m_Params->get_double_param( "LumiWin_Thickness" ) * cm;
  double LumiWin_Height = m_Params->get_double_param( "LumiWin_Height" ) * cm;
  double LumiWin_Length = m_Params->get_double_param( "LumiWin_Length" ) * cm;
  
  // Spectrometer dipole magnet
  double LumiMag_Z = m_Params->get_double_param( "LumiMag_Z" ) * cm - enclosureCenter;
  double LumiMag_inner = m_Params->get_double_param( "LumiMag_innerR" ) * cm;
  double LumiMag_outer = m_Params->get_double_param( "LumiMag_outerR" ) * cm;
  double LumiMag_DZ = m_Params->get_double_param( "LumiMag_DZ" ) * cm;
  double LumiMag_B = m_Params->get_double_param( "LumiMag_B" ) * tesla;

  double LumiSpec_Z = m_Params->get_double_param( "LumiSpec_Z" ) * cm - enclosureCenter;
  //double LumiSpec_XY = m_Params->get_double_param( "LumiSpec_XY" ) * cm;
  //double LumiPhotonCAL_Z = m_Params->get_double_param( "LumiPhotonCAL_Z" ) * cm - enclosureCenter;
  double LumiPhotonCAL_XY = m_Params->get_double_param( "LumiPhotonCAL_XY" ) * cm;

  // Lumi ee Spectrometer single tower (Calorimeter) dimensions
  double LumiSpecTower_XY = m_Params->get_double_param( "LumiSpecTower_XY" ) * cm;
  double LumiSpecTower_DZ = m_Params->get_double_param( "LumiSpecTower_DZ" ) * cm;
  int TotalLumiSpecTower = m_Params->get_int_param( "TotalLumiSpecTower" );

  // Tracker1 Details
  //double LumiTracker1Gap = m_Params->get_double_param( "LumiTracker1Gap" ) * cm; //Collimator dy dimension.

  // Tracker2 Details
  double LumiTracker2_XY = m_Params->get_double_param( "LumiTracker2_XY" ) * cm;
  double LumiTracker2_DZ = m_Params->get_double_param( "LumiTracker2_DZ" ) * cm;

  // LumiWin_Thickness*factor for different exit window versions.
  double factorV1 = 1.0; 
  double factorV2 = 1.0; 
  double factorV3 = 1.0; 
 
  // Initialisation of diff. volume region.
  std::string LumiWin_Material = m_Params->get_string_param( "LumiWin_Material" );
  std::string LumiMag_VesselMaterial = m_Params->get_string_param( "LumiMag_VesselMaterial" );
    
  std::string TrianTrapMaterial = "G4_AIR";
  std::string CuboidMaterial = "G4_AIR";
  std::string MagCoreMaterial = "G4_AIR";
  std::string RecConeMaterial = "G4_AIR";
  std::string ExitWinV2Material = "G4_AIR";
  std::string ExitWinV3Material = "G4_AIR";
  //std::string MidConvMaterial = "G4_AIR";

  //Change the material according to Version.
  switch(Version){

    case 1: //No change in material def.
      break;

    case 2: TrianTrapMaterial       = "G4_Galactic";
            CuboidMaterial          = "G4_Galactic";
            MagCoreMaterial         = "G4_Galactic";
            //MidConvMaterial       = LumiWin_Material;
            ExitWinV2Material       = LumiWin_Material;
            factorV1                = 0.1;
            break;

    case 3: TrianTrapMaterial       = "G4_Galactic";
            CuboidMaterial          = "G4_Galactic";
            MagCoreMaterial         = "G4_Galactic";
            RecConeMaterial         = "G4_Galactic";
            ExitWinV2Material       = "G4_Galactic";
            //MidConvMaterial       = LumiWin_Material;	
            ExitWinV3Material       = LumiWin_Material;
            factorV1                = 0.1;
            break;

    default : std::cout<<"WRONG CHOICE (ONLY 1, 2 & 3)"<<endl;
              break;

  }

  // sizes and positions (in mother volume coordinates)
  // Main Lumi Window
  G4ThreeVector size_lw = G4ThreeVector( LumiWin_Length, LumiWin_Height, factorV1*LumiWin_Thickness ); //thickness is factorised A/Version 1
  G4ThreeVector pos_lw = G4ThreeVector( LumiWin_X, LumiWin_Y, LumiWin_Z );
 
  // Outer Vessel
  G4ThreeVector size_ov = G4ThreeVector( LumiMag_inner, LumiMag_outer, LumiMag_DZ ); //x - inner radius, y - outer radius and z - length of cylinder vessel.
  G4ThreeVector pos_ov = G4ThreeVector( LumiWin_X, LumiWin_Y, LumiMag_Z ); //same axis as the lumi window.
  
  // Spectrometer Magnet Core
  G4ThreeVector size_mc = G4ThreeVector( 0., LumiMag_inner, LumiMag_DZ ); //x - inner radius, y - outer radius and z - length of cylindrical core.
  
  // Exit Windows for V2 and V3
  // thickness is factorised
  G4ThreeVector size_ewV2 = G4ThreeVector( LumiWin_Length, LumiWin_Height, factorV2*LumiWin_Thickness );
  G4ThreeVector size_ewV3 = G4ThreeVector( LumiWin_Length, LumiWin_Height, factorV3*LumiWin_Thickness );
  
  // Spectrometer Tracker 2
  G4ThreeVector size_tr2 = G4ThreeVector( LumiTracker2_XY, LumiTracker2_XY, LumiTracker2_DZ );
  G4ThreeVector pos_tr2 = G4ThreeVector( LumiWin_X, pos_lw.y() + LumiPhotonCAL_XY/2.0 + size_tr2.y()/2.0 + 0.01*cm, 
      LumiSpec_Z + LumiSpecTower_DZ/2.0 + LumiWin_Thickness + LumiTracker2_DZ/2.0 ); //tracker 2 pos 

  // Spectrometer Tracker 1
  double LumiTracker1_XY = pos_tr2.y() - size_ewV2.y()/2.;
  G4ThreeVector size_tr1 = G4ThreeVector( LumiTracker1_XY, LumiTracker1_XY, LumiTracker2_DZ );

  // Spectrometer Towers
  G4ThreeVector size_st = G4ThreeVector( 
      TotalLumiSpecTower*LumiSpecTower_XY, TotalLumiSpecTower*LumiSpecTower_XY, LumiSpecTower_DZ ); 
 
  // 
  double dz_tr2_daughter = -1*pos_tr2.z() - (-1*pos_ov.z() + size_ov.z()/2.0);
  G4ThreeVector pos_ewV2_daughter = G4ThreeVector( 0.0, 0.0, dz_tr2_daughter/2.0 - size_ewV2.z()/2.0);
  
  G4ThreeVector pos_tr2_top = G4ThreeVector( 0.0, LumiPhotonCAL_XY/2.0 + size_tr2.y()/2.0, 
      -1*(dz_tr2_daughter/2.0) + size_tr2.z()/2.0 ); //tracker2 pos in recCone;
  G4ThreeVector pos_tr2_bot = G4ThreeVector( pos_tr2_top.x(), -1*pos_tr2_top.y(), pos_tr2_top.z() ); 

  G4ThreeVector pos_tr1_top = G4ThreeVector( 0.0, pos_tr2.y()/2.0, 0.0 ); //tracker 1 position in RecCone.
  G4ThreeVector pos_tr1_bot = G4ThreeVector( pos_tr1_top.x() , -1*pos_tr1_top.y(), pos_tr1_top.z() );

  //-------------------------------------------------------
  // Add Components of Lumi Detector
  // magnet core material may be Air or Galactic depending on Version
  AddLumiWindow(          size_lw, pos_lw, LumiWin_Tilt, LumiWin_Material, logicWorld);
  AddTriangularTrapezoid( size_lw, pos_lw, LumiWin_Tilt, TrianTrapMaterial, logicWorld);
  AddCuboid(              size_lw, pos_lw, size_ov, pos_ov, LumiWin_Tilt, CuboidMaterial, logicWorld);
  AddLumiMag_OuterVessel( size_ov, pos_ov, LumiMag_VesselMaterial, logicWorld);
  AddLumiMag_MagCore(     size_mc, pos_ov, LumiMag_B, MagCoreMaterial, logicWorld);
  
  // sub-mother volume for trackers and V2/V3 exit windows
  G4LogicalVolume* logicRecCone = AddRecCone( size_lw, pos_lw, size_ov, pos_ov, 
      size_tr2, pos_tr2, LumiWin_Tilt, RecConeMaterial, logicWorld );
  
  AddExitWindowForV2( size_ewV2, pos_ewV2_daughter, LumiWin_Tilt, ExitWinV2Material, logicRecCone );

  //-------------------------------------------------------
  // Spectrometer Trackers
  AddLumiTracker( "TopSpecTracker1", 0, size_tr1, pos_tr1_top, logicRecCone);
  AddLumiTracker( "BotSpecTracker1", 1, size_tr1, pos_tr1_bot, logicRecCone);
  AddLumiTracker( "TopSpecTracker2", 2, size_tr2, pos_tr2_top, logicRecCone);
  AddLumiTracker( "BotSpecTracker2", 3, size_tr2, pos_tr2_bot, logicRecCone);
  
  AddExitWindowForV3( size_ewV3, pos_lw, size_tr2, pos_tr2, ExitWinV3Material, logicWorld );
  
  //-------------------------------------------------------
  // Spectrometer Calorimeter Towers
  AddCAL( "TopSpecTower", size_st, G4ThreeVector(LumiWin_X, pos_tr2.y(), LumiSpec_Z), TotalLumiSpecTower, logicWorld );
  AddCAL( "BotSpecTower", size_st, G4ThreeVector(LumiWin_X, -1*pos_tr2.y(), LumiSpec_Z), TotalLumiSpecTower, logicWorld );
  
 return;
}

//_______________________________________________________________
void EICG4LumiDetector::Print(const std::string &what) const
{
  std::cout << "EICG4Lumi Detector:" << std::endl;
  if (what == "ALL" || what == "VOLUME")
  {
    std::cout << "Version 0.1" << std::endl;
    std::cout << "Parameters:" << std::endl;
    m_Params->Print();
  }
  return;
}

//______________________________________________________________

PHParameters *EICG4LumiDetector::getParams()
{
  return m_Params;
}

//_____________________________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddLumiWindow(G4ThreeVector size, G4ThreeVector pos, double angle, std::string material, G4LogicalVolume *logicWorld){

  std::string name = "LumiWindow";
  G4Box *solid = new G4Box(name + "_solid", (size.x()/2.0) + 0.3*cm, size.y()/2.0, size.z()/2.0 );
  //G4Para *solid = new G4Para(name + "_solid", size.x()/2.0, size.y()/2.0, factor*(size.z()/2.0) , angle , angle , 0.);

  G4LogicalVolume *logical = new G4LogicalVolume( solid, GetDetectorMaterial(material), name + "_logical");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(1, 0, 0, 1) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes(vis);

  G4ThreeVector rot_axis = G4ThreeVector(0,1,0); //Y-axis
  G4RotationMatrix *rot_matrix = new G4RotationMatrix(rot_axis , angle); //is typedef to CLHEP::HepRotation
  //G4ThreeVector pos_new = G4ThreeVector(pos.x() - (size.z()/(2.0*TMath::Sin(angle))), pos.y() , pos.z() );
  G4ThreeVector pos_new = G4ThreeVector(pos.x() - (size.z()/(2.0*TMath::Sin(angle)) ), pos.y(), pos.z() );
  G4VPhysicalVolume *physical = new G4PVPlacement(rot_matrix, pos_new, logical, name+"_physical", logicWorld, 0, false, OverlapCheck());
  //G4VPhysicalVolume *physical = new G4PVPlacement(0, pos, logical, name+"_physical", logicWorld, 0, false, OverlapCheck());

  m_PassivePhysicalVolumesSet.insert( physical );

}

//_____________________________________________________________________________________________________________________________________________
G4ThreeVector EICG4LumiDetector::AddLumiExitWindow(G4ThreeVector size, G4ThreeVector pos, double factor, double angle, std::string material, G4LogicalVolume *logicWorld){

  std::string name = "LumiExitWindow";

  //G4Box *solid = new G4Box(name + "_solid", size.x()/2.0, size.y()/2.0, factor*size.z()/2.0);
  //G4Para *solid = new G4Para(name + "_solid", size.x()/2.0, size.y()/2.0, factor*(size.z()/2.0) - 0.01*cm, angle, angle, 0);
  G4Box *solid = new G4Box(name + "_solid", size.x(), size.y(), size.z());

  G4LogicalVolume *logical = new G4LogicalVolume( solid, GetDetectorMaterial(material), name + "_logical");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(0, 1, 0, 1) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes(vis);

  G4ThreeVector rot_axis = G4ThreeVector(0,1,0); //Y-axis
  G4RotationMatrix *rot_matrix = new G4RotationMatrix(rot_axis , angle); //is typedef to CLHEP::HepRotation
  G4ThreeVector pos_new = G4ThreeVector(pos.x(), pos.y(), pos.z() - size.z() - factor);
  G4VPhysicalVolume *physical = new G4PVPlacement(rot_matrix, pos_new, logical, name+"_physical", logicWorld, 0, false, OverlapCheck());
  m_PassivePhysicalVolumesSet.insert( physical );

  return pos_new;

}

//________________________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddLumiMag_OuterVessel( G4ThreeVector size, G4ThreeVector pos, std::string material, G4LogicalVolume *logicWorld)
{
  std::string name = "LumiMag_OuterVessel";

  //x - inner radius, y- outer radius and z - length of tube. Theta coverage 0 - 360 deg
  G4Tubs *solid = new G4Tubs(name+"_solid", size.x(), size.y(), size.z()/2., 0., 360.*deg);

  G4LogicalVolume *logical = new G4LogicalVolume(solid, GetDetectorMaterial(material), name+"_logical");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(0, 1, 0, 0.5) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes( vis );

  G4VPhysicalVolume *physical = new G4PVPlacement( 0, pos, logical, name+"_physical", logicWorld, 0, false, OverlapCheck() );
  m_PassivePhysicalVolumesSet.insert( physical);

}

//______________________________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddLumiMag_MagCore(G4ThreeVector size, G4ThreeVector pos, double Bx, std::string material, G4LogicalVolume *logicWorld)
{
  std::string name = "LumiMag_MagCore";

  // x - inner radius, y - outer radius, z - length of the cylindrical core. Theta coverage 0- 360 deg
  G4Tubs *solid = new G4Tubs(name, size.x(), size.y() - 0.01*cm, size.z()/2.0, 0., 360.*deg);

  G4LogicalVolume *logical = new G4LogicalVolume( solid, GetDetectorMaterial(material), name);
  logical->SetVisAttributes( G4VisAttributes::GetInvisible() );

  G4UniformMagField *field = new G4UniformMagField( G4ThreeVector(Bx, 0, 0 ) ); //Mag field stregth along x-direction
  G4FieldManager *fman = new G4FieldManager();
  fman->SetDetectorField( field );
  fman->CreateChordFinder( field );
  logical->SetFieldManager(fman, true);

  G4VPhysicalVolume *physical = new G4PVPlacement( 0, pos, logical, name, logicWorld, 0, false, OverlapCheck() );
  m_PassivePhysicalVolumesSet.insert( physical);

  //return logical;
}

//______________________________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddTriangularTrapezoid(G4ThreeVector size, G4ThreeVector pos, double angle, std::string material, G4LogicalVolume *logicWorld)
{
  // Construct the triangle in x-y plane, extrude in the z-axis and finally rotate it by -90 deg via x -axis.

  std::string name = "LumiTrainTrap"; 
  std::vector<G4TwoVector> polygon;

  double x1 = -1*(size.x()/2.0)*TMath::Cos(angle);
  double y1 = -1*(size.x()/2.0)*TMath::Sin(angle);

  double x2 = (size.x()/2.0)*TMath::Cos(angle);
  double y2 = -1*(size.x()/2.0)*TMath::Sin(angle);

  double x3 = (size.x()/2.0)*TMath::Cos(angle);
  double y3 = (size.x()/2.0)*TMath::Sin(angle);

  polygon.push_back({x1, y1});
  polygon.push_back({x2, y2});
  polygon.push_back({x3, y3});

  // Extrude the triange - Solid-Volume
  std::vector<G4ExtrudedSolid::ZSection> zsections = { 
    {-1*size.y()/2.0, {0,0}, 1.0}, {size.y()/2.0, {0,0}, 1.0} };

  G4ExtrudedSolid *solid = new G4ExtrudedSolid(name+"_ExtrudedSolid", polygon, zsections);

  // Logical-Volume{

  G4LogicalVolume *logical = new G4LogicalVolume(solid, GetDetectorMaterial(material), name+"_logical");
  //G4LogicalVolume *logical = new G4LogicalVolume(solid, material, name+"_logical");

  G4VisAttributes *vis = new G4VisAttributes( G4Color(0, 1, 1, 0.3) ); // ( red , green , blue , transperancy ) all in percent.
  vis->SetForceSolid(true);
  logical->SetVisAttributes(vis);

  //Physical Volume (Rotate the volume by -90' about x-axis)
  G4RotationMatrix *rot_matrix = new G4RotationMatrix( G4ThreeVector(1, 0, 0), -1*TMath::Pi()/2.0 );
  //G4ThreeVector pos_new = G4ThreeVector(pos.x(), pos.y(), pos.z() - (size.z()/(2.0*TMath::Cos(angle))));
  G4VPhysicalVolume *physical = new G4PVPlacement( rot_matrix, pos, logical, name+"_physical", logicWorld, 0, false, OverlapCheck());

  // Add it to the list of active volumes so the IsInDetector method picks them up
  m_PassivePhysicalVolumesSet.insert(physical); 

  //return pos_new;
}

//_______________________________________________________________________________________________________
void EICG4LumiDetector::AddCuboid(G4ThreeVector Wsize, G4ThreeVector Wpos, G4ThreeVector Msize, G4ThreeVector Mpos, double angle, std::string material, G4LogicalVolume *logicWorld)
{
  std::string name = "LumiCuboid";
  double dz_cuboid = -1*Mpos.z() - (Msize.z()/2.0) - ( -1*Wpos.z() + ((Wsize.x()/2.0)*TMath::Sin(angle)));
  G4Box *solid = new G4Box(name + "_solid",(Wsize.x()/2.0)*TMath::Cos(angle), Wsize.y()/2.0, dz_cuboid/2.0);

  G4LogicalVolume *logical = new G4LogicalVolume(solid, GetDetectorMaterial(material),name + "_logic");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(0, 1, 1, 0.3) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes( vis );

  double zpos_cuboid = Wpos.z() - (Wsize.x()/2.0)*TMath::Sin(angle) - dz_cuboid/2.0 ;
  G4ThreeVector pos_cuboid = G4ThreeVector( Wpos.x(), Wpos.y(), zpos_cuboid);
  G4VPhysicalVolume *physical = new G4PVPlacement(0, pos_cuboid, logical, name + "_physical", logicWorld, 0, false, OverlapCheck());
  m_PassivePhysicalVolumesSet.insert( physical);

  //return logical; //Used for Midway Convertor only.
}

//_____________________________________________________________________________________________________________

//_____________________________________________________________________________________________________________
G4LogicalVolume* EICG4LumiDetector::AddRecCone(G4ThreeVector Wsize, G4ThreeVector Wpos, G4ThreeVector Msize, G4ThreeVector Mpos, G4ThreeVector Tr2size, G4ThreeVector Tr2pos, double angle, std::string material, G4LogicalVolume *logicWorld)
{

  double dz_rec_cone = ( -1*Tr2pos.z() + Tr2size.z()/2.0 ) - (-1*Mpos.z() + Msize.z()/2.0);
  G4Trd *solid = new G4Trd("solid", Tr2size.x()/2.0 + 1*cm,(Wsize.x()/2.0)*TMath::Cos(angle), Tr2pos.y() + Tr2size.y()/2.0 + 2.0*cm, Wsize.y()/2.0, dz_rec_cone/2.0); //2.0cm to adjust the overlap due to conical shape.

  G4LogicalVolume *logical = new G4LogicalVolume(solid, GetDetectorMaterial("G4_Galactic"), "Lumi_RectangularCone");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(0, 1, 1, 0.3) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes( vis);

  G4ThreeVector pos_rec_cone = G4ThreeVector(Wpos.x(), Wpos.y(), Mpos.z() - Msize.z()/2.0 - dz_rec_cone/2.0);
  G4VPhysicalVolume *physical = new G4PVPlacement(0, pos_rec_cone, logical, "Lumi_RectangularCone", logicWorld, 0, false, OverlapCheck());
  m_PassivePhysicalVolumesSet.insert( physical);

  return logical;
}

//______________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddExitWindowForV2(G4ThreeVector Wsize, G4ThreeVector pos_daug, double angle, std::string material, G4LogicalVolume *logicWorld)
{
  std::string name = "LumiExitWinV2";
  G4Box *solid = new G4Box(name+"solid",(Wsize.x()/2.0)*TMath::Cos(angle), Wsize.y()/2.0, Wsize.z()/2.0);

  G4LogicalVolume *logical = new G4LogicalVolume( solid, GetDetectorMaterial(material), name+"logical");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(1, 0, 0, 1) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes( vis );

  G4VPhysicalVolume *physical = new G4PVPlacement(0, pos_daug, logical, name+"physical", logicWorld, 0, false, OverlapCheck());
  m_PassivePhysicalVolumesSet.insert( physical);
}

//_______________________________________________________________________________________________________
void EICG4LumiDetector::AddLumiTracker( std::string name, int copyNum, G4ThreeVector size, G4ThreeVector pos, G4LogicalVolume *logicWorld )
{

  G4Box *solid = new G4Box(name + "_solid", size.x()/2.0 , size.y()/2.0 , size.z()/2.0);

  G4LogicalVolume *logical = new G4LogicalVolume( solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Si"), name + "_logical");
  //G4LogicalVolume *logical = new G4LogicalVolume( solid, G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"), name + "_logical");

  G4VisAttributes *vis = new G4VisAttributes( G4Color(1.0, 1.0, 0.0, 1.0) );
  vis->SetForceSolid(true);
  logical->SetVisAttributes(vis);

  G4VPhysicalVolume *physical = new G4PVPlacement( 0, pos, logical, name + "_physical", logicWorld, false, copyNum, OverlapCheck() );

  m_ActivePhysicalVolumesSet.insert( physical );

}

//____________________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddExitWindowForV3(G4ThreeVector Wsize, G4ThreeVector Wpos, G4ThreeVector Tr2size, G4ThreeVector Tr2pos, std::string material, G4LogicalVolume *logicWorld)
{

  std::string name = "LumiExitWinV3";
  G4Box *solid = new G4Box(name+"_solid",Tr2size.x()/2.0 + 1*cm, Tr2size.y() + Tr2pos.y()/2.0 + 2.0*cm, Wsize.z()/2.0);//2.0cm to adjust the overlap due to conical shape.

  G4LogicalVolume *logical = new G4LogicalVolume( solid, GetDetectorMaterial(material),name+"logical");
  G4VisAttributes *vis = new G4VisAttributes( G4Color(1, 0, 0, 1) );
  vis->SetForceSolid( true );
  logical->SetVisAttributes( vis );

  G4ThreeVector pos = G4ThreeVector(Wpos.x(), Wpos.y(), Tr2pos.z() - Tr2size.z()/2.0 - Wsize.z()/2.0);
  G4VPhysicalVolume *physical = new G4PVPlacement(0,pos, logical, name+"physical", logicWorld, 0, false, OverlapCheck());
  m_PassivePhysicalVolumesSet.insert( physical);

}


//_______________________________________________________________________________________________________________________________
void EICG4LumiDetector::AddCAL( std::string name, G4ThreeVector size, G4ThreeVector pos, int total_tower, G4LogicalVolume *logicWorld )
{

  G4int nxy = total_tower; // 32
  //dimensions for single tower
  G4double towerSizeXY = size.x()/nxy;
  //geo->GetOptD(nam, "towerSizeXY", towerSizeXY, GeoParser::Unit(mm));
  G4double towerEMZ = size.z();
  //geo->GetOptD(nam, "towerEMZ", towerEMZ, GeoParser::Unit(mm));
  //G4double zpos = towerEMZ/2;
  //G4double zpos = pos.z();
  //geo->GetOptD(nam, "zpos", zpos, GeoParser::Unit(mm));

  //geo->GetOptI(nam, "nxy", nxy);

  //module size for tower assembly, increased to allow for tower rotation
  G4double modxy = size.x(); //  + 40*mm
  G4double modz = size.z(); //  + 6*mm

  //G4cout << "    modxy: " << modxy << G4endl;
  //G4cout << "    modz: " << modz << G4endl;

  //top calorimeter volume
  G4Box *mods = new G4Box(name+"_mod", modxy/2, modxy/2, modz/2);
  G4LogicalVolume *modv = new G4LogicalVolume(mods, G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"), name+"_mod");
  new G4PVPlacement(0, G4ThreeVector(pos.x(), pos.y(), pos.z()), modv, name+"_mod", logicWorld, false, 0, OverlapCheck());
  //modv->SetVisAttributes(new G4VisAttributes(G4Color(0, 0, 1)));
  modv->SetVisAttributes( G4VisAttributes::GetInvisible() );

  // Construct individual towers
  G4LogicalVolume *towv = MakeTower(towerSizeXY, towerEMZ);

  G4int tcnt = 0;
  G4double xypos0 = -(nxy*towerSizeXY)/2 + towerSizeXY/2;
  for(G4int ix=0; ix<nxy; ix++) {
    for(G4int iy=0; iy<nxy; iy++) {
      G4double xpos = xypos0 + ix*towerSizeXY;
      G4double ypos = xypos0 + iy*towerSizeXY;
      new G4PVPlacement(0, G4ThreeVector(xpos, ypos, 0), towv, towv->GetName(), modv, false, tcnt++, OverlapCheck());
    }
  }

}

//______________________________________________________________________________________________________________________________
G4LogicalVolume* EICG4LumiDetector::MakeTower(G4double calorSizeXY, G4double calorEMZ) {

  const double offset=0.5;//in mm
  const double dist=1.0;
  const double tot_len=calorSizeXY;
  const double h=0.5*sqrt(3)*dist;

  const int nx1=int((tot_len-2*offset)/(dist/2))+1;
  const int ny1=int((tot_len-offset)/(2*h))+1;
  const int ny2=int((tot_len-offset-h)/(2*h))+1;

  const double x0=-((tot_len/2.0)-offset);
  const double y01=((tot_len/2.0)-offset);
  const double y02=((tot_len/2.0)-offset-h);

  G4Material* defaultMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  G4Material* gapMaterial2 = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYSTYRENE");
  G4double a = 183.85*g/mole;
  G4Element* elW = new G4Element("Tungsten","W", 74., a);
  
  G4Material* EMCal_abs_mat = GetDetectorMaterial( "EMCal_fiber_mat", false ); // false suppress warnings
  if( ! EMCal_abs_mat ) {
    EMCal_abs_mat = new G4Material("EMCal_fiber_mat", 12.4*g/cm3, 2);
    EMCal_abs_mat->AddElement(elW, 96.0*perCent);
    EMCal_abs_mat->AddMaterial(gapMaterial2, 4.0*perCent);
  }

  //EM
  G4LogicalVolume* calorEM;
  G4VSolid* calorimeterEM = new G4Box("CalorimeterEM_", calorSizeXY/2, calorSizeXY/2, calorEMZ/2);
  calorEM=new G4LogicalVolume(calorimeterEM,defaultMaterial, m_Name+"_CalorEM");
    
  //absorber
  G4LogicalVolume* absorberEMLV;
  G4VSolid* absorberEM = new G4Box(m_Name+"_AbsoEM_solid",            // its name
                                     calorSizeXY/2, calorSizeXY/2, calorEMZ/2); // its size
  absorberEMLV = new G4LogicalVolume(absorberEM,EMCal_abs_mat, m_Name+"_AbsoEM");
  G4VPhysicalVolume *physical_absorber = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),absorberEMLV, m_Name+"_AbsoEM_p",calorEM,false,0, OverlapCheck());
  m_PassivePhysicalVolumesSet.insert( physical_absorber );

  G4VSolid* gapEM = new G4Tubs(m_Name+"_GapEM",             // its name0, pos, sheetLogic, name + "_g4physical", logicWorld, false, 0, OverlapCheck() );
                               0.0, 0.235*mm, calorEMZ/2,0.0,360.0 * deg); // its size//0.0 * deg, 360.0 * deg
    
  //Fibers
  G4LogicalVolume* gapEMLV;
  gapEMLV = new G4LogicalVolume(gapEM,gapMaterial2, m_Name);
  int copynono=0;
  double step_x=(dist/2.0)*mm;
  double step_y=(2.0*h)*mm;
  //G4cout<<"Nx: "<<nx1<<" Ny1: "<<ny1<<" Ny2: "<<ny2<<" step_x: "<<step_x<<" step_y: "<<step_y<<" x0: "<<x0<<" y01: "<<y01<<" y02: "<<y02<<G4endl;

  for(int i=0;i<nx1;i++){
    G4double pos_x=x0*mm+i*step_x;
    G4double pos_y=0.0;
    //if(i==(nFibAr-1)) continue;
    // if(i%2==0) pos_x=(-29.95+i*(0.05))*cm;
    // if(i%2!=0) pos_x=(-29.9+i*(0.05))*cm;
    // pos_x=(-29.95+i*(0.1))*cm;
    int jend=(i%2==0) ? ny1 : ny2;
    for(int j=0;j<jend;j++){
            
            
      if(i%2==0) pos_y=y01*mm-j*step_y;
      if(i%2!=0) pos_y=y02*mm-j*step_y;
      G4VPhysicalVolume *physical_scint = new G4PVPlacement(0,G4ThreeVector(pos_x,pos_y, 0.),gapEMLV,m_Name+"_EMGapPhysical",absorberEMLV,false,copynono, OverlapCheck());
      
      // Add to active layers
      m_ActivePhysicalVolumesSet.insert( physical_scint );

      //new G4PVPlacement(0,G4ThreeVector(pos_x,pos_y, 0.),gapEMLV,"EMGapPhysical",absorberEMLV,0,copynono);//first try
      copynono++;
      //G4cout<<"Point # "<<copynono<<" x: "<<pos_x<<" y: "<<pos_y<<G4endl;

    }
  }

  absorberEMLV->SetVisAttributes( G4VisAttributes::GetInvisible() );
  gapEMLV->SetVisAttributes( G4VisAttributes::GetInvisible() );

  G4VisAttributes* calorEMvis= new G4VisAttributes(G4Colour(1,0,1));//magenta calorimeter
  calorEMvis->SetForceAuxEdgeVisible(true);
  calorEM->SetVisAttributes(calorEMvis);

  return calorEM;
}

