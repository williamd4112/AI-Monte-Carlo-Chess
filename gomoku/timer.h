#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chrono>
#include <ctime>

namespace mcts
{
class Timer
{
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::milliseconds Units;
public:
  Clock::time_point start_time;
  Units duration;
  unsigned iteration_count;

  Units max_duration;
  unsigned max_iteration_count;

  Timer(unsigned max_duration, unsigned max_iteration_count):
    max_duration(Units(max_duration)),
    max_iteration_count(max_iteration_count)
  {
    reset();
  }

  long long get_duration() const
  {
    return duration.count();
  }

  void reset()
  {
    iteration_count = 0;
  }

  void init()
  {
    start_time = Clock::now();
  }

  void start_loop()
  {
    iteration_count += 1;
  }

  bool check_resource_limit()
  {
    Clock::time_point end_time = Clock::now();
    duration = std::chrono::duration_cast<Units>(end_time - start_time);
    return (duration > max_duration) ||
           (iteration_count > max_iteration_count);
  }
};
}

#endif // TIMER_H_INCLUDED
