#include <Arduino.h>
#include "SessionStream.h"

bool SessionStream::recvBuffer() {
  // Only receive if there's noting already in the buffer
  if (recvBuf.length() > 0) return true;

  // See if there's any data to receive
  int size = 0;
  unsigned long timeout = getTimeout() * TIMEOUT_RETRIES;
  unsigned long start = millis();
  while (size == 0) {
    int session0, session1;
    if (!modem->getCommonChannelRecvBuffers(session0, session1)) return false;
    if (session == 0) size = session0; else size = session1;
    if (size == 0) {
      if (millis() - start > timeout) return false;
      delay(CHECK_INTERVAL);
    }
  }

  // Receive data to fill the buffer
  int result = 0;
  if (!modem->recvCommonChannelData(session, &recvBuf, MAX_RECV_BUFFER, result)) return false;
  if (result != 0) return false;
  return true;
}

bool SessionStream::sendBuffer() {
  // Only send if there's something to send
  if (sendBuf.length() == 0) return true;

  // Send the data from the buffer
  int retries = 0;
  while (true) {
    if (modem->sendCommonChannelData(session, &sendBuf)) break;
    if (++retries >= TIMEOUT_RETRIES) return false;
  }
  sendBuf = "";
  return true;
}

SessionStream::SessionStream(byte session, Modem *modem) : session(session), modem(modem) {
  setTimeout(modem->getStream()->getTimeout());
}

int SessionStream::peek() {
  // Not implemented
  return -1;
}

int SessionStream::read() {
  // Check if there's something to read
  if (!recvBuffer()) return -1;

  // Read the next character
  char read = recvBuf.charAt(0);
  recvBuf.remove(0, 1);
  return read;
}

int SessionStream::available() {
  // Check if there's something to read
  if (!recvBuffer()) return 0;

  // Return how much to read
  return recvBuf.length();
}

size_t SessionStream::write(uint8_t data) {
  if (sendBuf.length() >= MAX_SEND_BUFFER) {
    if (!sendBuffer()) return 0;
  }
  sendBuf.concat((char)data);
  return 1;
}

void SessionStream::flush() {
  // Send any data in the buffer
  sendBuffer();

  // Empty the receive buffer
  recvBuf = "";

  // Wait till the send and receive buffers are empty
  int sendSize = 1, recvSize = 1;
  unsigned long timeout = getTimeout() * TIMEOUT_RETRIES;
  unsigned long start = millis();
  do {
    int session0, session1;
    if (sendSize > 0) {
      if (!modem->getCommonChannelSendBuffers(session0, session1)) return;
      if (session == 0) sendSize = session0; else sendSize = session1;
    }
    if (recvSize > 0) {
      if (!modem->getCommonChannelRecvBuffers(session0, session1)) return;
      if (session == 0) recvSize = session0; else recvSize = session1;
    }
    if (recvSize > 0) {
      if (!recvBuffer()) return;
      recvBuf = "";
    }
    if (sendSize > 0 || recvSize > 0) {
      if (millis() - start > timeout) return;
      delay(CHECK_INTERVAL);
    }
  } while (sendSize > 0 || recvSize > 0);
}