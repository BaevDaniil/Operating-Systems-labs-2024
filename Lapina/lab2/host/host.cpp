#include "host.h"

voia main()
{

}


Host::Host()
{
    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = SignalHandler;
    sig.sa_flags = SA_SIGINFO;

    if (sigaction(SIGTERM, &sig, nullptr)==-1)
    {
        throw std::runtime_error("Failed to register SIGTERM");
    }
    if (sigaction(SIGUSR1, &sig, nullptr)==-1)
    {
        throw std::runtime_error("Failed to register SIGUSR1");
    }

    openlog("lab2_host", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);

    syslog("Host pid = %d", int(getpid()));

}

void Host::SignalHandler(int signum, siginfo_t *si, void *data) {
  switch (signum) {
  case SIGTERM:
      Terminate();
      break;
  case SIGUSR1:
      goatHandler goatH = goatHandler();
      bool initStatus = goatH.initGoat(si->si_pid);
      if (initSatus)
      {
        goats.set(si->si_pid, goatH);
      }
      break;
  default:
    break;
  }
}

void Host::Terminate()
{
    if (!hostTerminated.load())
    {
        hostTerminated=true;
    }

    //how kill SIGTERM to all client who not closed

    syslog(LOG_INFO, "Terminating host");

}

void Host::wolfGame()
{
    
}