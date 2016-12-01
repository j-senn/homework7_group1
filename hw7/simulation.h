/*******************************************************************************
 * Header for the 'Simulation' class.
 *
 * Author/copyright:  Duncan Buell
 * Date: 19 July 2016
 *
 * Used with permission by: Group 1 (BRYCE ANDREW BLANTON,
 * WILLIAM R EDWARDS, JONATHAN SENN, SPENCER C SMITH,
 * CELENA CHRISTINE WILLIAMS)
 * Date last modified: 1 December 2016
 *
 * This class sets up and manages the precinct simulations.
**/

#ifndef SIMULATION_H
#define SIMULATION_H

#include <map>

#include "../Utilities/utils.h"
#include "../Utilities/scanner.h"
#include "../Utilities/scanline.h"

using namespace std;

#include "configuration.h"
#include "onepct.h"

class Simulation {
public:
/*******************************************************************************
 * Constructors and destructors for the class. 
**/
  Simulation();
  virtual ~Simulation();

/*******************************************************************************
 * General functions.
**/


/*******************************************************************************
 * Function 'ReadPrecincts'
 * Commented by William Edwards, 11/30/16
 *
 * Reads the data needed to initialize each precinct simulation.
 *
 * Arguments:
 *   Scanner& infile - A Scanner instance which reads data from the input file.
**/
  void ReadPrecincts(Scanner& infile);


/*******************************************************************************
 * Function 'RunSimulation'
 * Commented by William Edwards, 11/30/16
 *
 * This function sets up and runs the simulation for each precinct.
 *
 * Arguments:
 *   Configuration& config - An instance of the 'Configuration' class which
 *     contains certain parameters for the simulation
 *   MyRandom& random - An instance of a random number generator.
 *   ofstream& out_stream - A data stream corresponding to the output file.
**/
  void RunSimulation(const Configuration& config, MyRandom& random,
                     ofstream& out_stream);
/*******************************************************************************
 * Function 'ToString'.
 * Commented by William Edwards, 11/30/16
 *
 * Converts the data of the 'Simulation' instance into a string representation.
**/
  string ToString();
  // string ToStringPcts(); // This function does not exist.

private:
  
/*******************************************************************************
 * Variables.
**/

/******************************************************************************
 * Member variable 'pcts_'
 * Commented by William Edwards, 11/30/16
 *
 * Uses precinct number as a key and a ‘OnePct’ instance representing the
 * precinct as the value.  All precincts in the simulation
 * are stored in this variable.
**/
  map<int, OnePct> pcts_;

/*******************************************************************************
 * Private functions.
**/
  
};

#endif // SIMULATION_H
