#include <stdlib.h>
#include <iostream>
#include <memory>
#include "Auth.h"
#include "connection-pool/MySQLConnection.h"

using boost::shared_ptr;

extern void log_err(std::string message);
extern void log_notice(std::string message);


namespace active911 {


  class AuthHandler : virtual public AuthIf {
   public:
	AuthHandler(boost::shared_ptr<ConnectionPool<MySQLConnection> >mysql_pool){

	  this->mysql_pool=mysql_pool;
	};
	~AuthHandler(){};

	void ping() {
	  // Your implementation goes here
	  //printf("ping\n");
	}

	void authenticate_device(AuthDeviceResult& _return, const std::string& username, const std::string& password){

	  try {
	  
		// Get a connection 
		shared_ptr<MySQLConnection> conn=this->mysql_pool->borrow();

		// Run a query
		conn->sql_connection->setSchema("alert");
		shared_ptr<sql::PreparedStatement>stmt(conn->sql_connection->prepareStatement("SELECT id FROM devices WHERE id=? AND device_key=?"));
		stmt->setInt(1,atoi(username.c_str()));
		stmt->setString(2,password);
		shared_ptr<sql::ResultSet>result(stmt->executeQuery());

		size_t rows=result->rowsCount();      


		// Release the connecton
		this->mysql_pool->unborrow(conn);

		// Return the response
		if(rows){

		  _return.valid=true;
		  
		} else {

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

  };


}