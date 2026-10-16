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
#include <type_traits>
namespace cobb {
   //
   // Helper class for making singletons. Some singleton Subclass should derive from 
   // the singleton_ex<Subclass> class (yes, the class should be a template parameter 
   // for its own superclass). Offers the following features:
   // 
   //  - Disable movement and copying
   // 
   //  - A static `get_or_create` function which constructs an instance, if applicable
   // 
   //  - A static `get` function which acts as a fast getter, assuming the instance is 
   //    already constructed
   //
   template<typename T> class singleton_ex {
      protected:
         static T* instance;

         singleton_ex() {
            static_assert(std::is_base_of_v<singleton_ex<T>, T>);
            T::instance = (T*) this;
         }

         // hack to work around the inability to access T::T if it's protected, without 
         // requiring T to friend singleton_ex:
         struct _unprotect : public T {
            _unprotect() : T() {}
         };

      public:
         static T& get_or_create() {
            static _unprotect instance;
            return instance;
         }
         static T& get() {
            return *T::instance;
         }

         singleton_ex(singleton_ex&&) = delete;
         singleton_ex(const singleton_ex&) = delete;

         singleton_ex& operator=(singleton_ex&&) = delete;
         singleton_ex& operator=(const singleton_ex&) = delete;
   };
   template<typename T> T* singleton_ex<T>::instance = nullptr;
}