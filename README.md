mule-server
===========

Fast, generic service using Apache Thrift and Active911 fastcache + connection-pool

### Introduction

Imagine a LAMP + memcached service stack.  But...

- Apache and PHP have been replaced with a purpose-built, persistent C++ daemon
- MySQL is running with persistent, self-managing connections
- Memcached is instantaneous
- Apache Thrift is managing all your interface needs

All you really have to do to create your own awesome daemon is edit interface.thrift file and CustomHandler.h.

## Dependencies

- Apache Thrift. I compiled from scratch and then ran ldconfig. Thrift requires Bit::Vector (libbit-vector-perl) in order to get PERL bindings but neglects to mention it
- MySQL Connector/C++ (the debian package is called libmysqlcppconn-dev)
- Boost and Boost Thread (libboost-dev, libboost-thread-dev)

## Installation

- Install typical build packages, e.g., 'apt-get build-essential'.  
- Install thrift.  I found it helpful to ./configure --without-ruby .  Make sure you have C++ and PERL bindings at least.  Thrift lists the Deb packages it needs... add libbit-vector-perl to the list
- Install the MySQL Connector/C++ dev lib
- Run make 
- Copy muled.cfg.sample to muled.cfg and tweak as necessary
- Run ./muled -d to test, or ./muled to daemonize.  Daemon logs to syslog LOG_DAEMON facility.
- In a different window, run the PERL example_client.pl program to connect and test


