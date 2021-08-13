#pragma once
#include <QtXml>

namespace cobb::qt::xml {
   extern QVector<QDomElement> child_elements_of_type(QDomElement parent, const QString& node_name, Qt::CaseSensitivity cs = Qt::CaseSensitivity::CaseSensitive);
   extern QDomElement first_child_element_of_type(QDomElement parent, const QString& node_name, Qt::CaseSensitivity cs = Qt::CaseSensitivity::CaseSensitive);

   extern bool element_has_child_elements(QDomElement);

   template<class T> class const_iterable_node_list {
      public:
         class const_iterator {
            friend const_iterable_node_list;
            //
            const const_iterable_node_list& target;
            int index = 0;
            
            protected:
               const_iterator(const const_iterable_node_list& f, int i) : target(f), index(i) {}

            public:
               #pragma region Assign
               const_iterator& operator=(const const_iterator& other) {
                  assert(&this->target == &other.target);
                  this->index = other.index;
                  return *this;
               }
               #pragma endregion
               #pragma region Modify
               const_iterator& operator++() {
                  ++index;
                  return *this;
               }
               const_iterator& operator++(int) {
                  auto temp = *this;
                  ++temp.index;
                  return temp;
               }
               const_iterator& operator--() {
                  --index;
                  return *this;
               }
               const_iterator& operator--(int) {
                  auto temp = *this;
                  --temp.index;
                  return temp;
               }
               const_iterator& operator+(int i) const noexcept {
                  auto temp = *this;
                  temp.index += i;
                  return temp;
               }
               const_iterator& operator-(int i) const noexcept {
                  auto temp = *this;
                  temp.index -= i;
                  return temp;
               }
               #pragma endregion
               #pragma region Compare
               bool operator==(const const_iterator& other) const {
                  return &other.target == &this->target && other.index == this->index;
               }
               bool operator!=(const const_iterator& other) const {
                  return &other.target != &this->target || other.index != this->index;
               }
               #pragma endregion

               QDomNode operator*() { return this->target.list.item(index); }
               const QDomNode operator*() const { return this->target.list.item(index); }
               QDomNode* operator->() { return &this->target.list.item(index); }
         };

      protected:
         const T list;

      public:
         const_iterable_node_list(const T& l) : list(l) {}

         const_iterator begin() const noexcept {
            auto it = const_iterator(*this, 0);
            return it;
         }
         const_iterator end() const noexcept {
            auto it = const_iterator(*this, this->list.size());
            return it;
         }
         const_iterator cbegin() const noexcept {
            auto it = const_iterator(*this, 0);
            return it;
         }
         const_iterator cend() const noexcept {
            auto it = const_iterator(*this, this->list.size());
            return it;
         }

         inline size_t size() const noexcept {
            return this->list.size();
         }
   };
}