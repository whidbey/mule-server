#!/usr/bin/perl


use strict;
use warnings;

use lib './gen-perl';
use lib './lib';


use Thrift;
use Thrift::BinaryProtocol;
use Thrift::Socket;
use Thrift::FramedTransport;
use active911::Interface;
use Data::Dumper;

my $socket=new Thrift::Socket('localhost',9090);
my $transport=new Thrift::FramedTransport($socket);
my $protocol=new Thrift::BinaryProtocol($transport);
my $client=new active911::InterfaceClient($protocol);

eval {

	$transport->open();
	my $response=$client->authenticate_device("example_username","example_password");
	#print Dumper($response);
	$transport->close();
	if($response->{'valid'}==1){

		print("Valid\n");
	} else {

		print("Invalid\n");

	}
	1;

} or do {


	die(Dumper(\$@));

};

