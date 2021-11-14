import socket
import argparse

def parse_args():
    parser = argparse.ArgumentParser(description='Setup the client')
    parser.add_argument('--online', dest='online', action='store_true',
                        help='Set client to do local computation')
    parser.add_argument('--offline', dest='online', action='store_false',
                        help='Set client to do server computation')

    args = parser.parse_args()
    return args

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 23657        # The port used by the server

def main(args):
    online = args.online
    exit_op = False

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        while not exit_op:

            try:
                op_input = input('Enter function to process (num op num): ')

                if (op_input == 'q'):
                    exit_op = True
                    op_input = '0.0 q 0.0'

                if ('sqrt of' in op_input):
                    split_input = op_input.split(" ")
                    op_input = split_input[-1] + " ^ " + str(0.5)

                split_input = op_input.split(" ")

                if (len(split_input) < 3) and not exit_op:
                    raise Exception('Bad Input', 'User input does not meet critia for protocol, enter equation as num op num')

                op_list = ['+', '-', '*', '/', '^', 'q']

                num1 = float(split_input[0])
                if split_input[1] in op_list:
                    op = split_input[1]
                else:
                    raise ValueError('Bad Input', "User entered bad op '{}'".format(split_input[1]))
                num2 = float(split_input[2])

                if op == '/' and num2 == 0.0:
                    raise ValueError('Bad Input', "Cannot divide by 0")

                if op == '^' and (num1 < 0 and num2 < 1):
                    raise ValueError('Bad Input', "Cannot take the square root of a negative number")

                op_to_send = "{} {} {} \n".format(op, num1, num2)
                s.sendall(op_to_send.encode('utf-8'))

                if op != 'q':
                    data = s.recv(1024)
                    print('Result {0:.15f}'.format(float(data.decode(encoding='UTF-8',errors='strict').split('\n')[0])))

            except Exception as e:
                print(e)
                print("Please try a different input set")
                continue

if __name__ == '__main__':
    main(parse_args())
