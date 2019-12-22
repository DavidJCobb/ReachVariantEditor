#include "files.h"
#include <cassert>
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
      uint64_t size = 0;
      {
         struct _stat64 info;
         _wstati64(path, &info);
         size = info.st_size;
      }
      this->_size = size;
      HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if (file == INVALID_HANDLE_VALUE) {
         printf("<<Failed to open <%S> to create a file mapping. (%d)>>", path, GetLastError());
         assert(false && "Failed to map file.");
      }
      HANDLE mapped_file = CreateFileMapping(file, NULL, PAGE_READONLY, (uint32_t)(size >> 0x20), (uint32_t)size, NULL);
      CloseHandle(file);
      if (mapped_file == NULL) {
         printf("<<Failed to create file mapping object for <%S>. (%d)>>", path, GetLastError());
         assert(false && "Failed to map file.");
      }
      this->_view = MapViewOfFile(mapped_file, FILE_MAP_READ, 0, 0, 0);
      if (this->_view == nullptr) {
         printf("<<Failed to map view of file. (%d)>>", GetLastError());
         CloseHandle(mapped_file);
         assert(false && "Failed to map file.");
      }
      CloseHandle(mapped_file);
   }
   void mapped_file::open(FILE* file) noexcept {
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
      HANDLE mapped_file = CreateFileMapping((HANDLE)_get_osfhandle(_fileno(file)), NULL, PAGE_READONLY, (uint32_t)(size >> 0x20), (uint32_t)size, NULL);
      if (mapped_file == NULL) {
         printf("<<Failed to create file mapping object. (%d)>>", GetLastError());
         assert(false && "Failed to map file.");
      }
      this->_view = MapViewOfFile(mapped_file, FILE_MAP_READ, 0, 0, 0);
      if (this->_view == nullptr) {
         printf("<<Failed to map view of file. (%d)>>", GetLastError());
         CloseHandle(mapped_file);
         assert(false && "Failed to map file.");
      }
      CloseHandle(mapped_file);
   }
}