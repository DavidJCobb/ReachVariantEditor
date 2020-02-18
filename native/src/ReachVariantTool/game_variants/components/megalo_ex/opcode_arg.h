#pragma once
#include "../../base.h"
#include "../../../helpers/bitvector.h"
#include "../../../helpers/reference_tracked_object.h"
#include <functional>
#include <QString>

namespace MegaloEx {
   class Compiler;

   class OpcodeArgValue;

   enum class arg_consume_result {
      failure,
      success,
      still_hungry
   };
   class OpcodeArgTypeinfo {
      public:
         using load_functor_t        = std::function<bool(OpcodeArgValue&, uint64_t bits)>; // loads data from binary stream; returns success/failure
         using decode_functor_t      = std::function<bool(OpcodeArgValue&, std::string&)>; // returns success/failure
         using compile_functor_t     = std::function<arg_consume_result(OpcodeArgValue&, const std::string&, Compiler&)>;
         using postprocess_functor_t = std::function<bool(OpcodeArgValue&, GameVariantData*)>;
         //
         static bool default_postprocess_functor(OpcodeArgValue&, GameVariantData*) { return true; } // for argument types that don't need postprocessing
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
   
   class OpcodeArgValue : public virtual IGameVariantDataObjectNeedingPostprocess, cobb::reference_tracked_object {
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
      // I suppose. The data is essentially untyped, but you need an external force (the typeinfo) to work with 
      // it, rather than the data itself being accessible "as" a type.
      //
      public:
         virtual bool postprocess(GameVariantData* data) override { // IGameVariantDataObjectNeedingPostprocess
            return (this->type.postprocess)(*this, data);
         }
         //
         // TODO: We need to redesign the typeinfo functors. Some argument types include other argument types -- 
         // for example, shapes can include up to four number variables -- so functors need to be able to call 
         // each other. If the functors have to take an OpcodeArgValue&, however, then that isn't strictly 
         // possible. So here's what we do instead:
         //
         //  - The functors take a reference to the raw binary data inside of the OpcodeArgValue, instead of to 
         //    the OpcodeArgValue itself.
         //
         //     - No no no, wait. We still need to deal with cobb::reference_tracked_object. We either need to 
         //       pass the (relevant_objects) list as an argument too, or rethink that whole system. Passing 
         //       the list would be simpler.
         //
         //        - (We need to pass a *reference to* the list, given how reference_tracked_object::ref works.)
         //
         //     - When dealing with "nested argument types," like shapes, the outermost argument type can then 
         //       invoke the inner ones by creating a second bit-buffer, copying the remaining bits into that, 
         //       passing that to an inner type's functor, and then retrieving the contents; lather, rinse, 
         //       and repeat for as many inner arguments exist.
         //
         //  - We remove OpcodeArgValue::compile_step (which was intended to accommodate argument types that 
         //    exist as single arguments internally while mapping to arguments in script) and make it so that 
         //    the compiler itself keeps track of how many times it's calling the same compile functor for an 
         //    argument. Specifically, the code for compiling a function call should have a uint8_t counter 
         //    that is initialized to zero before the arguments loop; whenever any compile functor returns the 
         //    arg_consume_result::success result code, the counter is reset to zero; otherwise the counter is 
         //    incremented; and that counter is passed as an argument to the compile functor.
         //
         //     - As an example, the "Set Object Shape" action has two arguments: the object to act on and the 
         //       shape to set. Consider setting a box shape on an object, then. First, we call the compile 
         //       functor for the first script argument, the object variable, passing a counter of 0. That 
         //       signals a successful completion, so we reset the counter to 0 instead of incrementing it. 
         //       Then, we call the compile functor for the second script argument, the shape type, passing 
         //       a counter of 0. That returns arg_consume_result::still_hungry, so we increment the counter, 
         //       move onto the next script argument (the box width), and call the compile functor with a 
         //       counter of 1. We'll end up calling that same compile functor on that same argument three 
         //       more times, with counters of 2, 3, and 4.
         //
         // TODO: We need to expand the binary data from 64 bits to 128 bits; similarly, the load functor needs 
         // to receive 128 bits peeked from the bitstream. While we're at it, let's think of a more fitting name 
         // than "bitvector64," which wrongly implies that the size of the underlying storage can be expanded.
         //
         //  - It's tempting to want to heap-allocate the binary data if it has to be 128 bits, so that we're 
         //    not using so many extra bits for argument types that will never be larger than just a few bits. 
         //    However, if we heap-allocate the binary data, then we need a pointer, which burns 8 bytes in 
         //    addition to the binary data itself; moreover, allocated memory is usually prefixed with heap-
         //    related bookkeeping information, so the memory usage incurred by an allocation will be even 
         //    higher. Inlining a 128-bit buffer inside of the OpcodeArgValue instance really is the cheapest 
         //    way to ensure we have room for all argument types.
         //
         //  - "Shape" arguments have a two-bit type enum followed by up to four number avriables; the largest 
         //    possible number variable is an integer constant, which takes 22 bits (the six-bit scope enum 
         //    followed by a 16-bit signed integer); so the maximum bitcount for a shape argument is 90 bits.
         //
         cobb::bitvector64  data;
         OpcodeArgTypeinfo& type;
         uint8_t            compile_step = 0;
         QString            english; // for debugging
         ref<cobb::reference_tracked_object> relevant_objects[4]; // for when this argument refers to something that needs to be reference-tracked, like a Forge label or script option, so other systems can tell whether that something is in use by the script
         //
         using bit_storage_type = decltype(data)::storage_type;

         //
         // NOTES:
         //
         //  - The (relevant_objects) array ONLY exists for reference-tracking. Specific argument types may or 
         //    may not assume that particular indices in this array line up with particular values. As an exam-
         //    ple, the "player traits" type assumes that the 0th array element is always the trait-set, but 
         //    the "shape" type can't assume that any given entry in the array corresponds to any given dimens-
         //    ion of the shape (i.e. if you have a box shape, that'll have four dimensions; if all dimensions 
         //    except the third are integer constants and the third is a script option, then the 0th entry in 
         //    the (relevant_objects) list is going to line up with that third dimension).
         //
         //    In other words: whether it's "kosher" to actually use the pointers in (relevant_objects) will 
         //    vary between different argument types, so you probably shouldn't touch it from outside the type-
         //    info functors.
         //
         //    TODO: argument types that guarantee specific objects in specific list indices should note this 
         //    in documentation comments, for maintenance's sake.
         //
   };
}