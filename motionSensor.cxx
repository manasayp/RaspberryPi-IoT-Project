#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <time.h>
#include "biss0001.h"

using namespace std;

int shouldRun = true;

void sig_handler(int signo)
{
  if (signo == SIGINT)
    shouldRun = false;
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

int main ()
{
  signal(SIGINT, sig_handler);

  // Instantiate a Grove Motion sensor on GPIO pin 16
  upm::BISS0001* motion = new upm::BISS0001(16);
  
  while (shouldRun)
  {
      bool val = motion->value();
      cout << currentDateTime() <<"," << val <<"\n";
      cout << endl;
      sleep(1);
  }

  delete motion;
  return 0;
}
