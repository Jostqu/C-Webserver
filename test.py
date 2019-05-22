#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# This script makes a HTTP request to a specific server and compares
# responses with user-defined expected responses.
from __future__ import unicode_literals

import sys

from pewpewlaz0rt4nk import (
    Beam,
    Laz0rCannon,
)

__version__ = '2.0.0'

# Set host and port.
# Read the target IP address and port from the command line arguments.
# If there are no command line arguments, use the following default values.
host = 'localhost'
port = 31337

# Overwrite host and port (if specified)
if len(sys.argv) == 3:
    _, host, port = sys.argv
    port = int(port)

# Initialise Laz0rCannon
cannon = Laz0rCannon(host=host, port=port)

# Laz0r Beams (Tests to fire against the server).
cannon += Beam(
    description='Datei "/debug"',
    request='GET /debug HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='außerhalb des DocumentRoot',
    request='GET /..//..//..///..//.. HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 403']
)
cannon += Beam(
    description='Datei "/js/javascript.js"',
    request='GET /js/javascript.js HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Datei "/tux"',
    request='GET /images/tux HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Ungültige Datei "/debug."',
    request='GET /debug. HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 404']
)
cannon += Beam(
    description='Ungültige Methode "GETS"',
    request='GETS / HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 501']
)
cannon += Beam(
    description='Datei "/images/ein%20leerzeichen.png"',
    request='GET /images/ein%20leerzeichen.png HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Host test "intern"',
    request='GET / HTTP/1.1\r\nHost: intern\r\n\r\n',
    response=['HTTP/1.1 401']
)
cannon += Beam(
    description='Host test "extern"',
    request='GET / HTTP/1.1\r\nHost: extern\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Datei "/tux.png"',
    request='GET /images/tux.png HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Ungültige Methode "PUT"',
    request='PUT / HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 405']
)
cannon += Beam(
    description='Ungültiges Protokoll "0.0"',
    request='GET / HTTP/0.0\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 505']
)
cannon += Beam(
    description='Ungültiger Pfad "TEST"',
    request='GET "TEST" HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 404']
)
cannon += Beam(
    description='Gültige Methode "GET"',
    request='GET / HTTP/1.1\r\nHost: extern\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Punktnotation 1',
    request='GET /./ HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Punktnotation 2',
    request='GET /. HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Request ohne Host',
    request='GET / HTTP/1.1\r\n\r\n',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='Ohne CR',
    request='GET / HTTP/1.1\nHost: {host}',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='Host Groß- und Kleinschreibung',
    request='GET / HTTP/1.1\r\nhOsT: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Host Leerzeichen',
    request='GET / HTTP/1.1\r\nHost:  {host}  \r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='Statt Host nur : CR',
    request='GET /HTTP/1.1\r\n: \r',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='Invalid folder "/sonderzeichen!"§$%&/()=?"',
    request='GET /sonderzeichen!"§$%&/()=? HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 404']
)
cannon += Beam(
    description='HTTP Version "0.9"',
    request='GET / HTTP/0.9\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='HTTP Version "1.0"',
    request='GET / HTTP/1.0\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='HTTP Version "1.1"',
    request='GET / HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 200']
)
cannon += Beam(
    description='HTTP Version "2.0"',
    request='GET / HTTP/2.0\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 505']
)
cannon += Beam(
    description='Auth required mit Spaces und CS-Test',
    request='GET /index.html HTTP/1.1\r\nHoSt:  intern  \r\n\r\n',
    response=['HTTP/1.1 401']
)
cannon += Beam(
    description='Host mit Space davor',
    request='GET / HTTP/1.1\r\n Host: {host}\r\n\r\n',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='Request that contains \'GET GET GET\'',
    request='GET GET GET',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='Request that only contains a space',
    request=' ',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='Request ohne resource',
    request='GET HTTP/1.0',
    response=['HTTP/1.1 400']
)
cannon += Beam(
    description='DocumentRoot Ausbruch Host',
    request='GET /../forbidden.txt HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 403']
)
cannon += Beam(
    description='CMD Injection',
    request='GET %2F%20%26%20chmod%20%2Bs%20%2Fsbin%2Freboot%20%26%20reboot HTTP/1.1\r\nHost: {host}\r\n\r\n',
    response=['HTTP/1.1 404']
)



# Pew pew!
cannon.pewpew()

