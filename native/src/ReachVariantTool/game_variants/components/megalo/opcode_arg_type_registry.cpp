#include "opcode_arg_type_registry.h"
#include "opcode_arg.h"
#include "opcode_arg_types/all.h"
#include "../../../helpers/qt/string.h"
//
#include "actions.h"
#include "conditions.h"

namespace Megalo {
   bool OpcodeArgTypeRegistry::_has_type(const OpcodeArgTypeinfo& t) const noexcept {
      size_t size = this->types.size();
      for (size_t i = 0; i < size; i++)
         if (this->types[i] == &t)
            return true;
      return false;
   }
   OpcodeArgTypeRegistry::OpcodeArgTypeRegistry() {
      //
      // We need to add all of the OpcodeArgTypeinfos to the registry. Now, I know what you're thinking: 
      // why not just have OpcodeArgTypeinfo's constructor call OpcodeArgTypeRegistry::register_type? 
      // Yeah, I remember when I liked C++, too. Here's the problem with that: it breaks if we do this:
      //
      //    static someTypeinfo = OpcodeArgTypeinfo(...).some_chainable_decorator_method(...);
      //
      // In those cases, and only those cases, the typeinfo gets constructed in some other location and 
      // then moved to the final location, and crucially: the constructor runs at that other location, 
      // and so adds that other location (which will then be deleted and trashed) to the registry. There 
      // appears to be no way to control this behavior. Even if I were willing to just pass absolutely 
      // every single parameter through the constructor (which I'm not, given that this language lacks 
      // named arguments), I extremely do not trust that that would continue to work. I can't find any 
      // readily obvious explanation of why the other way is going wrong, which means I can't trust that 
      // the "only constructors are free of sin" way won't somehow start to go wrong at some point in 
      // the future.
      //
      // No. And I'm also not manually adding every single typeinfo to a list; that's not maintainable 
      // in the slightest. We're gonna do this the lazy way: iterate over every single opcode definition, 
      // grab every single type, and add it to the list. It's horribly inefficient, but it's only going 
      // to happen one time, and in my tests I don't even notice a delay. If anyone has a problem with 
      // that, they can take it up with Bjarne Stroustrup. I wash my hands of this.
      //
      for (auto& func : conditionFunctionList) {
         for (auto& base : func.arguments) {
            if (!this->_has_type(base.typeinfo))
               this->register_type(base.typeinfo);
         }
      }
      for (auto& func : actionFunctionList) {
         for (auto& base : func.arguments) {
            if (!this->_has_type(base.typeinfo))
               this->register_type(base.typeinfo);
         }
      }
   }
   void OpcodeArgTypeRegistry::register_type(const OpcodeArgTypeinfo& type) {
      this->types.push_back(&type);
   }
   const OpcodeArgTypeinfo* OpcodeArgTypeRegistry::get_by_internal_name(const QString& name) const {
      for (auto type : this->types)
         if (cobb::qt::stricmp(name, type->internal_name) == 0)
            return type;
      return nullptr;
   }
   const OpcodeArgTypeinfo* OpcodeArgTypeRegistry::get_static_indexable_type(const QString& name) const {
      for (auto type : this->types)
         if (type->can_be_static() && cobb::qt::stricmp(name, type->internal_name) == 0)
            return type;
      return nullptr;
   }
   const OpcodeArgTypeinfo* OpcodeArgTypeRegistry::get_variable_type(const QString& name) const {
      for (auto type : this->types)
         if (type->is_variable() && cobb::qt::stricmp(name, type->internal_name) == 0)
            return type;
      return nullptr;
   }
   void OpcodeArgTypeRegistry::lookup_imported_name(const QString& name, type_list_t& out) const {
      out.clear();
      for (auto type : this->types)
         if (type->has_imported_name(name))
            out.push_back(type);
   }
}