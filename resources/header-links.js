(function() {
   let node = document.querySelector("main");
   if (!node)
      return;
   let filename = window.location.pathname.substring(window.location.pathname.lastIndexOf(/\\\//) + 1);
   let headers  = node.querySelectorAll("h2,h3");
   for(let header of headers) {
      let node = header.firstElementChild;
      if (!node)
         continue;
      if (node.nodeName[0].toLowerCase() == "a" && node.hasAttribute("name") && !node.hasAttribute("href")) {
         let link = document.createElement("a");
         link.setAttribute("href", filename + "#" + node.getAttribute("name"));
         link.setAttribute("title", "permalink");
         link.classList.add("header-permalink");
         header.appendChild(link);
      }
   }
})();