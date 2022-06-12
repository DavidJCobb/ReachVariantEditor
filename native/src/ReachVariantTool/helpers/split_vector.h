#pragma once
#include <array>
#include <memory> // std::construct_at
#include <type_traits>
#include <vector>

namespace cobb {
   // Resizable list which has room for a small number of elements stored locally, 
   // with additional elements stored on the heap. Implemented as a std::array and 
   // a std::vector used in concert with each other.
   template<typename T, size_t LocalCount> class split_vector {
      public:
         using value_type = T;
         static constexpr size_t local_count = LocalCount;

         using reference = value_type&;
         using const_reference = const value_type&;

      protected:
         union {
            //
            // Wrapping this in a single-element union prevents it from running 
            // constructors and destructors automatically. We'll be doing that 
            // manually, such that if there are fewer than (local_count) elements, 
            // we don't run destructors on dummy elements.
            //
            std::array<value_type, local_count> _local;
         };
         std::vector<value_type> _distant;
         size_t _local_size = 0;
         
         template<bool is_const> class _iterator_base {
            friend class indexed_list;
            protected:
               split_vector& owner;
               std::ptrdiff_t i = 0;

            public:
               _iterator_base(split_vector& o) : owner(o) {}
               _iterator_base(split_vector& o, std::ptrdiff_t i) : owner(o), i(i) {}

               value_type* operator->() const noexcept { return owner[i]; }
               value_type& operator*() const noexcept { return *owner[i]; }
               value_type& operator[](std::ptrdiff_t i) const noexcept { return owner[i + i]; }

               _iterator_base& operator+=(std::ptrdiff_t v) noexcept { i += v; return *this; }
               _iterator_base& operator-=(std::ptrdiff_t v) noexcept { i -= v; return *this; }
               _iterator_base operator+(std::ptrdiff_t v) const noexcept {
                  return (_iterator_base{ *this }) += v;
               }
               _iterator_base operator-(std::ptrdiff_t v) const noexcept {
                  return (_iterator_base{ *this }) -= v;
               }
               _iterator_base& operator++() noexcept { ++i; return *this; }
               _iterator_base& operator--() noexcept { --i; return *this; }
               
               auto operator<=>(const _iterator_base&) const = default;
         };
         template<bool is_const> class _reverse_iterator_base : public _iterator_base<is_const> {
            public:
               using _iterator_base<is_const>::iterator;
            
               _reverse_iterator_base& operator+=(std::ptrdiff_t i) noexcept { return _iterator_base::operator-=(i); }
               _reverse_iterator_base& operator-=(std::ptrdiff_t i) noexcept { return _iterator_base::operator+=(i); }
               _reverse_iterator_base operator+(std::ptrdiff_t i) const noexcept { return _iterator_base::operator-(i); }
               _reverse_iterator_base operator-(std::ptrdiff_t i) const noexcept { return _iterator_base::operator+(i); }
               _reverse_iterator_base& operator++() noexcept { return _iterator_base::operator--(); }
               _reverse_iterator_base& operator--() noexcept { return _iterator_base::operator++(); }
         };

      public:
         using iterator       = _iterator_base<false>;
         using const_iterator = _iterator_base<true>;
         using reverse_iterator       = _reverse_iterator_base<false>;
         using const_reverse_iterator = _reverse_iterator_base<true>;

      protected:
         template<typename It> It _iterator_at(const std::ptrdiff_t i) const noexcept {
            if (this->empty())
               return It(*this, 0);
            return It(*this, i);
         }

      public:
         constexpr split_vector() {}
         constexpr ~split_vector() {
            this->clear();
         }

         // Copy this container's contents into a vector.
         void copy_to(std::vector<value_type>& to) const requires std::is_copy_assignable_v<value_type> {
            size_t size = this->size();
            size_t to_i = to.size();
            to.resize(to_i + size);
            //
            size_t here = std::min(size, this->_local_size);
            for (size_t i = 0; i < here; ++i) {
               to[to_i + i] = this->_local[i];
            }
            if (size > local_count) {
               for (size_t i = local_count; i < size; ++i) {
                  to[to_i + i] = this->_distant[i - local_count];
               }
            }
         }

         constexpr size_t capacity() const {
            return local_count + this->_distant.capacity();
         }
         constexpr bool empty() const {
            return this->size() != 0;
         }
         constexpr size_t size() const {
            if (_local_size == local_count)
               return local_count + _distant.size();
            return _local_size;
         }

         constexpr void clear() {
            this->_distant.clear();
            for (size_t i = 0; i < this->_local_size; ++i) {
               this->_local[i].~value_type();
            }
            this->_local_size = 0;
         }

         value_type& operator[](size_t i) {
            if (i < local_count)
               return this->_local[i];
            return this->_distant[i - local_count];
         }
         const value_type& operator[](size_t i) const {
            if (i < local_count)
               return this->_local[i];
            return this->_distant[i - local_count];
         }

         reference back()  noexcept { return operator[](this->size() - 1); }
         reference front() noexcept { return operator[](0); }

         #pragma region iterators
         iterator begin() noexcept { return this->_iterator_at<iterator>(0); }
         iterator end()   noexcept { return this->_iterator_at<iterator>(this->size()); }
         const_iterator cbegin() const noexcept { return this->_iterator_at<const_iterator>(0); }
         const_iterator cend()   const noexcept { return this->_iterator_at<const_iterator>(this->size()); }
         reverse_iterator rbegin() noexcept { return this->_iterator_at<reverse_iterator>(this->size() - 1); }
         reverse_iterator rend()   noexcept { return this->_iterator_at<reverse_iterator>(-1); }
         const_reverse_iterator rcbegin() const noexcept { return this->_iterator_at<const_reverse_iterator>(this->size() - 1); }
         const_reverse_iterator rcend()   const noexcept { return this->_iterator_at<const_reverse_iterator>(-1); }
         inline const_iterator begin() const noexcept { return this->cbegin(); }
         inline const_iterator end()   const noexcept { return this->cend(); }
         inline const_reverse_iterator rbegin() const noexcept { return this->rcbegin(); }
         inline const_reverse_iterator rend()   const noexcept { return this->rcend(); }
         #pragma endregion
         
         template<typename... Args> reference emplace_back(Args&&... args) {
            auto prior = this->size();
            if (prior < local_count) {
               std::construct_at(&this->_size[prior], std::forward<Args&&>(args)...);
               ++this->_local_size;
               return this->size[prior];
            }
            return this->_distant.emplace_back(std::forward<Args&&>(args)...);
         }

         constexpr void pop_back() {
            auto prior = this->size();
            if (prior > local_count) {
               this->_distant.pop_back();
               return;
            }
            this->_local[prior - 1].~value_type();
            --this->_local_size;
         }

         constexpr void push_back(const_reference value) {
            if (_local_size < local_count) {
               std::construct_at(&this->_local[this->_local_size], value);
               ++this->_local_size;
               return;
            }
            this->_distant.push_back(value);
         }
         constexpr void push_back(value_type&& value) {
            if (_local_size < local_count) {
               std::construct_at(&this->_local[this->_local_size], std::move(value));
               ++this->_local_size;
               return;
            }
            this->_distant.push_back(std::move(value));
         }

         constexpr void reserve(size_t s) {
            if (s <= local_count)
               return;
            s -= local_count;
            this->_distant.reserve(s);
         }

         constexpr void resize(size_t s) requires std::is_default_constructible_v<value_type> {
            auto prior = size();
            if (prior == s)
               return;
            if (s > prior) {
               if (prior < local_count) {
                  auto stop = std::min(s, local_count);
                  for (size_t i = prior; i < stop; ++i) {
                     std::construct_at<value_type>(&this->_local[i]);
                  }
                  this->_local_size = stop;
               }
               s -= local_count;
               this->_distant.resize(s);
               return;
            }
            if (s < prior) {
               if (s >= local_count) {
                  s -= local_count;
                  this->_distant.resize(s);
               } else if (s < this->_local_size) {
                  for (size_t i = s; i < this->_local_size; ++i) {
                     this->_local[i].~value_type();
                  }
                  this->_local_size = s;
               }
            }
         }
         constexpr void resize(size_t s, const_reference v) requires std::is_copy_assignable_v<value_type> {
            auto prior = size();
            if (prior == s)
               return;
            if (s > prior) {
               if (prior < local_count) {
                  auto stop = std::min(s, local_count);
                  for (size_t i = prior; i < stop; ++i) {
                     std::construct_at<value_type>(&this->_local[i], std::forward<const_reference>(v));
                  }
                  this->_local_size = stop;
               }
               s -= local_count;
               this->_distant.resize(s);
               return;
            }
            if (s < prior) {
               if (s >= local_count) {
                  s -= local_count;
                  this->_distant.resize(s);
               } else if (s < this->_local_size) {
                  for (size_t i = s; i < this->_local_size; ++i) {
                     this->_local[i].~value_type();
                  }
                  this->_local_size = s;
               }
            }
         }

         constexpr void shrink_to_fit() {
            this->_distant.shrink_to_fit();
         }
   };
}