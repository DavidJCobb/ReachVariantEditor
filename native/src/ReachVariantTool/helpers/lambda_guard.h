/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#pragma once
#include <exception>

namespace cobb {
   //
   // Guard object which  executes a lambda on destruction.  This is akin to the 
   // proposed `scope_exit` guard object included in "C++ Extensions for Library 
   // Fundamentals, Version 3." One difference is that this guard doesn't need a 
   // state bool, because it cannot be released early.
   // 
   // The official `scope_exit` comes with the following caveat:
   // 
   //    Note: If the exit function  object of a [...]  scope_exit object refers 
   //    to a  local variable  of the function where it is defined,  e.g.,  as a 
   //    lambda capturing  the variable by reference, and that  variable is used 
   //    as a return operand in that function,  that variable might have already 
   //    been returned  when the scope_guard's destructor executes,  calling the 
   //    exit function. This can lead to surprising behavior.
   // 
   // This note existed  as far back as the  original proposal's  sixth revision 
   // (P0052R6), but I can't  find an explanation for  how this actually ends up 
   // causing problems.  Apparently, the issue was raised  by the BSI C++ panel; 
   // I expect that'd likely be on their  private mailing list, so that's a dead 
   // end. Just try to avoid mucking around  with return values (e.g. using this 
   // to test them against postconditions), I guess.
   //
   template<typename Functor> class lambda_guard {
      private:
         Functor functor;
      public:
         lambda_guard(Functor&& f) noexcept : functor(std::move(f)) {}
         lambda_guard(const Functor&) = delete;

         lambda_guard& operator=(const lambda_guard&) = delete;
         lambda_guard& operator=(lambda_guard&&) = delete;

         ~lambda_guard() {
            functor();
         }
   };
   
   // Analogue to std::scope_success.
   template<typename Functor> class lambda_guard_on_success {
      private:
         Functor functor;
         int     excepts = 0;
      public:
         lambda_guard_on_success(Functor&& f) noexcept : functor(std::move(f)), excepts(std::uncaught_exceptions()) {}
         lambda_guard_on_success(const Functor&) = delete;

         lambda_guard_on_success& operator=(const lambda_guard_on_success&) = delete;
         lambda_guard_on_success& operator=(lambda_guard_on_success&&) = delete;

         ~lambda_guard_on_success() {
            if (std::uncaught_exceptions() == this->excepts)
               functor();
         }
   };
}