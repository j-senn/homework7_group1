/*******************************************************************************
 * Header for the 'OneVoter' class
 *
 * Author/copyright:  Duncan Buell
 * Date: 6 October 2016
 *
 * Used with permission by: Group 1 (BRYCE ANDREW BLANTON,
 * WILLIAM R EDWARDS, JONATHAN SENN, SPENCER C SMITH,
 * CELENA CHRISTINE WILLIAMS)
 * Date last modified: 1 December 2016
 * 
 * This class is used for the creation of an individual voter so that the
 * respective times for a voter can be outtputted to check for anomalies in 
 * data.
 *
**/

#ifndef ONEVOTER_H
#define ONEVOTER_H

#include "../Utilities/utils.h"

using namespace std;

static int kDummyVoterInt = -333;

class OneVoter {
public:
 
/*******************************************************************************
 * Constructors and destructors for the class. 
**/
 
 OneVoter();
 OneVoter(int sequence, int arrival_seconds, int duration_seconds);
 virtual ~OneVoter();

/*******************************************************************************
 * Accessors and Mutators.
**/
 
 int GetStationNumber() const;
 int GetTimeArrival() const;
 int GetTimeDoneVoting() const;
 int GetTimeWaiting() const;

/*******************************************************************************
 * General functions.
**/
 
 void AssignStation(int station_number, int start_time_seconds);
 void DoneVoting();
 int GetTimeInQ() const;

 string ToString();
 static string ToStringHeader();

private:
 int sequence_ = kDummyVoterInt;
 int time_arrival_seconds_ = kDummyVoterInt;
 int time_done_voting_seconds_ = kDummyVoterInt;
 int time_start_voting_seconds_ = kDummyVoterInt;
 int time_vote_duration_seconds_ = kDummyVoterInt;
 int time_waiting_seconds_ = kDummyVoterInt;
 int which_station_ = kDummyVoterInt;

/*******************************************************************************
 * General private functions.
**/
 
 string ConvertTime(int time_in_seconds) const;
 string GetTOD(int time) const;
};

#endif // ONEVOTER_H
