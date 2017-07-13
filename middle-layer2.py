#!/usr/bin/env python

# https://gist.github.com/bradmontgomery/2219997

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
#import SocketServer
import subprocess
import sys, os 

cpp_proc = 0

class S(BaseHTTPRequestHandler):
    Page = '''\
<html>
    <head>
<style>
    textarea {
        display: block;
        min-width: 320px;
        min-height: 180px;
    }
</style>
    </head>
    <body>
        <textarea id="text-input"></textarea>
        <button id="submit-btn">submit</button>
        <textarea id="text-output"></textarea>
    <script>
        let input_elem = document.querySelector('#text-input');
        let output_elem = document.querySelector('#text-output');
        let submit_btn = document.querySelector('#submit-btn');
        submit_btn.addEventListener('click', (evt) => {
            let req = fetch('http://172.25.102.36:8080/', {
                method: 'POST', 
                body: input_elem.value
            }).then((reply) => {
                return reply.text();
            }).then((json_str) => {
                let obj = JSON.parse(json_str);
                output_elem.value = obj.content;
                console.log(obj);
            });
        });
    </script>
    </body>
</html>
    '''

    def do_POST(self):
        global cpp_proc
        # read input
        content_len = int(self.headers['Content-Length'])
        print content_len
        data = self.rfile.read(content_len)
        # print "received len = " + str(content_len)
        # redirect to cpp_proc
        cpp_proc.stdin.write(format(content_len, '08x'))
        cpp_proc.stdin.write(data)
        # read back result
        out_len = int(cpp_proc.stdout.read(8), 16);
        out_str = cpp_proc.stdout.read(out_len);
        # print "cpp_proc out_len = " + str(out_len)
        # send to client
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.send_header('Content-Length', str(out_len))
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(out_str)

    def do_GET(self):
        """Respond to a GET request."""
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.send_header("Content-Length", str(len(self.Page)))
        self.end_headers()
        self.wfile.write(self.Page)
        

def run(server_class = HTTPServer, handler_class = S, port = 8081):
    # start cpp process
    global cpp_proc
    cpp_proc = subprocess.Popen(['./cmake-build-debug/sample_connector'], stdin = subprocess.PIPE, stdout = subprocess.PIPE)
    print 'Started cpp process'
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

