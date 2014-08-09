// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include <set>
#include <map>
#include <list>
#include <queue>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
using namespace std;