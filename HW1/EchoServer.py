import socket

def main(ip, port):
    client_socket: socket
    server_socket: socket
    msg: str

    # create unnamed network socket, bind and listen
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((LOCALHOST, PORT))
    server_socket.listen(1)

    print("Server started ... waiting for client request..")

    while True:
        # accept a connection to a client
        client_socket, client_address = server_socket.accept()
        print("Client connected ...")

        while True:
            # read data from client
            raw_data = client_socket.recv(2048)
            msg = raw_data.decode()
            print(msg, end ="")

            # send data back to client
            client_socket.send(bytes(msg, 'UTF-8'))

            if msg[0] == 'q':
                break


        print("\nClient at ", client_address, " disconnected...")

        # cleanup
        client_socket.close()

LOCALHOST = "127.0.0.1"
PORT = 23657

if __name__ == '__main__':
    main(LOCALHOST, PORT)

