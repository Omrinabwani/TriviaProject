import socket
import time

HOST = '127.0.0.1'  # Your server IP
PORT = 55551  # Same port as in C++ server

LOGIN_REQUEST_CODE = 1
SIGNIN_REQUEST_CODE = 2


def login():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))

        # Building the message that we are sending to the server
        username = input("Enter username: ")
        password = input("Enter password: ")

        request_content = '{"username": "' + username + '", "password": "' + password + '"}'
        request_content_len = len(request_content)
        request = LOGIN_REQUEST_CODE.to_bytes(1, byteorder="big") + request_content_len.to_bytes(4,
                                                                                                 byteorder="big") + request_content.encode(
            "ascii")

        print("Request sending to server:", request)
        sock.sendall(request)

        time.sleep(1)

        # After waiting for the server to send all the bytes, we receive them and build the response
        response_code = sock.recv(1)
        response_content_len = int.from_bytes(sock.recv(4), byteorder="big")
        response_content = sock.recv(response_content_len).decode()

        print("Response content from server: ", response_content.replace("0", "SUCCESS").replace("1", "FAILED"))


def signin():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))

        # Building the message that we are sending to the server
        username = input("Enter username: ")
        password = input("Enter password: ")
        email = input("Enter mail: ")

        request_content = '{"username": "' + username + '", "password": "' + password + '", "email": "' + email + '"}'
        request_content_len = len(request_content)
        serialized_request = SIGNIN_REQUEST_CODE.to_bytes(1, byteorder="big") + request_content_len.to_bytes(4,
                                                                                                  byteorder="big") + request_content.encode(
            "ascii")

        print("Request sending to server:", serialized_request)
        sock.sendall(serialized_request)

        time.sleep(1)

        # After waiting for the server to send all the bytes, we receive them and build the response
        response_code = sock.recv(1)
        response_content_len = int.from_bytes(sock.recv(4), byteorder="big")
        response_content = sock.recv(response_content_len).decode()

        print("Response content from server: ", response_content.replace("0", "SUCCESS").replace("1", "FAILED"))


def main():
    modes = {
        "1": login,
        "2": signin
    }
    print("Welcome to Trivia - v1.0.2!")
    while True:
        print("\nWhat would you like to do:")
        print("1 - Log in")
        print ("2 - Sign in")
        print("q - Exit")

        choice = input("Enter your choice: ")
        if choice == "q":
            break
        else:
            modes[choice]()


if __name__ == "__main__":
    main()
