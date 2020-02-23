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
   };

   enum class arg_consume_result {
      failure,
      success,
      still_hungry
   };
   class OpcodeArgTypeinfo {
      //
      // TODO: WE NEED TO SPEC OUT INDEX-FIXUP BEHAVIOR. Some opcode arguments rely on their relevant-
      // objects lists to resave their values back to the game variant file. This is mainly the case when 
      // dealing with objects that exist inside of indexed lists and can be reordered, such as player 
      // traits and script options: the opcode argument holds a pointer to the object so that when it 
      // comes time to resave, we can use the index that the object has at the time of saving.
      //
      // If we blindly just echo out the originally loaded bits, then we'll break any opcode that refers 
      // to any object in an indexed list that's been reordered. Accordingly, we've revised how we store 
      // relevant-objects (and we should probably rename the field to "indexed_objects") such that each 
      // pointer is accompanied by a bitrange indicating what portion of the loaded bits specifies the 
      // index of the object (i.e. start bit offset and number of bits). This approach means that we don't 
      // need per-typeinfo functors to do index fixup; the info needed for a generic approach is already 
      // there. We just need to write that generic approach.
      //
      // TODO: Below, we point out that for nested types and decode functors, the containing type's functor 
      // needs to copy the bit-array and shift it before invoking the contained type's functor, since the 
      // latter functor will read from the start of the bit-array. If we modify the (fragment) argument on 
      // the functors, however, then this may become unnecessary. We're passing a whole argument so we have 
      // the size of an x64 register (i.e. 8 bytes) to work with, so what if we make the (fragment) a struct 
      // consisting of: an index into the relevant-objects array; and a bit offset? Then, a containing type's 
      // functor could use the bit-offset in the fragment to tell a contained type's functor, "Hey, read our 
      // saved bits starting from HERE." The only difficulty is that the decode functor offers no way to know 
      // how many bits were read (picture a shape functor invoking the number-variable functor: if it has 
      // more than one number, how does it know what offset to pass to the second number-variable?); we may 
      // be able to address this by having decode_functor_t return a signed integer indicating the number of 
      // bits read (with negative values serving as sentinels and failure codes).
      //
      //  - The bit-offset would of course be 0 by default, and would be unused in basically every other 
      //    situation, including when the functors are invoked on the containing type.
      //
      // NOTE: When we implement compile functors, we need to give opcodes a way to access data on the 
      // containing game variant, i.e. when we're compiling an opcode that refers to a script option, Forge 
      // label, etc., and they need to get a refcounted pointer to the target data.
      //
      public:
         using load_functor_t        = std::function<bool(uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits)>; // loads data from binary stream; returns success/failure
         using decode_functor_t      = std::function<bool(uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out)>; // returns success/failure
         using compile_functor_t     = std::function<arg_consume_result(uint8_t fragment, OpcodeArgValue&, arg_rel_obj_list_t& relObjs, const std::string&, Compiler&)>;
         using postprocess_functor_t = std::function<bool(uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData*)>;
         //
         static bool default_postprocess_functor(uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData*) { return true; } // for argument types that don't need postprocessing
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
      // the typeinfo to interact with the data. You could think of the class as a sort of inside-out union type, 
      // I suppose. A union, being untyped data which can have any of a fixed set of types but which itself has 
      // no indicator as to which type it currently is, is a value that knows how it can be read but not how it 
      // should be read; it knows how it "can" be read in that when you access it as a given type, that type's 
      // behaviors come built-in; yet it doesn't know how it "should" be read. An OpcodeArgValue, by contrast, 
      // is untyped data that doesn't know how it can be read, but it knows where to find a capable reader and 
      // by virtue of knowing that, it knows how it should be read.
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
      //  - If a type can refer to reference-tracked objects, then the functors' "fragment" argument is used to 
      //    indicate what slot in the (relObjs) array is available to that functor.
      //
      //  - Output functors -- that is, functors which read already-loaded data and produce some result, i.e. the 
      //    functors for decompiling -- will read from the start of the bit-buffer. This means that in the case 
      //    of "nested" types, the "containing" type's functor will need to copy the loaded data and shift it, 
      //    and provide the shifted copy to the "contained" type's functor.
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
      // compile functor is invoked directly by the compiler, the (fragment) argument will refer to the number of 
      // times it has been called for that argument. Looking at our example above, the shape-type compile functor 
      // would be on fragment 0 when processing the shape type (box), fragment 1 when processing the first size 
      // (5), fragment 2 when processing the second size (2), fragment 3 when processing the third size (7), and 
      // fragment 4 when processing the fourth size (3). If that opcode had a second shape argument immediately 
      // following the first, then when we process the second shape's type enum we would use fragment 0 again.
      //
      // Of course, when dealing with "nested" types, a "containing" type's compile functor may invoke a "cont-
      // ained" type's compile functor and use the (fragment) argument as described above.
      //
      // IMPLEMENTATION DETAILS -- MOVE THIS WHEN THE COMPILER IS IMPLEMENTED: The compiler will maintain a local 
      // variable serving as a counter; this counter will be passed as the (fragment) argument to functors. It is 
      // initialized to 0, incremented when a functor returns (arg_consume_result::still_hungry), and reset to 0 
      // instead whenever a functor returns (arg_consume_result::success).
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
   };
}