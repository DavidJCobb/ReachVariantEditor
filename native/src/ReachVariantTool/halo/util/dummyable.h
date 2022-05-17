#pragma once

namespace halo::util {
   //
   // Halo: Reach's game variant files are not very well organized. It's common for data items to refer 
   // to the contents of indexed lists that are placed later in the file, i.e. "I want to use list item 
   // #2 in this list that hasn't loaded yet." This creates some complications for us, because we can't 
   // actually retain these indices: we need to use refcounted pointers, both so that we can allow an 
   // editor to rearrange lists, and so that we can prevent an editor from deleting list items that are 
   // in use by the script.
   // 
   // There are two ways we can address this situation.
   // 
   //  - Load all of the data, retaining indices temporarily, and then run a second pass. In the second 
   //    pass, we post-process the loaded data, looping over everything that can refer to later-loading 
   //    data to swap indices out for pointers.
   // 
   //  - Before loading data, pre-fill all indexed lists with dummy elements. When loading, repurpose 
   //    the dummy elements already present and turn them into non-dummy elements. Then, after loading, 
   //    post-process the loaded data by deleting all trailing dummy elements.
   // 
   // The latter approach allows for a simpler postprocess step, and also allows us to handle the case 
   // of an invalid file where earlier-loading data refers to non-existent items in later-loading lists.
   //
   class dummyable {
      public:
         bool is_defined = false;
   };
}