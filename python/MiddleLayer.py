#!/usr/bin/env python

# https://gist.github.com/bradmontgomery/2219997

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
#import SocketServer
import subprocess
import sys, os
from IndexSetup import StreamAudioIndex

index = 0

class ServerException(Exception):
    pass

class S(BaseHTTPRequestHandler):
    Error_Page="""\
    <html>
    <body>
    <h1>Error accessing {path}</h1>
    <p>{msg}</p>
    </body>
    </html>
    """

    path = '/test.html'

    def do_POST(self):
        global index
        # read input
        content_len = int(self.headers['Content-Length'])
        print content_len
        data = self.rfile.read(content_len)
        result = index.search(data)
        # print str()
        # send to client
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        # self.send_header('Content-Length', str(result))
        # self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(result)

    def do_GET(self):
        try:
            print 'get'
            full_path=os.getcwd()+self.path
            print full_path
            print os.getcwd()

            if not os.path.exists(full_path):
                raise ServerException("'{ 0 }'  not found ",format(self.path))
            elif os.path.isfile(full_path):
                self.handle_file(full_path)
            else:
                raise ServerException("unkown object '{ 0 }'",format(self.path))

        except Exception as msg:
            self.handle_error(msg)

    def handle_file(self,full_path):
        try:
            with open(full_path,'rb') as reader:
                content=reader.read()
            self.send_content(content)
        except IOError as msg:
            msg="'{0}' cannot be read :{1} ".format(self.path,msg)
            self.handle_error(msg)

    def handle_error(self,msg):
        content=self.Error_Page.format(path=self.path,msg=msg)
        self.send_content(content,404)

    def send_content(self,page,status=200):
        #print 'send_content function '
        self.send_response(status)
        self.send_header('Content-Type','text/html')
        self.send_header('Content-Length','test.html')
        self.end_headers()
        self.wfile.write(page)


def run(server_class = HTTPServer, handler_class = S, port = 8080):
    # start cpp process
    global index
    index = StreamAudioIndex()
    # start http server
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print 'Starting http server...'
    httpd.serve_forever()


if __name__ == "__main__":
    from sys import argv
    if len(argv) == 2:
        run(port = int(argv[1]))
    else:
        run()

