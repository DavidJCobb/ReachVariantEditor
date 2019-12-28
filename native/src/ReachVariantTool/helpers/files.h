#pragma once
#include <cstddef> // std::uintptr_t
#include <cstdint>
#include <cstdio>
//
#include <windows.h>
#include <io.h>
#include <memoryapi.h>
#include "intrusive_windows_defines.h"

namespace cobb {
   class mapped_file {
      protected:
         void*    _view  = nullptr;
         uint64_t _size  = 0;
         uint32_t _error = 0; // WinAPI error
      public:
         mapped_file() noexcept {}
         mapped_file(wchar_t const* path) noexcept { this->open(path); }
         ~mapped_file();
         //
         const void*    data() const noexcept { return this->_view; }
         const uint64_t size() const noexcept { return this->_size; }
         //
         void open(wchar_t const* path) noexcept;
         void open(FILE* handle) noexcept;
         //
         inline operator bool() const noexcept { return !(this->_error) && this->_view != nullptr; }
         inline uint32_t get_error() const noexcept { return this->_error; }
         //
         uint32_t read_from(uint32_t pos, void* buffer, uint32_t size) const noexcept {
            std::uintptr_t addr = (std::uintptr_t)this->_view + pos;
            memcpy(buffer, (void*)addr, size);
            return size;
         }
         uint32_t read_from(uint32_t pos, char* buffer, uint32_t size) const noexcept {
            return this->read_from(pos, (void*)buffer, size);
         }
         template<typename T> uint32_t read_from(uint32_t pos, T& field, uint32_t size) const noexcept {
            std::uintptr_t addr = (std::uintptr_t)this->_view + pos;
            memcpy(&field, (void*)addr, size);
            return size;
         }
         template<typename T> uint32_t read_from(uint32_t pos, T& field) const noexcept {
            std::uintptr_t addr = (std::uintptr_t)this->_view + pos;
            memcpy(&field, (void*)addr, sizeof(field));
            return sizeof(field);
         }
         //
         inline bool is_in_bounds(uint32_t offset, uint32_t bytes) const noexcept {
            return ((uint64_t)offset + bytes) < this->_size;
         }
   };
   class file_guard {
      protected:
         FILE* raw = nullptr;
      public:
         file_guard(FILE* r) : raw(r) {};
         ~file_guard() {
            if (this->raw)
               fclose(this->raw);
         }
   };
}