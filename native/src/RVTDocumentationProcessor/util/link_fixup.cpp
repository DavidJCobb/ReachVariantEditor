#include "link_fixup.h"

namespace {
   bool _is_relative_path(const QString& path) {
      if (path[0] == '/' || path[0] == '\\')
         return false;
      if (QStringRef(&path, 0, 7).compare(QString("script/")) == 0) // hack to avoid having to edit 90% of the documentation files again
         return false;
      return true;
   }
}

namespace util {
   extern void link_fixup(const QString& stem, QString& out) {
      if (out.startsWith("http"))
         return;
      //
      // Really hacky link/image fixup code:
      //
      QString hash;
      {
         auto h = out.lastIndexOf('#');
         if (h != -1) {
            hash = out.mid(h, out.size() - h);
            out = out.mid(0, h);
         }
      }
      if (_is_relative_path(out)) {
         //
         // Articles typically use hyperlink and image paths that are relative to themselves, so 
         // we need to fix those up since we're using a <base/> element to change relative paths 
         // (so that the nav and asset paths can work).
         //
         out = stem + out;
      } else if (out[0] == '/' || out[0] == '\\') {
         out = out.mid(1);
      }
      //
      // Links between articles usually lack file extensions.
      //
      auto a = out.lastIndexOf(".");
      auto b = out.lastIndexOf("/");
      if (a == -1 || (a < b && b != -1)) {
         out += ".html";
      }
      out += hash;
   }
}