#include "onepct.h"
/*******************************************************************************
 * Implementation for the 'OnePct' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Date: 21 May 2013
 *
 * Used with permission by: Group 1 (BRYCE ANDREW BLANTON,
 * WILLIAM R EDWARDS, JONATHAN SENN, SPENCER C SMITH,
 * CELENA CHRISTINE WILLIAMS)
 * Date last modified: 1 December 2016
 *
**/

static const string kTag = "OnePct: ";

/*******************************************************************************
* Constructor.
**/

OnePct::OnePct() {
}
/*******************************************************************************
* Constructor.
**/

OnePct::OnePct(Scanner& infile) {
  this->ReadData(infile);
}

/*******************************************************************************
* Destructor.
**/
OnePct::~OnePct(){
}

/*******************************************************************************
* Accessors and mutators.
**/

int OnePct::GetExpectedVoters() const {
  return pct_expected_voters_;
}

/*******************************************************************************
**/

int OnePct::GetPctNumber() const {
  return pct_number_;
}

/*******************************************************************************
* General functions.
*
*
* Commented by jsenn 11/21
* Function 'ComputeMeanAndDev()
* Computes the mean and the standard deviation of the waiting times and sets
* wait_mean_senconds_ and wait_dev_seconds_
**/

void OnePct::ComputeMeanAndDev() {
  int sum_of_wait_times_seconds = 0;
  double sum_of_adjusted_times_seconds = 0.0;
  // Unnecessary code: sum_of_wait_times_seconds = 0;
  multimap<int, OneVoter>::iterator iter_multimap;

  // Sum the waiting times
  for (iter_multimap = voters_done_voting_.begin();
       iter_multimap != voters_done_voting_.end(); ++iter_multimap) {
    OneVoter voter = iter_multimap->second;

    sum_of_wait_times_seconds += voter.GetTimeWaiting();
  }

  // Set wait mean
  wait_mean_seconds_ = static_cast<double>(sum_of_wait_times_seconds)
                       / static_cast<double>(pct_expected_voters_);
  // Unecessary code: sum_of_adjusted_times_seconds = 0.0;

  // Sum the square of the waiting times - mean waiting time
  for (iter_multimap = voters_done_voting_.begin();
       iter_multimap != voters_done_voting_.end(); ++iter_multimap) {

    OneVoter voter = iter_multimap->second;
    double this_addin = static_cast <double>(voter.GetTimeWaiting())
                      - wait_mean_seconds_;

    sum_of_adjusted_times_seconds += (this_addin)*(this_addin);
  }

  // Set wait standard deviation
  wait_dev_seconds_ = sqrt(sum_of_adjusted_times_seconds
                    /static_cast<double>(pct_expected_voters_));
}

/*******************************************************************************
 * Commented by Jonathan Senn 11/21
 * Function: 'Create Voters'
 * Given a particular simulation configuration, generate indivual voters and
 * store them in voter_backup_. Config provides voters in each hour, hours the
 * precint is open, and a voting duration distribution
 *
 * Voters are given a suquence number, an arrival time, and a voting length.
 * For voters at time=0, arrival time is zero, duration is determined by
 * randomly sampling from the voting duration list, and sequence is iterative.
 * For all voters after time=0, arrival time is determined by a random
 * exponentially distributed interval + previous arrival time.
 *
 * The backup is made in two steps: voters at time = 0, voters at time > 0
 * though it is the same process.
**/

void OnePct::CreateVoters(const Configuration& config, MyRandom& random,
                          ofstream& out_stream){
  int duration = 0;
  int arrival = 0;
  int sequence = 0;
  double percent = 0.0;
  string outstring = "XX";

  voters_backup_.clear();
  // Unnecessary code: sequence = 0;

  // Get the percent of voters arriving at open and calculate the actual number
  percent = config.arrival_zero_;
  int voters_at_zero = round((percent / 100.0) * pct_expected_voters_);
  arrival = 0;

  // For each voter, create a Voter instance with random time taken to vote
  // arrival time at 0, and their arrival number
  for (int voter = 0; voter < voters_at_zero; ++voter) {
    int duration_sub = random.RandomUniformInt(0,
                                               config.GetMaxServiceSubscript());
    duration = config.actual_service_times_.at(duration_sub);
    OneVoter one_voter(sequence, arrival, duration);

    // Map the arrival times to each voter
    voters_backup_.insert(std::pair<int, OneVoter>(arrival, one_voter));
    ++sequence;
  }

  // For each hour on election day
  for (int hour = 0; hour < config.election_day_length_hours_; ++hour) {
    percent = config.arrival_fractions_.at(hour);
    int voters_this_hour = round((percent / 100.0) * pct_expected_voters_);

    if (0 == hour%2)
      ++voters_this_hour;

    // Arrival second
    int arrival = hour*3600;

    // For each voter in the hour, starting from hour + 0 seconds, create
    // an instance of voter with arrival time at previous arrival + poisson
    // interval, random time taken to vote, and nth customer position
    for (int voter = 0; voter < voters_this_hour; ++voter) {
      // Calculate arrival time of next individual given a mean of lamba and
      // an exponential distribution.
      double lambda = static_cast<double>(voters_this_hour / 3600.0);
      int interarrival = random.RandomExponentialInt(lambda);
      arrival += interarrival;

      int duration_sub = random.RandomUniformInt(0,
                                               config.GetMaxServiceSubscript());
      duration = config.actual_service_times_.at(duration_sub);

      OneVoter one_voter(sequence, arrival, duration);
      voters_backup_.insert(std::pair <int, OneVoter>(arrival, one_voter));
      ++sequence;
    }
  }
}

/*******************************************************************************
 * Function DoStatistics
 * Returns the number of voters that waited beyond an ideal time.
 *
 * Additionally:
 * Scrapes the waiting times in minutes for histogram, and calculates the
 * number of voters that waited beyond an ideal wait time. The results are
 * Printed in a formatted manner
**/

int OnePct::DoStatistics(int iteration, const Configuration& config,
                          int station_count, map<int, int>& map_for_histo,
                          ofstream& out_stream) {
  string outstring = "\n";
  map<int, int>wait_time_minutes_map;

/*******************************************************************************
 * Calculate how many minutes each voter had to wait. Store to histo map and
 * wait_time_minutes_map
**/
  multimap<int, OneVoter>::iterator iter_multimap;
	
  for(iter_multimap = this->voters_done_voting_.begin();
      iter_multimap!=this->voters_done_voting_.end(); ++iter_multimap){
    OneVoter voter = iter_multimap->second;
    // Turn Seconds to minutes
    int wait_time_minutes = voter.GetTimeWaiting() / 60;

    ++(wait_time_minutes_map[wait_time_minutes]);
    ++(map_for_histo[wait_time_minutes]);
  }

/*******************************************************************************
 * For each waitime observed, calculate how many waited longer than desired
 * wait time, desired time + 10, and + 20
**/

  int toolongcount = 0;
  int toolongcountplus10 = 0;
  int toolongcountplus20 = 0;

  for (auto iter = wait_time_minutes_map.rbegin();
       iter != wait_time_minutes_map.rend(); ++iter) {
    int waittime = iter->first;
    int waitcount = iter->second;

    if (waittime > config.wait_time_minutes_that_is_too_long_)
      toolongcount += waitcount;

    if (waittime > config.wait_time_minutes_that_is_too_long_+10)
      toolongcountplus10 += waitcount;

    if (waittime > config.wait_time_minutes_that_is_too_long_+20)
      toolongcountplus20 += waitcount;
  }

/*******************************************************************************
 * Print out data about this precint in formatted manner
**/

  ComputeMeanAndDev();
  outstring = "" + kTag;
  outstring.append(Utils::Format(iteration, 3) + " ");
  outstring.append(Utils::Format(pct_number_, 4) + " ");
  outstring.append(Utils::Format(pct_name_, 25, "left"));
  outstring.append(Utils::Format(pct_expected_voters_, 6));
  outstring.append(Utils::Format(station_count, 4));
  outstring.append(" stations, mean/dev wait (mins) ");
  outstring.append(Utils::Format(wait_mean_seconds_ / 60.0, 8, 2) + " ");
  outstring.append(Utils::Format(wait_dev_seconds_ / 60.0, 8, 2) + " toolong ");
  outstring.append(Utils::Format(toolongcount, 6) + " ");
  outstring.append(Utils::Format(100.0*toolongcount
                    / static_cast<double>(pct_expected_voters_), 6, 2));
  outstring.append(Utils::Format(toolongcountplus10, 6) + " ");
  outstring.append(Utils::Format(100.0*toolongcountplus10
                    / static_cast<double>(pct_expected_voters_), 6, 2));
  outstring.append(Utils::Format(toolongcountplus20, 6) + " ");
  outstring.append(Utils::Format(100.0*toolongcountplus20
                    / static_cast<double>(pct_expected_voters_), 6, 2));
  outstring.append("\n");

  Utils::Output(outstring, out_stream, Utils::log_stream);

  wait_time_minutes_map.clear();

  return toolongcount;

}  //End OnePct::DoStatistics{...}

/*******************************************************************************
 * Function: 'ReadData'
 * Reads one line of data from a structured file, infile and intializes
 * necessary variables for the precint.
 *
**/

void OnePct::ReadData(Scanner& infile) {
  if (infile.HasNext()) {
    pct_number_ = infile.NextInt();
    pct_name_ = infile.Next();
    pct_turnout_ = infile.NextDouble();
    pct_num_voters_ = infile.NextInt();
    pct_expected_voters_ = infile.NextInt();
    pct_expected_per_hour_ = infile.NextInt();
    pct_stations_ = infile.NextInt();
    pct_minority_ = infile.NextDouble();

    int stat1 = infile.NextInt();
    int stat2 = infile.NextInt();
    int stat3 = infile.NextInt();
    stations_to_histo_.insert(stat1);
    stations_to_histo_.insert(stat2);
    stations_to_histo_.insert(stat3);
  }
}  // void OnePct::ReadData(Scanner& infile)

/*******************************************************************************
 * Commented by Bryce Blanton
 * November 29 2016
 *
 *
 * RunSimulationPct is what cranks out the sim to get all of the data. It runs
 * the sim wth differnt numbers of stations each time (that is the outer
 * for loop). The inner loop runs as many times as is specified by
 * config.number_of_iterations_ or until a perfect scenario is found, no voters
 * wait longer than is specified in config.wait_time_minutes_that_is_too_long_.
 *
 * Wait times are calculated for each number of stations by RunSimulationPct2.
 *
 * The results of each simulation are sent to out_stream.
 *
 * If histograms are specified in onepct.stations_to_histo_, then a histogram
 * of that station is generated and sent to out_stream.
 *
**/

void OnePct::RunSimulationPct(const Configuration& config,
                              MyRandom& random, ofstream& out_stream){
  string outstring = "XX";

  // The minimum number of stations = the total time taken / time available
  int min_station_count = pct_expected_voters_
                         *config.time_to_vote_mean_seconds_;
  min_station_count = min_station_count;

  // At least one station is needed
  if (min_station_count <= 0)
    min_station_count = 1;

  int max_station_count = min_station_count + config.election_day_length_hours_;

  bool done_with_this_count = false;

  // For each possible station count determine if perfect case exists and
  for (int stations_count = min_station_count;
       stations_count <= max_station_count; ++stations_count) {

    if (done_with_this_count)
      break;
    done_with_this_count = true;

    map <int, int> map_for_histo;

    outstring = kTag + this->ToString() + "\n";
    Utils::Output(outstring, out_stream, Utils::log_stream);

  // For n times, run the simulation and check for a perfect outcome:
  // no one waits longer than specified. Otherwise use the last simulation
    for (int iteration = 0; iteration < config.number_of_iterations_;
         ++iteration) {
      this->CreateVoters(config, random, out_stream);

      voters_pending_ = voters_backup_;
      voters_voting_.clear();
      voters_done_voting_.clear();

      this->RunSimulationPct2(stations_count);

      int number_too_long = DoStatistics(iteration, config, stations_count,
                                         map_for_histo, out_stream);
      if (number_too_long > 0)
        done_with_this_count = false;
    }  // for (int iteration = 0; iteration < config.number_of_iterations_;

    voters_voting_.clear();
    voters_done_voting_.clear();

    // Print simulation result to out_stream and log_stream
    outstring = kTag + "toolong space filler\n";
    Utils::Output(outstring, out_stream, Utils::log_stream);

    // If any histograms are desired as specified in stations_to_histo_,
    // generate and send to out_stream
    if (stations_to_histo_.count(stations_count) > 0) {
      outstring = "\n" + kTag + "HISTO " + this->ToString() + "\n";
      outstring.append(kTag + "HISTO STATIONS ");
      outstring.append(Utils::Format(stations_count, 4) + "\n");
      Utils::Output(outstring, out_stream, Utils::log_stream);

      int time_lower = (map_for_histo.begin())->first;
      int time_upper = (map_for_histo.rbegin())->first;

      int voters_per_star = 1;

      if (map_for_histo[time_lower] > 50) {
        voters_per_star = map_for_histo[time_lower]
                          / (50 * config.number_of_iterations_);
        if (voters_per_star <= 0)
          voters_per_star = 1;
      }

      for(int time = time_lower; time <= time_upper; ++time){
        int count = map_for_histo[time];

        double count_double = static_cast<double>(count)
                            /static_cast<double>(config.number_of_iterations_);

        int count_divided_ceiling = static_cast<int>(ceil(count_double
                                                            / voters_per_star));
        string stars = string(count_divided_ceiling, '*');

        outstring = kTag + "HISTO " + Utils::Format(time, 6) + ": ";
        outstring.append(Utils::Format(count_double, 7, 2) + ": ");
        outstring.append(stars + "\n");
        Utils::Output(outstring, out_stream, Utils::log_stream);
      }  // for (int time = time_lower ; time <= time_upper; ++time) {

      outstring = "HISTO\n\n";
      Utils::Output(outstring, out_stream, Utils::log_stream);
    }  // if (stations_to_histo_.count (stations_count) > 0) {
  }  // for (int stations_count = min_station_count; ...

}

/*******************************************************************************
 * Commented by Bryce Blanton
 * November 29 2016
 *
 *
 * RunSimulationPct2 is what is doing the time checking; it tests the amount of
 * voters and stations to find how long it will take for votes to be cast.
 * It does this by filling up the 'free stations' with voters then adds the
 * time to vote, and then when another 'voter' shows up it checks for free
 * stations then fills them with the 'pending voters'. It also keeps track of
 * the enter and exit times to make sure that it is not taking too long. This
 * ends when the 'voters pending' and the 'voters voting' are both 0.
 *
 *
**/
void OnePct::RunSimulationPct2(int stations_count) {
  free_stations_.clear();

  // Start wait time simulations with all stations available
  for (int i = 0; i < stations_count; ++i) {
    free_stations_.push_back(i);
  }

  voters_voting_.clear();
  voters_done_voting_.clear();

  int second = 0;
  bool done = false;

  while (!done) {
    // For each voter at a voting station. If the departure time = present time
    // Make their station available and add them to voters_dont_voting
    for (auto iter = voters_voting_.begin(); iter != voters_voting_.end();
         ++iter) {
      if (second == iter->first) {
        OneVoter one_voter = iter->second;

        int which_station = one_voter.GetStationNumber();
        free_stations_.push_back(which_station);
        voters_done_voting_.insert(std::pair<int, OneVoter>(second, one_voter));
      }
    }

    // Empty all voters that just departed
    voters_voting_.erase(second);
    vector<map<int, OneVoter>::iterator> voters_pending_to_erase_by_iterator;

    // For each voter that has not yet "arrived"
    // If the second in simulation is the arrival time of the voter & there is a
    // open station
    for (auto iter = voters_pending_.begin(); iter != voters_pending_.end();
         ++iter) {
      if (second >= iter->first && free_stations_.size() > 0) {

          OneVoter next_voter = iter->second;

        // Redundant check. iter->first = iter->second.GetTimeArrival()
          if (next_voter.GetTimeArrival() <= second) {
            // Remove a free station from the free station list and assign it
            // the voter
            int which_station = free_stations_.at(0);
            free_stations_.erase(free_stations_.begin());
            next_voter.AssignStation(which_station, second);

            // Add the voter to the voting map
            int leave_time = next_voter.GetTimeDoneVoting();
            voters_voting_.insert(std::pair<int, OneVoter>
                                  (leave_time, next_voter));
            voters_pending_to_erase_by_iterator.push_back(iter);
/*******************************************************************************
 * This was commented out 6 October 2016
 * Utils::log_stream << kTag << "ASSIGNED    "
 *                   << Utils::Format(second, 5) << ": "
 *                   << next_voter.ToString() << "\n";
 
 * Utils::log_stream << kTag << "PENDING, VOTING, DONE    "
 *                   << Utils::Format((int)voters_pending_.size(), 5)
 *		     << ": "
 *                   << Utils::Format((int)voters_voting_.size(), 5)
 *		     << ": "
 *                   << Utils::Format((int)voters_done_voting_.size(), 5)
 * 		     << endl;
**/
          }  // if (next_voter.GetTimeArrival() <= second) {
      } else {    // if (second == iter->first && free_stations_.size () > 0) {
        break;  // we have walked in time past current time to
                // arrivals in the future OR no stations are available
      }
    }  // for (auto iter = voters_pending_.begin();

    // Remove the voters from the wait list who are now voting
    for (auto iter = voters_pending_to_erase_by_iterator.begin();
         iter != voters_pending_to_erase_by_iterator.end(); ++iter) {
      voters_pending_.erase(*iter);
    }

    // Time step forward
    ++second;
    // if (second > 500) break;
    done = true;

    // If people are still voting, or haven't voted yet, run again.
    if ((voters_pending_.size() > 0) || (voters_voting_.size() > 0)) {
      done = false;
    }
  }  // while (!done) {

}  // void Simulation::RunSimulationPct2()

/*******************************************************************************
 *
 * Commented by Bryce Blanton
 * November 29 2016
 *
 *
 * This simply prints out all the data:
 * Precinct Number, Precint Name, Precint Turnout, Voter Turnout, Expected
 * Voters, Expected number of Voters per Hour, and the Number of Stations
 *
 *
**/
string OnePct::ToString(){
  string s = "";

  s.append(Utils::Format(pct_number_, 4) + " ");
  s.append(Utils::Format(pct_name_, 25, "left"));
  s.append(Utils::Format(pct_turnout_, 8, 2));
  s.append(Utils::Format(pct_num_voters_, 8));
  s.append(Utils::Format(pct_expected_voters_, 8));
  s.append(Utils::Format(pct_expected_per_hour_, 8));
  s.append(Utils::Format(pct_stations_, 3));
  s.append(Utils::Format(pct_minority_, 8, 2));
  s.append(" HH ");

  for (auto iter = stations_to_histo_.begin(); iter != stations_to_histo_.end();
       ++iter) {
    s.append(Utils::Format(*iter, 4));
  }
  s.append(" HH");

  return s;
}  // string OnePct::ToString()

/*******************************************************************************
 *
 *
 * Commented by Bryce Blanton
 * November 29 2016
 *
 * This prints the ToString by iterating through 'themap'
 *
 *
 *
**/
string OnePct::ToStringVoterMap(string label, multimap<int, OneVoter> themap) {
  string s =  "\n" + label + " WITH ";

  s.append(Utils::Format(static_cast<int>(themap.size()), 6) + " ENTRIES\n");
  s.append(OneVoter::ToStringHeader() + "\n");

  for (auto iter = themap.begin(); iter != themap.end(); ++iter) {
    s.append((iter->second).ToString() + "\n");
  }

  return s;
}  // string OnePct::ToString()
