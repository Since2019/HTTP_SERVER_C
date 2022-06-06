Judao Zhong       A01177492 FreeBSD
Clinton Fernandes A01182058 Mac OS
Amber Ma          A01178945 FreeBSD

Usage: ./server [options]
-r --server-root <root directory for server>
-s --server-name <server name>
-l --log-file <log file name>
-n --location-404 <404 file location>
-d --default-document <default file response>
-c --max-connections <max # of server connections>
-p --port <port # to listen on>
-m --server-mode
-h --help

All arguments are optional and may be specified in several methods (in order of precedence):
command line --> configuration file (config.cfg) --> environment variables --> defaults

Environment variable list:
server root: DC_SERVER_ROOT
log filename: DC_LOG_FILE
404 filename: DC_404_FILE
server name: DC_SERVER_NAME
default document: DC_DEFAULT_DOCUMENT
maximum connections: DC_MAX_CONNECTIONS
port number: DC_PORT
server mode: DC_SERVER_MODE

Other notes:
- Server mode is an integer value, 1 = threads, 2 = processes
- Server root directory must end in "/"

Demo:
Inside "./Demo" folder, there are three executables
- server_linux
- server_mac
- server_bsd

Run with sudo/root access to the system, because the bind() function opens up network ports.