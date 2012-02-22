#ifndef MADOKA_TIMER_H
#define MADOKA_TIMER_H

#include <time.h>

#ifdef WIN32
 #ifndef _MSC_VER
  #include <sys/types.h>
 #endif  // _MSC_VER
 #include <sys/timeb.h>
#else  // WIN32
 #include <unistd.h>
 #ifdef _POSIX_TIMERS
  #include <sys/time.h>
 #endif  // _POSIX_TIMERS
#endif  // WIN32

namespace madoka {

class Timer {
 public:
  Timer() throw() : last_(get_time()) {}
  ~Timer() throw() {}

  double elapsed() const throw() {
    return get_time() - last_;
  }

  void reset() throw() {
    last_ = get_time();
  }

 private:
  double last_;

  static double get_time() throw() {
#ifdef WIN32
 #ifdef _MSC_VER
    struct _timeb tb;
    ::_ftime_s(&tb);
    return tb.time + (tb.millitm * 0.001);
 #else  // _MSC_VER
    struct _timeb tb;
    ::_ftime(&tb);
    return tb.time + (tb.millitm * 0.001);
 #endif  // _MSC_VER
#else  // WIN32
 #ifdef _POSIX_TIMERS
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + (ts.tv_nsec * 0.000000001);
 #else  // _POSIX_TIMERS
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec * 0.000001);
 #endif  // _POSIX_TIMERS
#endif  // WIN32
  }

  // Disallows copy and assignment.
  Timer(const Timer &);
  Timer &operator=(const Timer &);
};

}  // namespace

#endif  // MADOKA_TIMER_H
