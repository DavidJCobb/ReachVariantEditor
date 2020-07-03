(function() {
   let node  = document.querySelector("#sidebar>ul");
   let items = node.querySelectorAll("li");
   let found = null;
   for(let item of items) {
      if (!found) {
         //
         // If a sidebar item links to the page we're already on, identify 
         // it with a CSS class so we can style it differently.
         //
         let link = item.firstElementChild;
         if (link.href == document.location.href) {
            link.classList.add("you-are-here");
            found = item;
         }
      }
      if (item.hasAttribute("data-disallow-collapse"))
         continue;
      //
      // Set up expand/collapse buttons:
      //
      let sub = item.querySelector("ul");
      if (item.hasAttribute("data-delete-if-empty")) {
         if (!sub || !sub.children.length) {
            item.parentNode.removeChild(item);
            continue;
         }
      }
      if (!sub)
         continue;
      let toggle = document.createElement("a");
      toggle.classList.add("toggle");
      item.insertBefore(toggle, item.children[0]);
   }
   node.addEventListener("click", function(e) {
      //
      // Click handler for expand/collapse buttons.
      //
      let toggle = e.target.closest(".toggle");
      if (!toggle)
         return true;
      e.preventDefault();
      let item = toggle.parentNode;
      item.classList.toggle("collapsed");
   });
   //
   if (window.page_load_start_time && found) {
      if (found.parentNode.closest("li") || found.querySelector("ul")) {
         //
         // Auto-collapse sidebar navigation elements that we're not "in," 
         // but only if the following conditions are met:
         //
         //  - The page loaded quickly.
         //  - We know where in the nav we are.
         //  - The place we're at is a child, OR it has children.
         //
         let done = Date.now();
         if (done - window.page_load_start_time < 1000) {
            for(let item of items) {
               if (item == found)
                  continue;
               if (item.hasAttribute("data-dont-default-collapse"))
                  continue;
               if (item.hasAttribute("data-disallow-collapse"))
                  continue;
               let sub = item.querySelector("ul");
               if (!sub)
                  continue;
               if (item.contains(found) || found.contains(item))
                  continue;
               item.classList.add("collapsed");
            }
         }
      }
   }
})();