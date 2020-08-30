// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017-2020 Carsten Arnholm
// All rights reserved
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation and appearing in the files LICENSE.GPL2
// and LICENSE.GPL3 included in the packaging of this file.
//
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE. ALL COPIES OF THIS FILE MUST INCLUDE THIS LICENSE.
// EndLicense:

#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class safe_queue {
public:
   safe_queue(void)
   : q()
   , m()
   , c()
   {}

   ~safe_queue(void)
   {}

   void enqueue(T t)
   {
      // Unlock before notifying to avoid waking up
      // the waiting thread only to block it again.
      // Therefore this extra code block.
      {
       std::lock_guard<std::mutex> lock(m);
       q.push(t);
      }
      c.notify_one();
   }

   // return false if queue is empty
   bool try_dequeue(T& val)
   {
      std::unique_lock<std::mutex> lock(m);
      if(q.empty())return false;

      val = q.front();
      q.pop();
      return true;
  }

   // wait for new data if queue empty
   T dequeue(void)
   {
      std::unique_lock<std::mutex> lock(m);
      while(q.empty())
      {
         c.wait(lock);
      }
      T val = q.front();
      q.pop();
      return val;
   }

   size_t size() const
   {
      std::lock_guard<std::mutex> lock(m);
      return q.size();
   }

private:
   std::queue<T> q;
   mutable std::mutex m;
   std::condition_variable c;
};

#endif // SAFE_QUEUE_H
