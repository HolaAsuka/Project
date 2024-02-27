import select
import socket
import struct
import sys
import getopt
from socketserver import StreamRequestHandler as Tcp, ThreadingTCPServer


class OurProxy(Tcp):

    def handle(self):
        print("客户端正在请求连接！")
        header = self.connection.recv(2)
        VER, NMETHODS = struct.unpack("!BB", header)
        assert VER == 5, '版本错误'

        methods = self.IsAvailable(NMETHODS)
        if 0 not in set(methods):
            self.server.close_request(self.request)
            return

        self.connection.sendall(struct.pack("!BB", 5, 0))

        version, cmd, _, address_type = struct.unpack("!BBBB", self.connection.recv(4))
        assert version == 5, '版本错误'
        if address_type == 1:
            address = socket.inet_ntoa(self.connection.recv(4))
        elif address_type == 3:
            domain_length = ord(self.connection.recv(1)[0])
            address = self.connection.recv(domain_length)
        port = struct.unpack('!H', self.connection.recv(2))[0]

        try:
            if cmd == 1:
                remote = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                remote.connect((address, port))
                bind_address = remote.getsockname()
                print('客户端已建立连接')
            else:
                self.server.close_request(self.request)
            addr = struct.unpack("!I", socket.inet_aton(bind_address[0]))[0]
            port = bind_address[1]
            reply = struct.pack("!BBBBIH", 5, 0, 0, address_type, addr, port)
        except Exception as err:
            print(err)
            reply = self.ReceriveReplyFaild(address_type, 5)
        self.connection.sendall(reply)

        if reply[1] == 0 and cmd == 1:
            self.ExchangeData(self.connection, remote)
        self.server.close_request(self.request)

    def IsAvailable(self, n):
        methods = []
        for i in range(n):
            methods.append(ord(self.connection.recv(1)))
        return methods

    def VerifyAuth(self):
        version = ord(self.connection.recv(1))
        assert version == 1
        username_len = ord(self.connection.recv(1))
        username = self.connection.recv(username_len).decode('utf-8')
        password_len = ord(self.connection.recv(1))
        password = self.connection.recv(password_len).decode('utf-8')
        if username == self.username and password == self.password:
            response = struct.pack("!BB", version, 0)
            self.connection.sendall(response)
            return True
        response = struct.pack("!BB", version, 0xFF)
        self.connection.sendall(response)
        self.server.close_request(self.request)
        return False

    def ReceriveReplyFaild(self, address_type, error_number):
        return struct.pack("!BBBBIH", 5, error_number, 0, address_type, 0, 0)

    def ExchangeData(self, client, remote):
        while True:
            rs, ws, es = select.select([client, remote], [], [])
            if client in rs:
                data = client.recv(4096)
                if remote.send(data) <= 0:
                    break
            if remote in rs:
                data = remote.recv(4096)
                if client.send(data) <= 0:
                    break


if __name__ == '__main__':
    argv = sys.argv[1:]
    host = None
    port = None
    try:
        opts, args = getopt.getopt(argv, "h:p:")
    except:
        print("Error")

    for opt, arg in opts:
        if opt in ['-h']:
            host = arg
        elif opt in ['-p']:
            port = arg
    Server = ThreadingTCPServer((host, int(port)), OurProxy)
    Server.serve_forever();
