#pragma once
#include <vector>
#include <QString>

namespace Megalo {
   class OpcodeArgTypeinfo;
   //
   class OpcodeArgTypeRegistry {
      public:
         using type_list_t = std::vector<const OpcodeArgTypeinfo*>;
         //
      protected:
         OpcodeArgTypeRegistry();
         //
         bool _has_type(const OpcodeArgTypeinfo&) const noexcept;
         //
      public:
         static OpcodeArgTypeRegistry& get() {
            static OpcodeArgTypeRegistry instance;
            return instance;
         }
         //
         std::vector<const OpcodeArgTypeinfo*> types;
         //
         void register_type(const OpcodeArgTypeinfo& type);
         const OpcodeArgTypeinfo* get_by_internal_name(const QString& name) const;
         const OpcodeArgTypeinfo* get_static_indexable_type(const QString& name) const;
         const OpcodeArgTypeinfo* get_variable_type(const QString& name) const;
         void lookup_imported_name(const QString& name, type_list_t& out) const;
   };
}