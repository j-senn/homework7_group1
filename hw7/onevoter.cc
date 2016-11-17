#include "onevoter.h"
/*******************************************************************************
 * Implementation for the 'OneVoter' class.
 *
 * Author/copyright:  Duncan Buell. All rights reserved.
 * Date: 6 October 2016
 * TODO update copyright
 *
**/

static const string kTag = "ONEVOTER: ";

/*******************************************************************************
* Constructor.
**/
OneVoter::OneVoter () {
}

/*******************************************************************************
**/

OneVoter::OneVoter (int sequence, int arrival_seconds, int duration_seconds) {
  sequence_ = sequence;
  time_arrival_seconds_ = arrival_seconds;
  time_start_voting_seconds_ = 0;
  time_vote_duration_seconds_ = duration_seconds;
  which_station_ = -1;
}

/*******************************************************************************
**/

OneVoter::~OneVoter () {
}

/*******************************************************************************
* Accessors and mutators.
**/

int OneVoter::GetTimeArrival () const {
  return time_arrival_seconds_;
}

/*******************************************************************************
**/
int OneVoter::GetTimeWaiting () const {
  return time_waiting_seconds_;
}

/*******************************************************************************
**/
int OneVoter::GetStationNumber () const {
  return which_station_;
}

/*******************************************************************************
* General functions.
**/

void OneVoter::AssignStation (int station_number, int start_time_seconds) {
  which_station_ = station_number;
  time_start_voting_seconds_ = start_time_seconds;
  time_done_voting_seconds_ = time_start_voting_seconds_  
                            + time_vote_duration_seconds_;
  time_waiting_seconds_ = time_start_voting_seconds_ - time_arrival_seconds_;
}

/*******************************************************************************
**/

int OneVoter::GetTimeDoneVoting () const {
  return time_start_voting_seconds_ + time_vote_duration_seconds_;
}

/*******************************************************************************
**/
int OneVoter::GetTimeInQ () const {
  return time_start_voting_seconds_ - time_arrival_seconds_;
}

/*******************************************************************************
**/
string OneVoter::GetTOD (int time_in_seconds) const {
  //  int offset_hours = 6;
  int offset_hours = 0;
  //  string s = ""; //Dead code
  return this->ConvertTime (time_in_seconds + offset_hours * 3600);
}

/*******************************************************************************
**/
string OneVoter::ConvertTime (int time_in_seconds) const {
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  string s = "";

  hours = time_in_seconds / 3600;
  minutes = (time_in_seconds - 3600 * hours) / 60;
  seconds = (time_in_seconds - 3600 * hours - 60 * minutes);

  s.append (Utils::Format(time_in_seconds, 6));

  if (hours < 0)
    s.append (" 00");
  else
    if (hours < 10)
      s.append (" 0" + Utils::Format (hours, 1));
    else
      s.append (" " + Utils::Format (hours, 2));

  if (minutes < 0)
    s.append (":00");
  else
    if (minutes < 10)
      s.append (":0" + Utils::Format (minutes, 1));
    else
      s.appned (":" + Utils::Format (minutes, 2));

  if (seconds < 0)
    s.append (":00");
  else
    if (seconds < 10)
      s.append (":0" + Utils::Format (seconds, 1));
    else
      s.append (":" + Utils::Format (seconds, 2));

  return s;
} // string OneVoter::ConvertTime(int time_in_seconds) const

/*******************************************************************************
**/
string OneVoter::ToString () {
  string s = kTag;

  s.append (Utils::Format (sequence_, 7) + ": ");
  s.append (Utils::Format (this->GetTOD (time_arrival_seconds_)) + " ");
  s.append (Utils::Format (this->GetTOD (time_start_voting_seconds_)) +" ";
  s.append (Utils::Format (this->ConvertTime(time_vote_duration_seconds_)) +" ";
  s.append (Utils::Format (this->GetTOD (time_start_voting_seconds_
                                         + time_vote_duration_seconds_)) + " ");
  s.append (Utils::Format (this->ConvertTime (GetTimeInQ ())) + ": ");
  s.append (Utils::Format (which_station_, 4));

  return s;
} // string OneVoter::toString()

/*******************************************************************************
**/
            
string OneVoter::ToStringHeader () {
  string s = kTag;
  s.append ("\tSeq\tArr\tStart\tDur\tEnd\t\tWait\tStn");
  return s;
}

