#include "daemon.hpp"

void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGHUP:
        Daemon::get_instance().got_sighup = 1;
        break;
    case SIGTERM:
        Daemon::get_instance().got_sigterm = 1;
        break;
    }
}

void Daemon::create_pid_file()
{
    int pid_file_handle = open(pid_file.c_str(), O_RDWR | O_CREAT, 0600);
    if (pid_file_handle == -1)
    {
        syslog(LOG_ERR, "PID file %s cannot be opened", pid_file.c_str());
        exit(EXIT_FAILURE);
    }

    if (lockf(pid_file_handle, F_TLOCK, 0) == -1)
    {
        syslog(LOG_ERR, "Daemon is already running (PID file is locked)");
        exit(EXIT_FAILURE);
    }

    char old_pid_str[10];
    if (read(pid_file_handle, old_pid_str, sizeof(old_pid_str) - 1) > 0)
    {
        int old_pid = atoi(old_pid_str);

        if (old_pid > 0 && kill(old_pid, 0) == 0)
        {
            syslog(LOG_INFO, "Process with PID %d is already running, sending SIGTERM", old_pid);
            kill(old_pid, SIGTERM);
            sleep(1);
        }
        else
        {
            syslog(LOG_INFO, "No process found with PID %d, continuing...", old_pid);
        }
    }

    ftruncate(pid_file_handle, 0);
    lseek(pid_file_handle, 0, SEEK_SET);

    char str[10];
    snprintf(str, sizeof(str), "%d\n", getpid());
    write(pid_file_handle, str, strlen(str));

    syslog(LOG_INFO, "PID file %s created successfully with PID %d", pid_file.c_str(), getpid());

    close(pid_file_handle);
}

void Daemon::daemonize(const std::filesystem::path &pid_folder)
{
    pid_t pid, sid;
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);
    umask(0);
    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);
    if ((chdir("/")) < 0)
        exit(EXIT_FAILURE);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    pid_file = pid_folder / std::to_string(pid);
    create_pid_file();
}