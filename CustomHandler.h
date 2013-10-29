#include <stdlib.h>
#include <iostream>
#include <memory>
#include <string>
#include "Interface.h"
#include "connection-pool/MySQLConnection.h"
#include "fastcache/Fastcache.h"


using boost::shared_ptr;

extern void log_err(std::string message);
extern void log_notice(std::string message);


namespace active911 {


class InterfaceHandler : virtual public InterfaceIf {

public:
	InterfaceHandler(boost::shared_ptr<ConnectionPool<MySQLConnection> > mysql_pool) {

  
	  this->mysql_pool=mysql_pool;
	  this->cache=shared_ptr<Fastcache<string, int> >(new Fastcache<string, int>());
	};


  void authenticate_device(AuthDeviceResult& _return, const std::string& username, const std::string& password) {

	  try {
	  
	  	// Check the cache (in real life, we would want a better key or to do some validity checks!)
	  	shared_ptr<int>cache_result=this->cache->get(username+password);
	  	if(cache_result) {

	  		std::cout << "Cache hit" << std::endl;
	  		_return.valid=*cache_result;
	  		return;
	  	}

  		std::cout << "Cache MISS" << std::endl;


		// Get a connection 
		shared_ptr<MySQLConnection> conn=this->mysql_pool->borrow();

		// Run a query
		//conn->sql_connection->setSchema("alert");
		shared_ptr<sql::PreparedStatement>stmt(conn->sql_connection->prepareStatement("SELECT 1"));
		//stmt->setInt(1,atoi(username.c_str()));
		//stmt->setString(2,password);
		shared_ptr<sql::ResultSet>result(stmt->executeQuery());

		size_t rows=result->rowsCount();      


		// Release the connecton
		this->mysql_pool->unborrow(conn);

		// Return the response
		if(rows){

		this->cache->set(username+password,shared_ptr<int>(new int(1)));
		  _return.valid=true;
		  
		} else {

		this->cache->set(username+password,shared_ptr<int>(new int(0)));
		  _return.valid=false;
		}

	  } catch (std::exception& e) {

		ServiceUnavailable s;
		s.error_message="Can't perform query against database: "+std::string(e.what());

		throw s;
	  }

	}


  private:

	boost::shared_ptr<ConnectionPool<MySQLConnection> > mysql_pool;
	boost::shared_ptr<Fastcache<string, int> > cache;


  };


}