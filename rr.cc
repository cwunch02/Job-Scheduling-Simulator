#include <iostream>
#include <deque>
#include <vector>
#include <queue>
#include "task.h"
#include "sched.h"
#include "events.h"

using namespace std;

void RoundRobinScheduler::sched(TaskSet &taskSet, TaskSet &departures) {
  deque<Task> queue;
  Time current_time = 0;
  Task task;
  vector<Event> traces;

  while(true){
    if(queue.empty()){
      if(taskSet.next(task)){
	task.remaining_time = task.service_time;
	current_time = task.arrival_time;
	queue.push_back(task);
	traces.push_back(Event{task.arrival_time, task.task_id, 'Q', '-'});
      }
    }

    while(taskSet.nextArrivedBefore(task, current_time)){
      task.remaining_time = task.service_time;
      queue.push_back(task);
      traces.push_back(Event{task.arrival_time, task.task_id, 'Q', '-'});
    }
    
    if(!queue.empty()){

      if(queue.front().arrival_time > current_time)
	current_time = queue.front().arrival_time;
      
      traces.push_back(Event{current_time, queue.front().task_id, 'R', '-'});

      Time timeToRun = min(queue.front().remaining_time, quantum);

      if(taskSet.nextArrivedBefore(task, current_time + timeToRun)){
	task.remaining_time = task.service_time;
	queue.push_back(task);
	traces.push_back(Event{queue.back().arrival_time, queue.back().task_id, 'Q', '-'});
      }
      
      current_time += timeToRun;
      queue.front().remaining_time -= timeToRun;

      if(queue.front().remaining_time <= 0){
	traces.push_back(Event{current_time, queue.front().task_id, 'X', '-'});
	queue.front().departure_time = current_time;
	departures.append(queue.front());
	queue.pop_front();
      } else {
	queue.push_back(queue.front());
	queue.pop_front();
	traces.push_back(Event{current_time, queue.back().task_id, 'Q', '-'});
      }

      continue;
      
    }
    if(taskSet.empty())
      break;
  }
  print_traces(traces);
  departures.print();
}
