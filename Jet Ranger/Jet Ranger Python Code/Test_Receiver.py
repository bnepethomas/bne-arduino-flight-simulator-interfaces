import socket
import json

# --- UDP Configuration ---
# This IP and port must match where your SimConnect script is sending data.
# If the SimConnect script is running on the same machine, use "127.0.0.1" (localhost).
# If it's on a different machine, use the IP address of the machine running this listener script.
LISTEN_IP = "0.0.0.0"  # Listen on all available network interfaces
LISTEN_PORT = 5005     # Must match the UDP_PORT in your SimConnect sender script

def udp_listener():
    """
    Listens for UDP packets, decodes them as JSON, and prints the data.
    """
    sock = None
    try:
        # Create a UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Bind the socket to the listening IP and port
        sock.bind((LISTEN_IP, LISTEN_PORT))
        print(f"UDP Listener started on {LISTEN_IP}:{LISTEN_PORT}")
        print("Waiting for data...")

        while True:
            # Receive data and the sender's address
            # 1024 is the buffer size; adjust if your JSON strings are longer
            data, addr = sock.recvfrom(1024)

            print(f"\nReceived packet from {addr}:")
            try:
                # Decode the received bytes to a UTF-8 string
                json_string = data.decode('utf-8')
                print(f"Raw JSON string: {json_string}")

                # Parse the JSON string into a Python dictionary
                parsed_data = json.loads(json_string)

                print("Parsed Data:")
                for key, value in parsed_data.items():
                    print(f"  {key}: {value}")

            except UnicodeDecodeError:
                print("Error: Could not decode received data as UTF-8.")
                print(f"Raw received bytes: {data}")
            except json.JSONDecodeError:
                print("Error: Could not decode received data as JSON.")
                print(f"Received string (attempted): {json_string}")
            except Exception as e:
                print(f"An unexpected error occurred during processing: {e}")

    except socket.error as e:
        print(f"Socket error: {e}")
        print(f"Ensure that port {LISTEN_PORT} is not already in use and firewall allows connections.")
    except KeyboardInterrupt:
        print("\nUDP Listener stopped by user.")
    finally:
        if sock:
            sock.close()
            print("Socket closed.")

if __name__ == "__main__":
    udp_listener()