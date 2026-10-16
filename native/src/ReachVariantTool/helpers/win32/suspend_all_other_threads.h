#pragma once

namespace cobb::win32 {
   // Intended for use in crash-logging code, i.e. after you've hit a point of no 
   // return and you no longer consider the current process salvageable.
   extern void suspend_all_other_threads();
}