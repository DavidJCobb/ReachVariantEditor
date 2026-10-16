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
#include <variant>

namespace cobb::variants {
   namespace impl {
      template<typename T>
      struct dummy {};

      template<typename Variant>
      struct make_dumb;

      template<typename... Types>
      struct make_dumb<std::variant<Types...>> {
         using type = std::variant<dummy<Types>...>;
      };
   }

   template<typename Variant, typename T>
   constexpr std::size_t index_of_type = []() {
      typename impl::make_dumb<Variant>::type v{ impl::dummy<T>{} };
      return v.index();
   }();
}