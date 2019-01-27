#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <dirent.h>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/signal.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */

#include "websocket.h"

using namespace std;

webSocket server;

/* called when a client connects */
void openHandler(size_t clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has joined.";

    vector<size_t> clientIDs = server.getClientIDs();
    for (size_t i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
    server.wsSend(clientID, "Welcome!");
}

/* called when a client disconnects */
void closeHandler(size_t clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has leaved.";

    vector<size_t> clientIDs = server.getClientIDs();
    for (size_t i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
}

/* called when a client sends a message to the server */
void messageHandler(size_t clientID, string message){
    ostringstream os;
    os << "Stranger " << clientID << " says: " << message;

    vector<size_t> clientIDs = server.getClientIDs();
    for (size_t i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
}

/* called once per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next){
        ostringstream os;
        string timestring = ctime(&current);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<size_t> clientIDs = server.getClientIDs();
        for (size_t i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());

        next = time(NULL) + 10;
    }
}

struct app_args {
    bool daemon = false;
    int port = 8888;
    string state = "start";
    string pidfile = "chatroom.pid";
    string logfile = "chatroom.log";
};

app_args args;

void welcome();
void usage(int, char **);
void parse_args(int, char **);
void init(int, char **);
void remove_pidfile();
void kill_process();
int daemonize(const char *);
void write_pid();
int read_pid();
void check_pidfile();
int file_put_contents(const std::string &, const std::string &);
int file_get_contents(const std::string &, std::string *);
bool file_exists(const std::string &);
bool is_dir(const std::string &);
bool is_file(const std::string &);

int main(int argc, char **argv){
    welcome();
    usage(argc, argv);
    parse_args(argc, argv);
    init(argc, argv);
    {
        /* set event handler */
        server.setOpenHandler(openHandler);
        server.setCloseHandler(closeHandler);
        server.setMessageHandler(messageHandler);
        //server.setPeriodicHandler(periodicHandler);

        /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
        server.startServer(args.port);
    }
    write_pid();
    remove_pidfile();
    //delete server;
    return 1;
}
void welcome() {
    cout << "Chatroom v1.0" << endl;
    cout << "Copyright (c) 2019" << endl;
}

void usage(int argc, char **argv) {
    cout << "Usage: " << argv[0] << " -p port [-d] [-s start|stop|restart]" << endl;
    cout << "Options:" << endl;
    cout << "  -p --port\tport number" << endl;
    cout << "  -d --daemon\trun as daemon" << endl;
    cout << "  -s --state\toption to start|stop|restart server" << endl;
    cout << "  -h --help\tshow help" << endl;
}

void parse_args(int argc, char **argv) {
    for(int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-p" || arg == "--port") {
            if (i + 1 <  argc) {
                i++;
                args.port = atoi(argv[i]);
            } else {
                usage(argc, argv);
                exit(0);
            }
        } else if (arg == "-d" || arg == "--daemon") {
            args.daemon = true;
        } else if (arg == "-s" || arg == "--state") {
            if (i + 1 > argc) {
                i++;
                args.state = argv[i];
            } else {
                usage(argc, argv);
                exit(0);
            }
            if (args.state != "start" && args.state != "stop" && args.state != "restart") {
                usage(argc, argv);
                exit(0);
            }
        } else if (arg == "-h" || arg == "--help") {
            usage(argc, argv);
            exit(0);
        }
    }
}

void init(int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    if (args.state == "stop") {
        kill_process();
        exit(0);
    }
    if(args.state == "restart") {
        kill_process();
    }
    check_pidfile();
    if (args.daemon) {
        daemonize(NULL);
    }
}

bool file_exists(const std::string &filename){
	struct stat st;
	return stat(filename.c_str(), &st) == 0;
}
bool is_dir(const std::string &filename){
	struct stat st;
	if(stat(filename.c_str(), &st) == -1){
		return false;
	}
	return (bool)S_ISDIR(st.st_mode);
}
bool is_file(const std::string &filename){
	struct stat st;
	if(stat(filename.c_str(), &st) == -1){
		return false;
	}
	return (bool)S_ISREG(st.st_mode);
}

int file_get_contents(const std::string &filename, std::string *content){
	char buf[8192];
	FILE *fp = fopen(filename.c_str(), "rb");
	if(!fp){
		return -1;
	}
	int ret = 0;
	while(!feof(fp) && !ferror(fp)){
		int n = fread(buf, 1, sizeof(buf), fp);
		if(n > 0){
			ret += n;
			content->append(buf, n);
		}
	}
	fclose(fp);
	return ret;
}

int file_put_contents(const std::string &filename, const std::string &content){
	FILE *fp = fopen(filename.c_str(), "wb");
	if(!fp){
		return -1;
	}
	int ret = fwrite(content.data(), 1, content.size(), fp);
	fclose(fp);
	return ret == (int)content.size()? ret : -1;
}

void check_pidfile(){
	if(args.pidfile.size()){
		if(access(args.pidfile.c_str(), F_OK) == 0){
			cout << "Fatal error!\nPidfile " << args.pidfile.c_str() << " already exists!\nKill the running process before you run this command,\nor use '-s restart' option to restart the server." << endl;
			exit(1);
		}
	}
}

int read_pid(){
	if(args.pidfile.empty()){
		return -1;
	}
	string s;
	file_get_contents(args.pidfile, &s);
	if(s.empty()){
		return -1;
	}
	return atoi(s.c_str());
}

void write_pid(){
	if(args.pidfile.empty()){
		return;
	}
	int pid = (int)getpid();
	string s = to_string(pid);
	//("pidfile: %s, pid: %d", args.pidfile.c_str(), pid);
	int ret = file_put_contents(args.pidfile, s);
	if(ret == -1){
		//log_error("Failed to write pidfile '%s'(%s)", args.pidfile.c_str(), strerror(errno));
		exit(1);
	}
}

void remove_pidfile(){
	if(args.pidfile.size()){
		remove(args.pidfile.c_str());
	}
}

void kill_process() {
    int pid = read_pid();
	if(pid == -1){
		cout << "could not read pidfile: " << args.pidfile << "(" << strerror(errno) << ")" << endl;
		exit(1);
	}
	if(kill(pid, 0) == -1 && errno == ESRCH){
		cout << "process: " << pid << " not running" << endl;
		remove_pidfile();
		return;
	}
	int ret = kill(pid, SIGTERM);
	if(ret == -1){
		cout << "could not kill process: " << pid << "(" << strerror(errno) << ")" << endl;
		exit(1);
	}
	
	while(file_exists(args.pidfile)){
		usleep(100 * 1000);
	}
}

int daemonize(const char *dir = NULL) {
    switch(fork()){
		case -1: return -1;
		case 0: break;
		default: exit(0);
	}
	if(setsid() == -1){
		exit(0);
	}
	if(dir != NULL){
		if(chdir(dir) == -1){
			exit(0);
		}
	}

	if(close(STDIN_FILENO) == -1 || close(STDOUT_FILENO) == -1 || close(STDERR_FILENO) == -1){
		exit(0);
	}

	int fd = open("/dev/null", O_RDWR, 0);
	if(fd == -1){
		exit(0);
	}
	if(dup2(fd, STDIN_FILENO) == -1 || dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1){
		exit(0);
	}
    return 0;
}