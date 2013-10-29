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

- Apache Thrift
- Boost and Boost Thread (libboost-dev, libboost-thread-dev)
- MySQL "Connection/C++" lib

