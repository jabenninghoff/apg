#!/usr/bin/perl -w
$host = "localhost";
use IO::Socket;
$remote = IO::Socket::INET->new(
    Proto => "tcp",
    PeerAddr => $host,
    PeerPort => "pwdgen(129)",
    )
    or die "cannot connect to pwdgen port at $host";
while ( <$remote> ) { print }
