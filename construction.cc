#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{   
    nCols = 100; nRows=100;

    fMessenger = new G4GenericMessenger(this, "/detector/","Detector Construction");

    fMessenger->DeclareProperty("nCols", nCols, "Number of columns");
    fMessenger->DeclareProperty("nRows", nRows, "Number of rows");
    
    DefineMaterial();

    isAtmosperic = true;

    //define the size of world volume 
        //determine the dimension for x y and z
            xWorld = 40*km  ;
            yWorld = 40*km  ;
            zWorld = 20*km  ;

}

MyDetectorConstruction::~MyDetectorConstruction()
{}

void MyDetectorConstruction::DefineMaterial()
{
    G4NistManager *nist = G4NistManager::Instance();
////////////////////////////////////////////////////////////////////////////////////////////////////

//Material

    //World Material 
        //Defining the world Material
            worldMat = nist->FindOrBuildMaterial("G4_AIR");
        //Adding the optical parameter to the material

            //Defining parameters
                G4double energy[2] = {1.239841939*eV/0.9,1.239841939*eV/0.2};

                G4double rindexWorld[2]= {1.0,1.0};

            //Add refractive index to the material
                G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
                mptWorld->AddProperty("RINDEX",energy,rindexWorld,2);
                worldMat->SetMaterialPropertiesTable(mptWorld);


    //Atmospheric Air

        //Defining Parameters
            G4double density0 = 1.29*kg/m3; //density on the ground
            G4double aN = 14.01*g/mole ; //the mole mass of the nitrogen
            G4double aO = 16.00*g/mole ; //the mole mass of the oxygen

            N = new G4Element("Nitrogen","N", 7 ,aN);
            O = new G4Element("Oxygen",  "O", 8 ,aO);

            //Demostrating the function of atmosphere density

                G4double f = 3;//degree of freedom
                G4double R = 8.3144626191532;//gas const
                G4double g0 = 9.81; // gravity const
                G4double kappa = (f+2)/f;//smth known in thermodynamics
                G4double T = 293.15; //Temperature on the ground 
                G4double M = (0.3*aO +0.7*aN)/1000.; //Total Molar mass

            //Assign of each layer its density
            for(G4int i = 0 ; i < 10 ; i++)
            {
                //we need a label for each layer then we use stringstream
                std::stringstream stri;
                stri << i;

                G4double h = 40e3/10.*i ; //height
                G4double density = density0*pow((1-(kappa)/kappa*M*g0*h/(R*T)),(1/(kappa-1)));//the formula

                //we have to assign it to Air 
                Air[i] = new G4Material("G4_AIR_"+stri.str(),density,2);
                Air[i]->AddElement(N,70*perCent);
                Air[i]->AddElement(O,30*perCent);
            }




}

void MyDetectorConstruction::ConstructAtmosphere()
{
    solidAtmosphere = new G4Box("solidAtmosphere", xWorld, yWorld,zWorld/10.);
    for(G4int i = 0; i < 10 ; i++)
    {
        logicAtmosphere[i] = new G4LogicalVolume(solidAtmosphere, Air[i],"logicAtmosphere");

        physAtmosphere[i]  = new G4PVPlacement(0,G4ThreeVector(0,0,zWorld/10.*2*i - zWorld+ zWorld/10.),logicAtmosphere[i],"physAtmosphere",logicWorld,false,i,true);
    }
    
}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
   
//Boxes
    //Creating the room / the mother box which contain Everything


        solidWorld = new G4Box("solidWorld",xWorld,yWorld,zWorld);
        logicWorld = new G4LogicalVolume(solidWorld,worldMat,"logicWorld");
        physWorld = new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicWorld,"physWorld",0,false,0,true);

    if(isAtmosperic)
        ConstructAtmosphere();

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
/*
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

    
    logicDetector->SetSensitiveDetector(sensDet);
       */ 
}