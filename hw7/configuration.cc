#include "configuration.h"
/*******************************************************************************
 * Implementation for the 'Configuration' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * TODO update copyright
 * Date: 6 October 2016
**/

/*******************************************************************************
 * GetMaxServiceSubscript() returns the size of the vector minus 1.
 *
 * ReadConfiguration(Scanner& instream) takes in an instance of scanner and
 * manipulates it.
 *
 * ToString () takes all the calculations done in ReadConfiguration () and 
 * formats into a easily readable string to be output to the user.
**/

static const string kTag = "CONFIG: ";

/*******************************************************************************
 * Constructor.
**/
Configuration::Configuration(){
}

/*******************************************************************************
 * Destructor.
**/
Configuration::~Configuration(){
}

/*******************************************************************************
 * Accessors and mutators.
**/

int Configuration::GetMaxServiceSubscript() const{
  return static_cast<int>(actual_service_times_.size()) - 1;
}

/*******************************************************************************
 * General functions.
**/

void Configuration::ReadConfiguration(Scanner& instream){
  string line;
  ScanLine scanline;

  line = instream.NextLine();
  scanline.OpenString(line);
  seed_ = scanline.NextInt();
  election_day_length_hours_ = scanline.NextInt();
  election_day_length_seconds_ = election_day_length_hours_*3600;
  time_to_vote_mean_seconds_ = scanline.NextInt();
  min_expected_to_simulate_ = scanline.NextInt();
  max_expected_to_simulate_ = scanline.NextInt();
  wait_time_minutes_that_is_too_long_ = scanline.NextInt();
  number_of_iterations_ = scanline.NextInt();

  line = instream.NextLine();
  scanline.OpenString(line);
  arrival_zero_ = scanline.NextDouble();
  
  for(int sub = 0; sub < election_day_length_hours_; ++sub){
    double input = scanline.NextDouble();
    arrival_fractions_.push_back(input);
  }

  Scanner service_times_file;
  service_times_file.OpenFile("dataallsorted.txt");
  
  while(service_times_file.HasNext()){
    int the_time = service_times_file.NextInt();
    actual_service_times_.push_back(the_time);
  }
}

string Configuration::ToString(){
  int offset;
  
  string s = "\n" + kTag + "RN seed:\t";
  s.append(Utils::Format(seed_, 8) + "\n");
  s.append(kTag + "Election Day length:  ");
  s.append(Utils::Format(election_day_length_seconds_, 8) + " = ");
  s.append(Utils::Format(election_day_length_seconds_ / 3600.0, 8, 2) + " (");
  s.append(Utils::Format(election_day_length_hours_, 8, 2) + ") hours\n");
  s.append(kTag + "Time to vote mean:    ");
  s.append(Utils::Format(time_to_vote_mean_seconds_, 8) + " = ");
  s.append(Utils::Format(time_to_vote_mean_seconds_/60.0, 8, 2));
  s.append(" minutes\n" + kTag);
  s.append("Min and max expected voters for this simulation: ");
  s.append(Utils::Format(min_expected_to_simulate_, 8));
  s.append(Utils::Format(max_expected_to_simulate_, 8) + "\n");
  s.append("Wait time (minutes) that is 'too long': ");
  s.append(Utils::Format(wait_time_minutes_that_is_too_long_, 8) + "\n");
  s.append("Number of iterations to perform: ");
  s.append(Utils::Format(number_of_iterations_, 4) + "\n");
  s.append("Max service time subscript: ");
  s.append(Utils::Format(GetMaxServiceSubscript(), 6) + "\n");
  s.append(kTag + Utils::Format(0, 2) + "-" + Utils::Format(0, 2));
  s.append(" : " + Utils::Format(arrival_zero_, 7, 2) + "\n");
  
  for(UINT sub = 0; sub < arrival_fractions_.size(); ++sub){
    s.append(kTag + Utils::Format(offset+sub, 2));
    s.append("-" + Utils::Format(offset+sub + 1, 2));
    s.append(" : " + Utils::Format(arrival_fractions_.at(sub), 7, 2) + "\n");
  }
  
  s.append("\n");
  return s;
}
