#include "simulation.h"
/*******************************************************************************
 * Implementation for the 'Simulation' class.
 * Commented by William Edwards, 11/30/16
 *
 * This class sets up and manages the precinct simulations.  Most of the heavy
 * lifting for the simulation is located in the OnePct class.  The Simulation
 * class serves mainly to coordinate the simulations of different precincts.
 *
 * Author/copyright:  Duncan Buell
 * Date: 19 July 2016
 *
 * Used with permission by: Group 1 (BRYCE ANDREW BLANTON,
 * WILLIAM R EDWARDS, JONATHAN SENN, SPENCER C SMITH,
 * CELENA CHRISTINE WILLIAMS)
 * Date last modified: 1 December 2016
 *
**/

static const string kTag = "SIM: ";

/*******************************************************************************
* Constructor.
**/
Simulation::Simulation() {
}

/*******************************************************************************
* Destructor.
**/
Simulation::~Simulation() {
}

/*******************************************************************************
* Accessors and mutators.
**/

/*******************************************************************************
* General functions.
**/

/*******************************************************************************
 * Function 'ReadPrecincts'
 * Commented by William Edwards, 11/30/16
 *
 * Reads the data needed to initialize each precinct simulation.  This is
 * accomplished by creating an instance of 'OnePct' for each precinct in the file
 * and calling the 'ReadData' for each precinct to parse the data for that
 * precinct.
 *
 * Arguments:
 *   Scanner& infile - A Scanner instance which reads data from the input file.
**/
void Simulation::ReadPrecincts(Scanner& infile) {
  while (infile.HasNext()) {
    OnePct new_pct;
    new_pct.ReadData(infile);
    pcts_[new_pct.GetPctNumber()] = new_pct;
  } // while (infile.HasNext()) {
} // void Simulation::ReadPrecincts(Scanner& infile) {

/*******************************************************************************
 * Function 'RunSimulation'
 * Commented by William Edwards, 11/30/16
 *
 * This function sets up and runs the simulation for each precinct.  For each
 * precinct, the function checks whether the number of expected voters is within
 * the range accepted for simuation.  If this check fails, then the precinct is
 * skipped.  Otherwise,  the simulation is run for that precinct by calling the
 * 'RunSimulationPct' function for that instance of 'OnePct'.  Details of this
 * process are printed to the to the output and log files.
 *
 * Arguments:
 *   Configuration& config - An instance of the 'Configuration' class which
 *     contains certain parameters for the simulation
 *   MyRandom& random - An instance of a random number generator.
 *   ofstream& out_stream - A data stream corresponding to the output file.
**/
void Simulation::RunSimulation(const Configuration& config,
                               MyRandom& random, ofstream& out_stream) {
  string outstring = "XX";
  int pct_count_this_batch = 0;
  for (auto iterPct = pcts_.begin(); iterPct != pcts_.end(); ++iterPct) {
    OnePct pct = iterPct->second;

    int expected_voters = pct.GetExpectedVoters();
    if ((expected_voters <= config.min_expected_to_simulate_) ||
        (expected_voters > config.max_expected_to_simulate_))
      continue;

    outstring = kTag + "RunSimulation for pct " + "\n";
    outstring += kTag + pct.ToString() + "\n";
    Utils::Output(outstring, out_stream, Utils::log_stream);

    ++pct_count_this_batch;
    pct.RunSimulationPct(config, random, out_stream);

    //    break; // we only run one pct right now
  } // for(auto iterPct = pcts_.begin(); iterPct != pcts_.end(); ++iterPct)

  outstring = kTag + "PRECINCT COUNT THIS BATCH "
              + Utils::Format(pct_count_this_batch, 4) + "\n";
  //  Utils::Output(outstring, out_stream);
  Utils::Output(outstring, out_stream, Utils::log_stream);
  //  out_stream << outstring << endl;
  //  out_stream.flush();
  //  Utils::log_stream << outstring << endl;
  //  Utils::log_stream.flush();
} // void Simulation::RunSimulation()

/*******************************************************************************
 * Function 'ToString'.
 * Commented by William Edwards, 11/30/16
 *
 * Converts the data of the 'Simulation' instance into a string representation.
 * This is accomplished by calling the 'ToString' method for each 'OnePct'
 * instance in pcts_.
**/
string Simulation::ToString() {
  string s;

  for (auto iterPct = pcts_.begin(); iterPct != pcts_.end(); ++iterPct) {
    s += kTag + (iterPct->second).ToString() + "\n";
  }

  return s;
} // string Simulation::ToString()

