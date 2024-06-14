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
#include "files.h"
#include <sys/stat.h>
#include "intrusive_windows_defines.h"

namespace cobb {
   mapped_file::~mapped_file() {
      if (this->_view) {
         UnmapViewOfFile(this->_view);
         this->_view = nullptr;
      }
   }
   void mapped_file::open(wchar_t const* path) noexcept {
      if (this->_view) {
         UnmapViewOfFile(this->_view);
         this->_view = nullptr;
      }
      HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (file == INVALID_HANDLE_VALUE) {
         this->_error = GetLastError();
         return;
      }
      uint64_t size = 0;
      {
         struct _stat64 info;
         _wstati64(path, &info);
         size = info.st_size;
      }
      this->_size = size;
      HANDLE mapped_file = CreateFileMappingW(file, NULL, PAGE_READONLY, (uint32_t)(size >> 0x20), (uint32_t)size, NULL);
      CloseHandle(file);
      if (mapped_file == NULL) {
         this->_error = GetLastError();
         return;
      }
      this->_view = MapViewOfFile(mapped_file, FILE_MAP_READ, 0, 0, 0);
      if (this->_view == nullptr) {
         this->_error = GetLastError();
         CloseHandle(mapped_file);
         return;
      }
      CloseHandle(mapped_file);
      this->_error = 0;
   }
   void mapped_file::open(FILE* file) noexcept {
      if (this->_view) {
         UnmapViewOfFile(this->_view);
         this->_view = nullptr;
      }
      uint64_t size = 0;
      {
         fpos_t _pos;
         fgetpos(file, &_pos);
         fseek(file, 0, SEEK_END);
         fpos_t _end;
         fgetpos(file, &_end);
         size = _end;
         fseek(file, _pos, SEEK_SET);
      }
      this->_size = size;
      HANDLE mapped_file = CreateFileMappingW((HANDLE)_get_osfhandle(_fileno(file)), NULL, PAGE_READONLY, (uint32_t)(size >> 0x20), (uint32_t)size, NULL);
      if (mapped_file == NULL) {
         this->_error = GetLastError();
         return;
      }
      this->_view = MapViewOfFile(mapped_file, FILE_MAP_READ, 0, 0, 0);
      if (this->_view == nullptr) {
         this->_error = GetLastError();
         CloseHandle(mapped_file);
         return;
      }
      CloseHandle(mapped_file);
      this->_error = 0;
   }
}