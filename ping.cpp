#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <chrono>

void ping(std::string address, int port);
float getRTT(std::string address, int port);

int main(int argc, char *argv[])
{
  int port = 0;
  std::string address = "";

  if(argc == 2)
  {
    port = 2081;
  }
  else if(argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " <address> <port>" << std::endl;
    return -1;
  }

  address = argv[1];

  if(port == 0)
  {
    port = atoi(argv[2]);
  }

  ping(address, port);

  return 0;
}

void ping(std::string address, int port)
{
  int sentPings = 10;
  int receivedPings = 0;

  auto initialTime = std::chrono::high_resolution_clock::now();
  for(int i = 0; i < sentPings; i++)
  {
    float rtt = getRTT(address, port);

    if(rtt >= 0)
    {
      receivedPings += 1;
      std::cout << "send ping from " << address;
      std::cout << ": icmp_seq=" << i << " time=" << rtt << " ms" << std::endl;
    }
    else
    {
      std::cout << "send ping from " << address;
      std::cout << ": icmp_seq=" << i << " time=lost" << std::endl;
    }
  }

  auto finalTime = std::chrono::high_resolution_clock::now();
  auto deltaTimeTemp = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - initialTime).count();

  float deltaTime = ((float)deltaTimeTemp) / 1000.0f;

  std::cout << sentPings << " packets transmitted, " << receivedPings << " received, time " << deltaTime << " ms" << std::endl;
}

float getRTT(std::string address, int port)
{
  float rtt = -1.0f;
  int receiveLenght;
  int socketId = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in sockaddress;

  if(socketId < 0) {
    throw std::runtime_error("Could not create socket.");
  }

  memset((char *) &sockaddress, 0, sizeof(sockaddress));
  sockaddress.sin_family = AF_INET;
  sockaddress.sin_port = htons(port);
  sockaddress.sin_addr.s_addr = inet_addr(address.c_str());

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 100;

  char message[4096];
  socklen_t socklenght = sizeof(sockaddress);

  auto initialTime = std::chrono::high_resolution_clock::now();
  sendto(socketId, "", 0, 0, (struct sockaddr*) &sockaddress, sizeof(sockaddress));
  setsockopt(socketId, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(tv));
  receiveLenght = recvfrom(socketId, message, 4096, 0, (struct sockaddr *)&sockaddress, &socklenght);

  if(receiveLenght >= 0)
  {
    auto finalTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - initialTime).count();
    rtt = ((float)totalTime) / 1000.0f;
  }

  return rtt;
}
