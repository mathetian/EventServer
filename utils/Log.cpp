// Copyright (c) 2014 The EventServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "Log.h"
using namespace utils;

ostream* Log::m_out;
Log      Log::m_log = Log::warn;
int      Log::m_level;