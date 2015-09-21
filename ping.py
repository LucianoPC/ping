#!/usr/bin/env python3

import sys
import argparse

import time
import socket
from socket import socket as Socket


def main():

    # Command line arguments. Use a server_port > 1024 by default
    # so that we can run server without sudo.
    parser = argparse.ArgumentParser()

    help_message = 'Server_Port to use'
    parser.add_argument('--server-port', '-p', default=2081, type=int,
                        help=help_message)

    help_message = 'Run a ping server'
    parser.add_argument('--run-server', '-s', action='store_true',
                        help=help_message)

    help_message = 'Server to ping, no effect if running as a server.'
    parser.add_argument('server_address', default='localhost',
                        help=help_message)

    args = parser.parse_args()

    if args.run_server:
        return run_server(args.server_port)
    else:
        return run_client(args.server_address, args.server_port,)


def run_server(server_port):
    """Run the UDP pinger server
    """

    # Create the server socket (to handle UDP requests using ipv4), make sure
    # it is always closed by using with statement.
    with Socket(socket.AF_INET, socket.SOCK_DGRAM) as server_socket:

        # The socket stays connected even after this script ends. So in order
        # to allow the immediate reuse of the socket (so that we can kill and
        # re-run the server while debugging) we set the following option. This
        # is potentially dangerous in real code: in rare cases you may get junk
        # data arriving at the socket.
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        # Set the server port
        server_socket.bind(('', server_port))

        # Start accepting ping requests
        print("Ping server ready on port", server_port)
        while True:
            # Receive message and send one back
            _, client_address = server_socket.recvfrom(1024)
            server_socket.sendto("".encode(), client_address)

    return 0


def run_client(server_address, server_port):
    """Make 10 pings to a UDP server at the given address
    """
    sent_pings = 10
    received_pings = 0

    initial_time = current_milli_time()
    for i in range(sent_pings):
        rtt = get_rtt(server_address, server_port)

        if rtt >= 0.0:
            received_pings += 1
            variables = (server_address, (i+1), rtt)
            print("send ping from %s: icmp_seq=%d time=%.3f ms" % variables)
        else:
            variables = (server_address, (i+1), "lost")
            print("send ping from %s: icmp_seq=%d time=%s" % variables)

    final_time = current_milli_time()
    total_time = final_time - initial_time

    variables = (sent_pings, received_pings, total_time)
    print("%d packets transmitted, %d received, time %.2f ms" % variables)
    return 0


def get_rtt(server_address, server_port):
    """Ping a UDP ping server running at the given address
    and show the ping status
    """
    server = (server_address, server_port)
    message = bytes("", 'UTF-8')

    client_socket = Socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_socket.settimeout(1.0)

    try:
        initial_time = current_milli_time()
        client_socket.sendto(message, server)
        client_socket.recvfrom(1024)
        final_time = current_milli_time()

        rtt = final_time - initial_time
    except:
        rtt = -1

    client_socket.close()

    return rtt


def current_milli_time():
    return float(time.time() * 1000)


if __name__ == "__main__":
    sys.exit(main())
