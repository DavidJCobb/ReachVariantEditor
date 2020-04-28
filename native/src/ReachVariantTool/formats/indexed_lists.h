#pragma once
#include "../helpers/refcounting.h"

class indexed_list_item : public cobb::refcountable {
   //
   // Halo: Reach's game variant files are not very well organized. It's common for data items to refer 
   // to the contents of indexed lists that are placed later in the file, i.e. "I want to use list item 
   // #2 in this list that hasn't loaded yet." This creates some complications for us, because we can't 
   // actually retain these indices: we need to use refcounted pointers, both so that we can allow an 
   // editor to rearrange lists, and so that we can prevent an editor from deleting list items that are 
   // in use by the script.
   //
   // There are two ways we can address the situation, then. The naive approach is "postprocessing:" we 
   // load all of the data, retaining indices temporarily, and then we run a second pass, looping over 
   // everything that can refer to later-loading data so that those objects can grab pointers. However, 
   // there's a more clever approach: at the start of the load process, we can fill all indexed lists 
   // with dummy elements; when it comes time to actually load a list, we simply repurpose the dummy 
   // elements already present and turn them into non-dummy elements; and then we delete all of the 
   // dummy elements. This removes the need for a postprocess step: even if Data A refers to Data B 
   // which loads later, it can still grab a pointer to B.
   //
   // Thus: this base class. It's a variant on cobb::indexed_refcountable which contains a bool value, 
   // (is_defined), which indicates whether the list item is a dummy. It's default-initialized to 
   // false; indexed list items should set their (is_defined) values to (true) themselves during their 
   // load process.
   //
   public:
      int16_t index      = -1;
      bool    is_defined = false;
};