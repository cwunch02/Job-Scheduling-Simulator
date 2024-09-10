#include <deque>
#include <vector>
#include <queue>
#include "task.h"
#include "sched.h"
#include "events.h"

using namespace std;

void MLFQScheduler::sched(TaskSet &taskSet, TaskSet &departures) {
  deque<Task> highPriorityQueue;
  deque<Task> lowPriorityQueue;

  Time current_time = 0;
  Task task;
  vector<Event> traces;
  int leftoverTime;

  while(true){

    if(highPriorityQueue.empty() && lowPriorityQueue.empty()){
      if (taskSet.next(task)) {
          task.remaining_time = task.service_time;
          current_time = task.arrival_time;
          highPriorityQueue.push_back(task);
          traces.push_back(Event{task.arrival_time, task.task_id, 'Q', 'H'});
      }
    }

    while(taskSet.nextArrivedBefore(task, current_time)){
      task.remaining_time = task.service_time;
      highPriorityQueue.push_back(task);
      traces.push_back(Event{task.arrival_time, task.task_id, 'Q', 'H'});
    }
    
    if(!highPriorityQueue.empty()){

      traces.push_back(Event{current_time, highPriorityQueue.front().task_id, 'R', 'H'});

      Time timeToRun = min(highPriorityQueue.front().remaining_time, h_quantum);

      current_time += timeToRun;
      highPriorityQueue.front().remaining_time -= timeToRun;

      while(taskSet.nextArrivedBefore(task, current_time)){
        task.remaining_time = task.service_time;
        highPriorityQueue.push_back(task);
        traces.push_back(Event{highPriorityQueue.back().arrival_time, highPriorityQueue.back().task_id, 'Q', 'H'});
      }
      
      if(highPriorityQueue.front().remaining_time <= 0){
        traces.push_back(Event{current_time, highPriorityQueue.front().task_id, 'X', 'H'});
        highPriorityQueue.front().departure_time = current_time;
        departures.append(highPriorityQueue.front());
        highPriorityQueue.pop_front();
      } else {
        lowPriorityQueue.push_back(highPriorityQueue.front());
	traces.push_back(Event{current_time, lowPriorityQueue.front().task_id, 'Q', 'L'});
        highPriorityQueue.pop_front();
        traces.push_back(Event{current_time, highPriorityQueue.back().task_id, 'E', 'H'});
      }
    
    } else if(!lowPriorityQueue.empty()){
   
      traces.push_back(Event{current_time, lowPriorityQueue.front().task_id, 'R', 'L'});

      Time timeToRun = l_quantum;

      if(taskSet.nextArrivedBefore(task, current_time + timeToRun)){
        Time usedTime = task.arrival_time - current_time;
	task.remaining_time = task.service_time;
	current_time += usedTime;
	lowPriorityQueue.front().remaining_time -= usedTime;
	leftoverTime = timeToRun - usedTime;
	traces.push_back(Event{current_time, lowPriorityQueue.front().task_id, 'P', 'L'});

	highPriorityQueue.push_back(task);
	traces.push_back(Event{task.arrival_time, task.task_id, 'Q', 'H'});
	continue;
      }

      if(leftoverTime > 0){
	timeToRun = leftoverTime;
	current_time += timeToRun;
	leftoverTime = 0;
      } else {
	current_time += timeToRun;
      }
      
      if(lowPriorityQueue.front().remaining_time - timeToRun > lowPriorityQueue.front().service_time){
	current_time -= (timeToRun - lowPriorityQueue.front().remaining_time);
	lowPriorityQueue.front().remaining_time = 0;
      } else {
	lowPriorityQueue.front().remaining_time -= timeToRun;
      }
      
      if(lowPriorityQueue.front().remaining_time <= 0){
        traces.push_back(Event{current_time, lowPriorityQueue.front().task_id, 'X', 'L'});
        lowPriorityQueue.front().departure_time = current_time;
        departures.append(lowPriorityQueue.front());
        lowPriorityQueue.pop_front();
      } else {
        lowPriorityQueue.push_back(lowPriorityQueue.front());
        lowPriorityQueue.pop_front();
        traces.push_back(Event{current_time, lowPriorityQueue.back().task_id, 'P', 'L'});
      }
      
    }

    if(taskSet.empty() && highPriorityQueue.empty() && lowPriorityQueue.empty())
      break;
  }
  print_traces(traces);
  departures.print();
}
