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
})();