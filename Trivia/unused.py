"""import socket
import json
import time

SERVER_IP = '127.0.0.1'
SERVER_PORT = 55551

def send_request(request: dict):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((SERVER_IP, SERVER_PORT))
        # Serialize and send the request
        msg = json.dumps(request).encode()
        sock.sendall(msg)
        # Receive the response
        response = sock.recv(4096)
        return json.loads(response.decode())

def test_signup_and_login():
    username = "testuser"
    password = "testpass"

    print("\n---- First Signup Attempt ----")
    response = send_request({
        "type": "signup",
        "username": username,
        "password": password
    })
    print("Response:", response)

    print("\n---- Duplicate Signup Attempt ----")
    response = send_request({
        "type": "signup",
        "username": username,
        "password": password
    })
    print("Response:", response)

    print("\n---- Login with Unregistered User ----")
    response = send_request({
        "type": "login",
        "username": "not_registered",
        "password": "something"
    })
    print("Response:", response)

    print("\n---- Valid Login Attempt ----")
    response = send_request({
        "type": "login",
        "username": username,
        "password": password
    })
    print("Response:", response)

    print("\n---- Duplicate Login Attempt ----")
    response = send_request({
        "type": "login",
        "username": username,
        "password": password
    })
    print("Response:", response)

    print("\n---- Invalid Username Tests ----")
    invalid_usernames = ["", "a", " ", "user@name", "user name"]
    for invalid_username in invalid_usernames:
        response = send_request({
            "type": "signup",
            "username": invalid_username,
            "password": "pass123"
        })
        print(f"Signup with username '{invalid_username}' => Response: {response}")

if __name__ == "__main__":
    test_signup_and_login()
"""
