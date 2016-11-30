#include "onevoter.h"
/******************************************************************************
 * Implementation for the 'OneVoter' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Date: 6 October 2016
 *
 * Used with permission by: Group 1 (BRYCE ANDREW BLANTON,
 * WILLIAM R EDWARDS, JONATHAN SENN, SPENCER C SMITH,
 * CELENA CHRISTINE WILLIAMS)
 * Date last modified: 1 December 2016
 *
 * This class is used for the creation of an individual voter so that the
 * respective times for a voter can be outtputted to check for anomlies in 
 * data.
 *
**/

//orginal string for data output
static const string kTag = "ONEVOTER: ";

/******************************************************************************
 * Constructor.
 **/

OneVoter::OneVoter() {
}
/******************************************************************************
 * Custom constructor for a voter which has assigned sequence, arrival time, 
 * and duration spent at polling station.
 **/

OneVoter::OneVoter(int sequence, int arrival_seconds,
                   int duration_seconds) {
  sequence_ = sequence;
  time_arrival_seconds_ = arrival_seconds;
  time_start_voting_seconds_ = 0;
  time_vote_duration_seconds_ = duration_seconds;
  
  //note that the voting stations are started at 0 instead of 1
  which_station_ = -1;
}

/******************************************************************************
 *Default constructor for a voter
 **/

OneVoter::~OneVoter() {
}

/******************************************************************************
 * Accessors and mutators.
 **/

/******************************************************************************
 * Returns the arrival time of an individual voter
 **/

int OneVoter::GetTimeArrival() const {
  return time_arrival_seconds_;
}

/******************************************************************************
 * Returns the waiting time of an individual voter
 **/

int OneVoter::GetTimeWaiting() const {
  return time_waiting_seconds_;
}

/******************************************************************************
 * Returns the polling station number of an individual voter
 **/

int OneVoter::GetStationNumber() const {
  return which_station_;
}

/******************************************************************************
 * General functions.
 **/

/******************************************************************************
 * AssignStation take in a station number and a start time. The method then
 * calculates the length of time that it takes a voter to vote at their polling
 * station. This method also calculates the total time the voter waited until
 * reaching the polling station.
 **/

void OneVoter::AssignStation(int station_number,
                             int start_time_seconds) {
  which_station_ = station_number;
  time_start_voting_seconds_ = start_time_seconds;
  time_done_voting_seconds_ = time_start_voting_seconds_
                              + time_vote_duration_seconds_;
  time_waiting_seconds_ = time_start_voting_seconds_
                          - time_arrival_seconds_;
}

/******************************************************************************
 * GetTimeDoneVoting calculates the time that the individual voter completesk
 * voting. It does this by adding start time and duration to get completion
 * time.
 **/

int OneVoter::GetTimeDoneVoting() const {
  return time_start_voting_seconds_ + time_vote_duration_seconds_;
}

/******************************************************************************
 * GetTimeInQ calculates the time that the individual voter spends in the line
 * queue. It does this by subtracting start time from the arrival time to get
 * the total time spent in line waiting to vote.
 **/

int OneVoter::GetTimeInQ() const {
  return time_start_voting_seconds_ - time_arrival_seconds_;
}

/******************************************************************************
 * GetTOD calculates the time of day. It does this by taking in a time in 
 * seconds which then can have an offset time added to it, which represents the
 * opening time of the polling station which is then converted into a time in 
 * seconds via the ConvertTime method. The returned converted time in seconds
 * is then returned as a string
 **/

string OneVoter::GetTOD(int time_in_seconds) const {
//  int offset_hours = 6;
  int offset_hours = 0;
  //  string s = ""; //Dead code
  return this->ConvertTime(time_in_seconds + offset_hours*3600);
}

/******************************************************************************
 * ConvertTime takes in an int,  time_in_seconds. This number is then used to
 * create a formatted number output in the form of 'HH:MM:SS'. It does this by
 * dividing time_in_seconds to calculate hours, minutes, and seconds. The
 * corresponding output is then formated by the included utils package. The
 * resulting formated string is then returned.
 **/
string OneVoter::ConvertTime(int time_in_seconds) const {
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  string s = "";

  //int automatically rounds down to the nearest hour
  hours = time_in_seconds / 3600;
  //subtracts the number of hours from time then calculates minutes
  minutes = (time_in_seconds - 3600 * hours) / 60;
  //subtracts the number of hours and minutes from time then calculates
  //seconds
  seconds = (time_in_seconds - 3600 * hours - 60 * minutes);

  s += Utils::Format(time_in_seconds, 6);

  if (hours < 0)
    s += " 00";
  else if (hours < 10)
    s += " 0" + Utils::Format(hours, 1);
  else
    s += " " + Utils::Format(hours, 2);

  if (minutes < 0)
    s += ":00";
  else if (minutes < 10)
    s += ":0" + Utils::Format(minutes, 1);
  else
    s += ":" + Utils::Format(minutes, 2);

  if (seconds < 0)
    s += ":00";
  else if (seconds < 10)
    s += ":0" + Utils::Format(seconds, 1);
  else
    s += ":" + Utils::Format(seconds, 2);

  return s;
} // string OneVoter::ConvertTime(int time_in_seconds) const

/******************************************************************************
 * OneVoter::ToString outputs the individual characteristics for a voter.  The
 * the output is setup and formatted to have multiple voter outputs at a time.
 * The order of output is: sequence, arrival time, start time, voting duration
 * end time, total wait time, and which station the voter was assigned to. The
 * output data was formatted by using the external Utils package. The resulting
 * formatted string is then outputted.
 **/

string OneVoter::ToString() {
  string s = kTag;

  s += Utils::Format(sequence_, 7);
  s += ": ";
  s += Utils::Format(this->GetTOD(time_arrival_seconds_));
  s += " ";
  s += Utils::Format(this->GetTOD(time_start_voting_seconds_));
  s += " ";
  s += Utils::Format(this->ConvertTime(time_vote_duration_seconds_));
  s += " ";
  s += Utils::Format(this->GetTOD(time_start_voting_seconds_
                                  + time_vote_duration_seconds_));
  s += " ";
  s += Utils::Format(this->ConvertTime(GetTimeInQ()));
  s += ": ";
  s += Utils::Format(which_station_, 4);

  return s;
} // string OneVoter::toString()

/******************************************************************************
 *ToStringHeader is the header that goes above the output, OneVoter::ToString, 
 * so that the output is properly labled and organized.
 **/

string OneVoter::ToStringHeader() {
  string s = kTag;
  s += "    Seq        Arr           Start             Dur             End";
  s += "            Wait         Stn";
  return s;
}

