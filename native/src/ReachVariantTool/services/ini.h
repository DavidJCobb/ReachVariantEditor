#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace ReachINI {
   //
   // There's a callback system for changes; see bottom of header file
   //
   struct setting;
   enum class setting_type {
      boolean,
      float32,
      integer_signed,
      integer_unsigned,
   };
   union setting_value_union {
      bool     b;
      float    f;
      int32_t  i;
      uint32_t u;
      //
      setting_value_union() : u(0) {};
      setting_value_union(bool v) : b(v) {};
      setting_value_union(float v) : f(v) {};
      setting_value_union(int32_t v) : i(v) {};
      setting_value_union(uint32_t v) : i(v) {};
   };

   template<typename T> struct setting_type_constant_for_type {};
   template<> struct setting_type_constant_for_type<bool> { static constexpr setting_type value = setting_type::boolean; };
   template<> struct setting_type_constant_for_type<float> { static constexpr setting_type value = setting_type::float32; };
   template<> struct setting_type_constant_for_type<int32_t> { static constexpr setting_type value = setting_type::integer_signed; };
   template<> struct setting_type_constant_for_type<uint32_t> { static constexpr setting_type value = setting_type::integer_unsigned; };
   template<typename T> static constexpr setting_type setting_type_constant_for_type_v = setting_type_constant_for_type<T>::value;

   //
   // INI SETTING DEFINITIONS.
   //
   // The macro used here intentionally differs from the one used in the CPP file. Keep the namespaces 
   // synchronized between the two files.
   //
   #define REACHTOOL_MAKE_INI_SETTING(name, category, value) extern setting name;
   namespace Editing {
      REACHTOOL_MAKE_INI_SETTING(bAllowUnsafeValues, "Editing", false);
   }
   namespace UIWindowTitle {
      REACHTOOL_MAKE_INI_SETTING(bShowFullPath,     "UIWindowTitle", true);
      REACHTOOL_MAKE_INI_SETTING(bShowVariantTitle, "UIWindowTitle", true);
   }
   #undef REACHTOOL_MAKE_INI_SETTING

   //
   // Change callback system.
   // If (setting) is nullptr, then it's a generic change event. (For now, specific change events are not 
   // used or supported, but I want room to add them later.)
   //
   typedef void(*ChangeCallback)(setting* setting, setting_value_union oldValue, setting_value_union newValue);
   extern  void RegisterForChanges(ChangeCallback c);
   extern  void UnregisterForChanges(ChangeCallback c);
   extern  void SendChangeEvent();
   extern  void SendChangeEvent(setting* setting, setting_value_union oldValue, setting_value_union newValue);

   //
   // INTERNALS BELOW.
   //
   class INISettingManager {
      private:
         typedef std::vector<setting*> VecSettings;
         typedef std::unordered_map<std::string, VecSettings> MapSettings;
         VecSettings settings;
         MapSettings byCategory;
         //
         struct _CategoryToBeWritten { // state object used when saving INI settings
            _CategoryToBeWritten() {};
            _CategoryToBeWritten(std::string name, std::string header) : name(name), header(header) {};
            //
            std::string name;   // name of the category, used to look up INISetting pointers for found setting names
            std::string header; // the header line, including whitespace and comments
            std::string body;   // the body
            VecSettings found;  // found settings
            //
            void Write(INISettingManager* const, std::fstream&);
         };
         //
         VecSettings& GetCategoryContents(std::string category);
         //
      public:
         static INISettingManager& GetInstance();
         //
         void Add(setting* setting);
         void Load();
         void Save();
         void AbandonPendingChanges();
         //
         setting* Get(std::string& category, std::string& name) const;
         setting* Get(const char*  category, const char*  name) const;
         void ListCategories(std::vector<std::string>& out) const;
   };
   struct setting {
      setting(const char* name, const char* category, bool   v) : name(name), category(category), default(v), current(v), type(setting_type::boolean) {
         INISettingManager::GetInstance().Add(this);
      };
      setting(const char* name, const char* category, float  v) : name(name), category(category), default(v), current(v), type(setting_type::float32) {
         INISettingManager::GetInstance().Add(this);
      };
      setting(const char* name, const char* category, int32_t v) : name(name), category(category), default(v), current(v), type(setting_type::integer_signed) {
         INISettingManager::GetInstance().Add(this);
      };
      setting(const char* name, const char* category, uint32_t v) : name(name), category(category), default(v), current(v), type(setting_type::integer_unsigned) {
         INISettingManager::GetInstance().Add(this);
      };

      const char* const name;
      const char* const category;
      setting_value_union current;
      setting_value_union default;
      setting_value_union pending;
      const setting_type type;
      //
      std::string to_string() const;
      //
      void modify(bool v) noexcept { this->current.b = v; }
      void modify(float v) noexcept { this->current.f = v; }
      void modify(int32_t v) noexcept { this->current.i = v; }
      void modify(uint32_t v) noexcept { this->current.u = v; }
      //
      template<typename T> void modify_and_signal(T value) {
         setting_type t = setting_type_constant_for_type_v<T>;
         if (t != this->type)
            return;
         setting_value_union old = this->current;
         this->modify(value);
         SendChangeEvent(this, old, this->current);
      }
      //
      void abandon_pending_changes() noexcept {
         switch (this->type) {
            case setting_type::boolean:
               this->pending.b = this->current.b;
               return;
            case setting_type::float32:
               this->pending.f = this->current.f;
               return;
            case setting_type::integer_signed:
               this->pending.i = this->current.i;
               return;
            case setting_type::integer_unsigned:
               this->pending.u = this->current.u;
               return;
         }
      }
      void commit_pending_changes() noexcept {
         switch (this->type) {
            case setting_type::boolean:
               if (this->pending.b != this->current.b)
                  this->modify_and_signal(this->pending.b);
               return;
            case setting_type::float32:
               if (this->pending.f != this->current.f)
                  this->modify_and_signal(this->pending.f);
               return;
            case setting_type::integer_signed:
               if (this->pending.i != this->current.i)
                  this->modify_and_signal(this->pending.i);
               return;
            case setting_type::integer_unsigned:
               if (this->pending.u != this->current.u)
                  this->modify_and_signal(this->pending.u);
               return;
         }
      }
   };
}