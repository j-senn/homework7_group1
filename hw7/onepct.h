/*******************************************************************************
 * Header for the 'OnePct' class
 *
 * Author/copyright:  Duncan Buell
 * Date: 6 October 2016
 * Used with permission by: Group 1 (BRYCE ANDREW BLANTON,
 * WILLIAM R EDWARDS, JONATHAN SENN, SPENCER C SMITH,
 * CELENA CHRISTINE WILLIAMS)
 * Date last modified: 1 December 2016
 *
 * OnePct contains all necessary variables and functions to simulate the
 * wait times for each voter of a given voting precinct.
 *
**/

#ifndef ONEPCT_H
#define ONEPCT_H

#include <cmath>
#include <map>
#include <set>
#include <vector>

#include "../Utilities/utils.h"
#include "../Utilities/scanner.h"
#include "../Utilities/scanline.h"

using namespace std;

#include "configuration.h"
#include "myrandom.h"
#include "onevoter.h"

static const double kDummyDouble = -88.88;
static const int kDummyInt = -999;
static const string kDummyString = "dummystring";

class OnePct {
public:
 
/*******************************************************************************
 * Constructors and destructors for the class. 
**/
 
 OnePct();
 OnePct(Scanner& infile);
 virtual ~OnePct();

/*******************************************************************************
 * Accessors and Mutators.
**/
 // Return pct_expected_voters_

 int GetExpectedVoters() const;
 // Returns pct_number_
 int GetPctNumber() const;

/******************************************************************************
 * General functions.
**/
 // Reads precinct data from a structured, space delimited file

 void ReadData(Scanner& infile);
 // Iteratively simulates the wait time outcomes for a number of stations
 // scenarios in a precinct and sends the results to out_stream
 void RunSimulationPct(const Configuration& config, MyRandom& random,
                       ofstream& out_stream);

 string ToString();
 string ToStringVoterMap(string label, multimap<int, OneVoter> themap);

private:
 int    pct_expected_voters_ = kDummyInt;
 int    pct_expected_per_hour_ = kDummyInt;
 double pct_minority_ = kDummyDouble;
 string pct_name_ = kDummyString;
 int    pct_number_ = kDummyInt;
 double pct_turnout_ = kDummyDouble;
 int    pct_stations_ = kDummyInt;
 int    pct_num_voters_ = kDummyInt;
 double wait_dev_seconds_;
 double wait_mean_seconds_;
 set<int> stations_to_histo_;
 vector<int> free_stations_;
 multimap<int, OneVoter> voters_backup_;
 multimap<int, OneVoter> voters_done_voting_;
 multimap<int, OneVoter> voters_pending_;
 multimap<int, OneVoter> voters_voting_;

/*******************************************************************************
 * General private functions.
**/
 // Populates voters_backup

 void CreateVoters(const Configuration& config, MyRandom& random,
                   ofstream& out_stream);
 // Calculates, Returns the number of voters who waited longer than desired
 // Additionally, prints out this precincts variables
 int DoStatistics(int iteration, const Configuration& config, 
                  int station_count, map<int, int>& map_for_histo,
       	          ofstream& out_stream);
 // Computes wait mean and standard deviation
 void ComputeMeanAndDev();
 // Calculates the waiting times for each voter
 void RunSimulationPct2(int stations);

};

#endif // ONEPCT_H
