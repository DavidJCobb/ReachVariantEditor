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
#include <cstdint>

namespace cobb::ini {
   enum class setting_type {
      boolean,
      float32,
      integer_signed,
      integer_unsigned,
      string,
   };
   
   template<typename T> struct setting_type_constant_for_type {};
   template<> struct setting_type_constant_for_type<bool> { static constexpr const setting_type value = setting_type::boolean; };
   template<> struct setting_type_constant_for_type<float> { static constexpr const setting_type value = setting_type::float32; };
   template<> struct setting_type_constant_for_type<int32_t> { static constexpr const setting_type value = setting_type::integer_signed; };
   template<> struct setting_type_constant_for_type<uint32_t> { static constexpr const setting_type value = setting_type::integer_unsigned; };
   template<typename T> static constexpr const setting_type setting_type_constant_for_type_v = setting_type_constant_for_type<T>::value;
}