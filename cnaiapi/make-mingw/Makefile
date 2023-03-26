.c.o:
	gcc -c -I. -o $@ $<

all: client.exe server.exe chatclient.exe chatserver.exe webclient.exe webserver.exe

client.exe: client.o
	gcc -o client client.o -l wsock32

server.exe: server.o
	gcc -o server server.o -l wsock32

chatclient.exe: chatclient.o cnaiapi_init.o cname_to_comp.o readln.o send_eof.o make_contact.o
	gcc -o chatclient chatclient.o cnaiapi_init.o cname_to_comp.o \
        readln.o send_eof.o make_contact.o -l wsock32

chatserver.exe: chatserver.o cnaiapi_init.o cname_to_comp.o readln.o send_eof.o await_contact.o
	gcc -o chatserver chatserver.o cnaiapi_init.o cname_to_comp.o \
        readln.o send_eof.o await_contact.o -l wsock32

webclient.exe: webclient.o cnaiapi_init.o cname_to_comp.o readln.o send_eof.o \
        appname_to_appnum.o make_contact.o
	gcc -o webclient webclient.o cnaiapi_init.o cname_to_comp.o appname_to_appnum.o\
        readln.o send_eof.o make_contact.o -l wsock32

webserver.exe: webserver.o cnaiapi_init.o cname_to_comp.o readln.o send_eof.o await_contact.o
	gcc -o webserver webserver.o cnaiapi_init.o cname_to_comp.o \
        readln.o send_eof.o await_contact.o -l wsock32