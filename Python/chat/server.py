'''
KTN-project 2014
Very simple server implementation
Group 75
'''
import SocketServer
import json
import datetime
import re

class CLientHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        self.connection = self.request
        self.ip = self.client_address[0]
        self.port = self.client_address[1]
        print 'Client connected @' + self.ip + ':' + str(self.port)

        while True:
            data = self.connection.recv(1024).strip()
            if data:
                data = json.loads(data)
                response = server.parse_message(data, self)
                if data['request'] == 'logout':
                    break
                else:
                    self.connection.sendall(json.dumps(response))
                
        print 'Client disconnected @' + self.ip + ':' + str(self.port)
        self.connection.close()

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    user_dict = {}
    messagelog = []

    def parse_message(self, message, handler):
        if message['request'] == 'login':
            return self.login(message['username'], handler)
        elif message['request'] == 'logout':
            return self.logout(handler)
        elif message['request'] == 'message':
            return self.push_chatmessage(message['message'], handler)

    def login(self, username, handler):
        response = {'response': 'login', 'username': username}
        if not re.match('^\w+$', username):
            response['error'] = 'Invalid username!'
        elif username in self.user_dict.values():
            response['error'] = 'Name already taken!'
        else:
            messagelog_string = "Previous messages:"
            for line in self.messagelog:
                messagelog_string = messagelog_string + '\n' + line

            response['messages'] = messagelog_string
            self.user_dict[handler] = username
            print username + ' has logged in!'
        return response

    def logout(self, handler):
        response = {'response': 'logout', 'username': ""}
        if not self.user_dict.has_key(handler):
            response['error'] = 'Not logged in!'
        else:
            username = self.user_dict[handler]
            response['username'] = username
            del self.user_dict[handler]
            print username + " has gone offline!"
        return response

    def push_chatmessage(self, chatmessage, handler):
        if self.user_dict.has_key(handler):
            username = self.user_dict[handler]
            message = username + ' said @ ' + datetime.datetime.now().strftime('%d/%m/%Y %H:%M:%S %Z') + ': ' + chatmessage
            self.messagelog.append(message)
            response = {'response': 'message', 'message': message}
            response = json.dumps(response)

            for addr, user in self.user_dict.iteritems():
                if username != user:
                    addr.connection.sendall(response)
            return
        else:
            response = {'response': 'message', 'error': 'You are not logged in!'}
        return response

if __name__ == "__main__":
    HOST = 'localhost'
    PORT = 9999

    # Create the server, binding to localhost on port 9999
    server = ThreadedTCPServer((HOST, PORT), CLientHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()
