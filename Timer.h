#ifndef TIMER_H
#define TIMER_h

class Timer{
  public:
    unsigned long time_set = 0;

    Timer();
    Timer(int time_to_set);
    bool isOver();
    void setSeconds(int seconds);
    void setMinutes(int minutes);
    void start();
    //void stop();

    long elapsedTime();
    int elapsedSeconds();
    long remainingTime();
    int remainingSeconds();

  private:
    unsigned long start_time;
  
};

#endif
