/* 
 * Copyright (c) 2006  Michael Duerig  
 * Bern University of Applied Sciences
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 */

#ifdef WINDOWS
#  include <winsock2.h>
#else
#  include <errno.h>
#  include <string.h>
#  include <sys/socket.h>
#  include <arpa/inet.h>
#  include <unistd.h>
#endif

#include <stdio.h>

#include "trackerspu.h"
#include "trackerspu_udp.h"
#include "cr_error.h"

#ifdef WINDOWS
#  define EINTR WSAEINTR
#else
#  define INVALID_SOCKET -1
#  define SOCKET_ERROR -1
#endif


static int crGetLastError() {
#ifdef WINDOWS
  return WSAGetLastError();
#else
  return errno;
#endif
}

static void crClose(crSocket sock) {
#ifdef WINDOWS
  closesocket(sock);
#else
  close(sock);
#endif
}

static void SocketError(char *msg, int LastError) {
  crWarning("Tracker SPU: %s", msg);
  crWarning("Tracker SPU: WSAGetLastError returned %d", LastError);
}

CR_THREAD_PROC_DECL SocketThreadProc(void* arg) {
  enum {BUF_SIZE = 256 };

  int LastError;
  int RecvLen;
  char buf[BUF_SIZE]; 
  CRmatrix pose;
  static GLvectorf zero = {0.0f, 0.0f, 0.0f, 1.0f};

  for (;;) {
    if( (RecvLen = recvfrom(tracker_spu.listen_sock, buf, BUF_SIZE - 1, 0, NULL, NULL)) == SOCKET_ERROR ) {  

      LastError = crGetLastError();

      switch(LastError) {

#ifdef WINDOWS        
        case WSAESHUTDOWN:
          CR_THREAD_EXIT(0);
#endif

        case EINTR:
          continue;

        default:
          SocketError("Error on server socket of tracker SPU", LastError);
          CR_THREAD_EXIT(0);
      }
    }

    else {
      buf[RecvLen] = 0; 

      if (!unpackTrackerPose(&pose, buf)) 
        crWarning("Tracker SPU: Invalid datagram for tracker SPU");

      else {
        GLvectorf *l = &(tracker_spu.nextPos->left);
        GLvectorf *r = &(tracker_spu.nextPos->right);

        // Transform tracker position to left eye and convert to world coordinates
        *l = zero;
        crMatrixTransformPointf(&tracker_spu.leftEyeMatrix, l);
        crMatrixTransformPointf(&pose, l);
        crMatrixTransformPointf(&tracker_spu.caveMatrix, l);
        crDebug("Tracker SPU: Tracker left  (%f, %f, %f)", l->x, l->y, l->z);
        
        // Transform tracker position to right eye and convert to world coordinates
        *r = zero;
        crMatrixTransformPointf(&tracker_spu.rightEyeMatrix, r);
        crMatrixTransformPointf(&pose, r);
        crMatrixTransformPointf(&tracker_spu.caveMatrix, r);
        crDebug("Tracker SPU: Tracker right (%f, %f, %f)", r->x, r->y, r->z);

        // Mark this position as new by setting the dirty flag...
        tracker_spu.nextPos->dirty = 1;

        // ...and release it such that the main thread can consume it.
        tracker_spu.nextPos = crInterlockedExchangePointer((void*)&tracker_spu.freePos, tracker_spu.nextPos);
      }
    }
  }

  CR_THREAD_EXIT(0);
}

void trackerspuStartUDPServer() {
  struct sockaddr_in sock_addr;

#ifdef WINDOWS
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(2, 0);

  if (WSAStartup(wVersionRequested, &wsaData) != 0) {
    SocketError("Couldn't initialize winsock for tracker SPU", crGetLastError());
    return;
  }
#endif
 
  if ( (tracker_spu.listen_sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET ) {
    SocketError("Could not create server socket for tracker SPU", crGetLastError());
    return;
  }

  memset(&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;

  if (!tracker_spu.listenIP)
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
  else
    sock_addr.sin_addr.s_addr = inet_addr(tracker_spu.listenIP);

  sock_addr.sin_port = htons(tracker_spu.listenPort); 

  if ( bind(tracker_spu.listen_sock, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) != 0 ) {
    SocketError("Could not bind server socket for tracker SPU", crGetLastError());
    crClose(tracker_spu.listen_sock);
    return;
  }

  if (!crCreateThread(&tracker_spu.socketThread, 0, SocketThreadProc, NULL)) {
    SocketError("Could not create socket thread for tracker SPU", crGetLastError());
    crClose(tracker_spu.listen_sock);
    return;
  }
}

void trackerspuStopUDPServer() {
  crClose(tracker_spu.listen_sock);
  crThreadJoin(tracker_spu.socketThread);

#ifdef WINDOWS
  WSACleanup();
#endif
}

static char calcChecksum(const char *buf) {
  char cs = 0;
  const char *p;

  for(p = buf; *p != 0; p++)
    cs -= *p;

  return cs;
}

int packTrackerPose(const CRmatrix* pose, void *buf, int len) {
  int l;
  char *b = (char *)buf;

  l = snprintf(b, len, 
    "%f %f %f %f"
    "%f %f %f %f"
    "%f %f %f %f"
    "%f %f %f %f cs", 
    pose->m00, pose->m10, pose->m20, pose->m30,
    pose->m01, pose->m11, pose->m21, pose->m31,
    pose->m02, pose->m12, pose->m22, pose->m32,
    pose->m03, pose->m13, pose->m23, pose->m33);

  // Patch in checksum
  if (l <= len) {
    b[l - 2] = 0;
    b[l - 2] = calcChecksum(b);
    b[l - 1] = 0;
    return l;
  }
  else
    return 0;
}

int unpackTrackerPose(CRmatrix *pose, const void* buf) {
  if (calcChecksum(buf) != 0)
    return 0;

  return sscanf((char *)buf, 
    "%f %f %f %f"
    "%f %f %f %f"
    "%f %f %f %f"
    "%f %f %f %f",
    &pose->m00, &pose->m10, &pose->m20, &pose->m30,
    &pose->m01, &pose->m11, &pose->m21, &pose->m31,
    &pose->m02, &pose->m12, &pose->m22, &pose->m32,
    &pose->m03, &pose->m13, &pose->m23, &pose->m33) == 16;
}

