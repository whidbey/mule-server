

#include "Auth.h"
#include "AuthHandler.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "INIReader.h"
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


//#define DEBUG_NEW new(__FILE__, __LINE__)
//#define new DEBUG_NEW

// Namespaces...
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;
using boost::shared_ptr;
using namespace active911;
using namespace std;

// Functions
void thrift_logger(const char* str);
void writelog(int level, string message) ;
void closeup (int retval);
void fatal(string message);
void log_err(string message);
void log_notice(string message);
void signal_handler(int signum);	// Catch SIGHUP, etc


// Globals
shared_ptr<TNonblockingServer> nb_server;
bool daemonized=false;


int main(int argc, char **argv) {

	// Read the config file
	INIReader reader("gua.cfg");
	if (reader.ParseError() < 0) {
		fatal("Can't read gua.cfg");
	}

	// Set config parameters
	int ip_port = reader.GetInteger("authd","ip-port",9090);
	int worker_threads = reader.GetInteger("authd","worker-threads",16);
	string user= reader.Get("authd","user","authd");
	string group= reader.Get("authd","group","authd");
	string pid_file_name= reader.Get("authd","pidfile","/var/run/authd.pid");
	string mysql_username=reader.Get("mysql","username","root");
	string mysql_password=reader.Get("mysql","password","");
	string mysql_server=reader.Get("mysql","server","tcp://127.0.0.1:3306");
	int mysql_poolsize = reader.GetInteger("mysql","poolsize",5);

	// Daemonize and setup logging 
	if(argc==2 && 0==strcmp(argv[1],"-d")) { 

		// -d set.	Don't daemonize
		writelog(LOG_NOTICE,"-d set.	Not daemonizing.");

	} else {

		if(daemon(0,0)!=0) {

			fatal("Error daemonizing! Exiting...");
		}
		daemonized=true;
		openlog("authd",0,LOG_AUTH);
	}

	// Install signal handlers
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);


	// Write PID.	 Do this before dropping root in case dir is not writeable by daemon user.
	ofstream pidfile;
	pidfile.open(pid_file_name.c_str());
	pidfile << getpid();
	pidfile.close();
	if(pidfile.fail()){

		fatal("Unable to write pid to \""+pid_file_name+"\"");
	}

	// Drop root
	if (getuid() == 0) {

		// Get uid and gid info 
		struct passwd* p=getpwnam(user.c_str());		// according to getpwnam(3), this should not be free()d
		struct group* g=getgrnam(group.c_str());

		if (p==NULL || g==NULL || setgid(g->gr_gid) != 0 || setuid(p->pw_uid) != 0) {

			fatal("Unable to change user / group to "+user+"/"+group);
		}
	}


	try {

		// Create the connection factory and connection pool
		shared_ptr<MySQLConnectionFactory>mysql_connection_factory(new MySQLConnectionFactory(mysql_server,mysql_username,mysql_password));
		shared_ptr<ConnectionPool<MySQLConnection> >mysql_pool(new ConnectionPool<MySQLConnection>(mysql_poolsize, mysql_connection_factory));

		shared_ptr<AuthHandler> handler(new AuthHandler(mysql_pool));
		shared_ptr<TProcessor> processor(new AuthProcessor(handler));
		shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
		shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(worker_threads);
		shared_ptr<PosixThreadFactory> threadFactory = shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
		threadManager->threadFactory(threadFactory);
		threadManager->start();
		nb_server=shared_ptr<TNonblockingServer>(new TNonblockingServer(processor, protocolFactory, ip_port, threadManager));
		GlobalOutput.setOutputFunction(thrift_logger);		 // Redirect logging output to our function

	} catch (exception& e){

		fatal(e.what());
	}

	// Go!
	stringstream ss; 
	ss << "Starting server" << endl;
	ss << "    pid            : " << getpid() << endl; 
	ss << "    worker threads : " << worker_threads << endl;
	ss << "    mysql poolsize : " << mysql_poolsize << endl;
	ss << endl;
	writelog(LOG_NOTICE, ss.str());
	nb_server->serve();


	closeup(0);
	return 0;
}



void fatal(string message) {

	writelog(LOG_ERR, "fatal: " + message);
	closeup(1);
}

void log_notice(string message) {

	writelog(LOG_NOTICE,message);

}

void log_err(string message) {

	writelog(LOG_ERR,message);

}

void writelog(int level, string message) {

	// If we are a daemon, syslog it
	if(daemonized){

		syslog(level, message.c_str());

	} else {

		switch(level){
			case LOG_ERR:
				cout << "error: ";
				break;
			case LOG_WARNING:
				cout << "warning: ";
				break;
		}

		cout << message << endl;
	}
}

void closeup (int retval) {

	// Stop the server
	if(nb_server) {

		nb_server->stop();
		nb_server.reset(); // Release the object, since it was global
	}

	if(daemonized){

		closelog();
	}

	exit(retval);
}

void signal_handler(int signum){

	if(signum==SIGTERM || signum==SIGINT){

		writelog(LOG_NOTICE,string("caught signal. Closing..."));
	// Stop the server
		if(nb_server) {

			nb_server->stop();
			nb_server.reset(); // Release the object, since it was global
		} 
	 }

}


void thrift_logger (const char* str) {	

	writelog(LOG_NOTICE,string(str)); 
}


