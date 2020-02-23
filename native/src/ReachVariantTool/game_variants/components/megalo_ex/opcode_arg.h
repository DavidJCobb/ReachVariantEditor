#pragma once
#include "../../base.h"
#include "../../../helpers/bitarray.h"
#include "../../../helpers/refcounting.h"
#include <functional>
#include <QString>

namespace MegaloEx {
   class Compiler;

   class OpcodeArgValue;

   struct arg_rel_obj_list_t {
      static constexpr int count = 4;
      struct bitrange {
         uint8_t start = 0;
         uint8_t count = 0;
      };
      //
      cobb::refcount_ptr<cobb::indexed_refcountable> pointers[count];
      bitrange ranges[count];
      //
      inline bool index_is_set(uint8_t i) const noexcept { return this->pointers[i] || this->ranges[i].count; }
   };
   struct fragment_specifier {
      //
      // State object passed to and between functors, to aid in two cases: argument types that include 
      // other argument types as members (e.g. shape-arguments including number-arguments); and argument 
      // types that are represented as one argument internally but multiple arguments in scripted function 
      // calls (e.g. shape-arguments).
      //
      // Fully explained in documentation comments further below.
      //
      // This object is passed by-value, so there's no benefit to keeping it less than 8 bytes large (i.e. 
      // the size of one x64 register); feel free to stuff whatever inter-functor state we need in here.
      //
      uint8_t obj_index  = 0; // index into arg_rel_obj_list_t
      uint8_t bit_offset = 0; // helps with output/decode functors and nested types
      uint8_t which_arg  = 0; // helps identify which script-argument is currently being compiled
   };

   enum class arg_consume_result {
      failure,
      success,
      still_hungry
   };
   class OpcodeArgTypeinfo {
      //
      // TODO: WHEN IMPLEMENTING RESAVING, WE NEED TO CALL OpcodeArgValue::do_index_fixup BEFORE WRITING 
      // THE BINARY ARGUMENT DATA TO THE TARGET FILE.
      //
      // NOTE: When we implement compile functors, we need to give opcodes a way to access data on the 
      // containing game variant, i.e. when we're compiling an opcode that refers to a script option, Forge 
      // label, etc., and they need to get a refcounted pointer to the target data.
      //
      public:
         using load_functor_t        = std::function<bool(fragment_specifier, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits)>; // loads data from binary stream; returns success/failure
         using decode_functor_t      = std::function<bool(fragment_specifier, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out)>; // returns success/failure
         using compile_functor_t     = std::function<arg_consume_result(fragment_specifier, OpcodeArgValue&, arg_rel_obj_list_t& relObjs, const std::string&, Compiler&)>;
         using postprocess_functor_t = std::function<bool(fragment_specifier, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData*)>;
         //
         static bool default_postprocess_functor(fragment_specifier, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData*) { return true; } // for argument types that don't need postprocessing
         //
         struct flags {
            flags() = delete;
            enum type : uint32_t {
               none = 0,
               may_need_postprocessing = 0x00000001,
               is_variable             = 0x00000002,
               is_nestable_variable    = 0x00000004,
               is_static_variable      = 0x00000008, // e.g. player[3], team[3]
            };
         };
         using flags_type = std::underlying_type_t<flags::type>;
         //
         QString    name;
         QString    desc;
         flags_type flags = 0;
         //
         load_functor_t        load        = nullptr;
         postprocess_functor_t postprocess = nullptr;
         decode_functor_t      to_english  = nullptr;
         decode_functor_t      decompile   = nullptr;
         compile_functor_t     compile     = nullptr;
         //
         OpcodeArgTypeinfo(
            QString name,
            QString desc,
            uint32_t f,
            load_functor_t        l,
            postprocess_functor_t p,
            decode_functor_t      e,
            decode_functor_t      d,
            compile_functor_t     c = nullptr // optional for now so that we can add these incrementally once we begin actually implementing a compiler
         )
            : name(name), desc(desc), flags(f), load(l), postprocess(p), to_english(e), decompile(d), compile(c)
         {}
   };
   
   class OpcodeArgValue : public virtual IGameVariantDataObjectNeedingPostprocess {
      //
      // This class stores only the raw bits that represent the argument value, and a reference to a "typeinfo" 
      // object. The typeinfo object is capable of loading raw bits from a file, compiling a script argument into 
      // raw bits, and decompiling raw bits into a script argument.
      //
      // The naive approach to handling the different opcode argument types would be to have one class per type, 
      // decoding the argument values into nice, orderly little structs. For example, the vector3 type would have 
      // a matching class with x, y, and z fields. The problem with this is that because the compiler needs to 
      // have the typeinfo instances, we'd end up with a class and an instance per argument type -- just a ton of 
      // code and confusingly-overlapping objects.
      //
      // With the approach we're taking here, we have a single class for loaded opcode arguments, and we rely on 
      // the typeinfo to interact with the data. You could think of typeinfos as "inside-out classes," where non-
      // member functions are used to access fields on the value; or you could think of values as "inside-out 
      // unions," where the value consists of undifferentiated data, but it clearly indicates its type and you 
      // must refer to some other system to then read that data.
      //
      // ========================================================================================================
      //
      // NOTES:
      //
      // --------------------------------------------------------------------------------------------------------
      //
      // Typeinfo functors don't pass references to OpcodeArgValue instances; rather, they pass references to the 
      // bit-buffer and the relevant-objects lists inside of OpcodeArgValue instances. The reason for this is so 
      // that functors can be interdependent, in the case where one type includes another type. A "shape"-type 
      // argument, for example, consists of an enum (indicating whether the shape is a box, sphere, cylinder, or 
      // none) followed by zero or more number variables -- as in, the exact same data as is used for a number 
      // variable argument. As such, the functors for "shape"-type arguments need to be able to call the functors 
      // for number-variable arguments.
      //
      // Broadly speaking, "nested" types such as these will work as follows:
      //
      //  - If a type can refer to indexed-refcountable objects via its (relevant_objects) array, then functors' 
      //    (fragment_specifier)-type argument is used to indicate the first index in that array that is avail-
      //    able to the functor. The "outer" type's functor will want to pass a fragment specifier with the app-
      //    ropriate index to the "inner" type's functor.
      //
      //     = The "outer" type's functor should always receive an obj_index of zero.
      //
      //  - The "decode" functors work by reading the already-loaded data and producing some result, e.g. plain-
      //    English representations of the argument or decompiled script code for the argument. These functors 
      //    need to know where in the binary data to start reading from; the fragment specifier's (bit_offset) 
      //    field can be used to indicate this.
      //
      //     = The "outer" type's functor should always receive a bit_offset of zero.
      //
      //  - The "outer" type's functor is not, as of this writing, required to ferry any other fragment-specifier 
      //    state to "inner" types' functors.
      //
      // --------------------------------------------------------------------------------------------------------
      //
      // Sometimes, a single argument may be represented as multiple arguments in script. A "shape"-type argument 
      // for example would have each of its constituent parts (the shape type and the number variables) written 
      // in script code as multiple arguments, e.g. this script code which invokes an opcode that has one object 
      // variable argument and one shape argument:
      //
      //    current_object.set_shape(box, 5, 2, 7, 3)
      //
      // To effect this, an argument type's "compile" functor will be called multiple times -- specifically, once 
      // to start with, and then once more every time it returns (arg_consume_result::still_hungry). When the 
      // compile functor is invoked directly by the compiler, the fragment-specifier argument's (which_arg) field 
      // will refer to the number of times that functor has been called for that argument. Looking at our example 
      // above, the shape-type compile functor would be on which_arg 0 when processing the shape type (box), 
      // which_arg 1 when processing the first size (5), which_arg 2 when processing the second size (2), 
      // which_arg 3 when processing the third size (7), and which_arg 4 when processing the fourth size (3). If 
      // that opcode had a second shape argument immediately following the first, then when we process the second 
      // shape's type enum we would use which_arg 0 again.
      //
      // IMPLEMENTATION DETAILS -- MOVE THIS WHEN THE COMPILER IS IMPLEMENTED: The compiler will maintain a local 
      // variable serving as a counter; this counter will be passed as the fragment-specifier which_arg to 
      // functors. It is initialized to 0, incremented when a functor returns (arg_consume_result::still_hungry), 
      // and reset to 0 instead whenever a functor returns (arg_consume_result::success).
      //
      // --------------------------------------------------------------------------------------------------------
      //
      // We use 128 bits' worth of storage inside of each opcode argument. This is enough to hold the binary data 
      // for the longest possible argument: a "shape" argument with the "box" type, using a constant integer for 
      // each of its dimensions. (That's two bits for the shape type, followed by four number variables; a number 
      // variable uses a six-bit type enum; constant integers can store 16-bit values; so that's 22 bits per box 
      // size value, for a grand total of 90 bits for the entire shape argument.)
      //
      //  - It's tempting to want to heap-allocate the binary data if it has to be 128 bits, so that we're not 
      //    using so many extra bits for argument types that will never be larger than just a few bits. However, 
      //    if we heap-allocate the binary data, then we need a pointer, which burns 8 bytes in addition to the 
      //    binary data itself; moreover, allocated memory is usually prefixed with heap-related bookkeeping 
      //    information, so the memory usage incurred by an allocation will be even higher. Inlining a 128-bit 
      //    buffer inside of the OpcodeArgValue instance really is the cheapest way to ensure we have room for 
      //    all argument types.
      //
      // --------------------------------------------------------------------------------------------------------
      //
      // The (relevant_objects) array holds references to indexed-refcountable objects that the opcode argument 
      // refers to, such as scripted player traits and Forge labels; this allows the rest of the program to know 
      // which such objects are in use by the gametype script, and it allows the gametype script to properly 
      // respond when these objects are reordered within their containing lists.
      //
      // Each entry in (relevant_objects) consists of a pointer and a bitrange (start offset and bitcount) which 
      // indicates which bits in the loaded data specify the index of the target object. When the opcode argument 
      // needs to be written back to a game variant file, the pointer is used to find the indexed-refcountable 
      // object's current index within its containing list, and the bitrange is used to know what bits in the 
      // opcode argument data need to be fixed up -- that is, what bits need to be overwritten with the object's 
      // index at the time of saving.
      // 
      // The (relevant_objects) array is only guaranteed to be suitable for reference-tracking. Specific argument 
      // types may or may not assume that particular indices in this array line up with particular values. The 
      // "player traits" type, for example, assumes that the 0th array element is always the trait-set, and it 
      // uses this information both for stringification and for saving; however, it's not required that indices 
      // "line up" with anything.
      // 
      // In other words: whether it's "kosher" to actually use the pointers in (relevant_objects) will vary 
      // between different argument types, so you probably shouldn't touch it from outside the typeinfo functors.
      //
      // IMPLEMENTATION DETAILS -- MOVE THIS WHEN RESAVING IS IMPLEMENTED: The (do_index_fixup) member function 
      // must be called early in the save process, before writing any opcode argument binary data to the target 
      // file.
      //
      public:
         virtual bool postprocess(GameVariantData* data) override { // IGameVariantDataObjectNeedingPostprocess
            return (this->type.postprocess)(0, this->data, this->relevant_objects, data);
         }
         cobb::bitarray128  data;
         OpcodeArgTypeinfo& type;
         arg_rel_obj_list_t relevant_objects; // for when this argument refers to something that needs to be reference-tracked, like a Forge label or script option, so other systems can tell whether that something is in use by the script
         QString            english; // for debugging
         //
         using bit_storage_type = decltype(data)::storage_type;
         //
         void do_index_fixup();
         //
         inline static uint64_t excerpt_loaded_index(cobb::bitarray128& data, arg_rel_obj_list_t& ro, uint8_t which) noexcept {
            auto& item = ro[which];
            if (!item.count)
               return 0;
            return data.excerpt(item.start, item.count);
         }
   };
}