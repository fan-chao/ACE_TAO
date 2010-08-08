// -*- C++ -*-

//=============================================================================
/**
 *  @file    FTRMFF_Basic_Algorithm.cpp
 *
 *  $Id$
 *
 *  @author  Friedhelm Wolf (fwolf@dre.vanderbilt.edu)
 */
//=============================================================================

#include <sstream>
#include "FTRMFF_Basic.h"
#include "Task_Scheduler.h"
#include "Simple_Ranking.h"

FTRMFF_Basic::FTRMFF_Basic (CTT_Algorithm & ctt)
  : ctt_ (ctt)
{
}

FTRMFF_Basic::~FTRMFF_Basic ()
{
}

FTRMFF_Output
FTRMFF_Basic::operator () (const FTRMFF_Input & input)
{
  FTRMFF_Basic_Algorithm algorithm (input.processors,
                                    input.backup_count,
                                    ctt_);

  FTRMFF_Output output;
  output.schedule = algorithm (input.tasks);
  output.unscheduled_tasks = algorithm.get_unschedulable ();

  DBG_OUT (remove_empty_processors (algorithm.schedule ()));

  return output;
}

FTRMFF_Basic_Algorithm::FTRMFF_Basic_Algorithm (
  const PROCESSOR_LIST & processors,
  unsigned int consistency_level,
  CTT_Algorithm & ctt)
  : FTRMFF_Algorithm_Impl (consistency_level),
    schedule_ (create_schedule (processors)),
    ctt_ (ctt),
    ranking_algorithm_ (new Simple_Ranking ())
{
}

FTRMFF_Basic_Algorithm::~FTRMFF_Basic_Algorithm ()
{
}

SCHEDULING_MAP
FTRMFF_Basic_Algorithm::operator () (const TASK_LIST & tasks)
{
  // sort tasks based on their periods, which results in a priority
  // ordered list since we do rate monotonic scheduling
  TASK_LIST sorted_input = tasks;

  std::sort (sorted_input.begin (), 
	     sorted_input.end (), 
	     PeriodComparison <Task> ());

  for (TASK_LIST::iterator it = sorted_input.begin ();
       it != sorted_input.end ();
       ++it)
    {
      // use a copy of the original schedule since we will modify it
      SCHEDULE temp_schedule = schedule_;
      Task_Scheduler scheduler (temp_schedule,
                                ctt_);

      // create the right amount of backup replica tasks
      TASK_LIST task_group = create_tasks (*it, consistency_level_);

      // schedule the backup tasks of one application
      SCHEDULE_RESULT_LIST results;
      std::transform (task_group.begin (),
                      task_group.end (),
                      std::inserter (results,
                                     results.begin ()),
                      scheduler);
          
      // rank backups according to their wcrt
      unsigned int scheduled_backups = 
        (*ranking_algorithm_) (results,
                               schedule_);

      if (scheduled_backups < results.size ())
        {
          // could not schedule all backups
          ScheduleProgress pg = {*it, 1 + scheduled_backups};
          unschedulable_.push_back (pg);
          continue;
        }

      // if we reach this code, we can add all tasks to the schedule
      add_schedule_results (results, schedule_);
    }

  return transform_schedule (schedule_);
}

const SCHEDULE & 
FTRMFF_Basic_Algorithm::schedule () const
{
  return schedule_;
}