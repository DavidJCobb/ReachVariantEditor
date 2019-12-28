#pragma once
#include "../opcode_arg.h"
#include "scalar.h"

namespace Megalo {
   enum class ShapeType {
      none,
      sphere,
      cylinder,
      box,
      //
      _count
   };
   class OpcodeArgValueShape : public OpcodeArgValueScalar {
      public:
         cobb::bitnumber<cobb::bitcount((int)ShapeType::_count - 1), ShapeType> shapeType = ShapeType::none; // 2 bits
         OpcodeArgValueScalar radius; // or "width"
         OpcodeArgValueScalar length;
         OpcodeArgValueScalar top;
         OpcodeArgValueScalar bottom;
         //
         virtual bool read(cobb::bitreader& stream) noexcept override {
            this->shapeType.read(stream);
            switch (this->shapeType) {
               case ShapeType::sphere:
                  return this->radius.read(stream);
               case ShapeType::cylinder:
                  return (
                     this->radius.read(stream)
                  && this->top.read(stream)
                  && this->bottom.read(stream)
                  );
               case ShapeType::box:
                  return (
                     this->radius.read(stream)
                  && this->length.read(stream)
                  && this->top.read(stream)
                  && this->bottom.read(stream)
                  );
            }
            return false;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            this->shapeType.write(stream);
            switch (this->shapeType) {
               case ShapeType::sphere:
                  this->radius.write(stream);
                  return;
               case ShapeType::cylinder:
                  this->radius.write(stream);
                  this->top.write(stream);
                  this->bottom.write(stream);
                  return;
               case ShapeType::box:
                  this->radius.write(stream);
                  this->length.write(stream);
                  this->top.write(stream);
                  this->bottom.write(stream);
                  return;
            }
         }
         virtual void to_string(std::string& out) const noexcept override {
            std::string temp;
            switch (this->shapeType) {
               case ShapeType::none:
                  out = "none";
                  return;
               case ShapeType::sphere:
                  out = "sphere of radius ";
                  this->radius.to_string(temp);
                  out += temp;
                  return;
               case ShapeType::cylinder:
                  out = "cylinder of radius ";
                  this->radius.to_string(temp);
                  out += temp;
                  out += ", top ";
                  this->top.to_string(temp);
                  out += temp;
                  out += ", and bottom ";
                  this->bottom.to_string(temp);
                  out += temp;
                  return;
               case ShapeType::box:
                  out = "box of width ";
                  this->radius.to_string(temp);
                  out += temp;
                  out += ", length ";
                  this->length.to_string(temp);
                  out += temp;
                  out += ", top ";
                  this->top.to_string(temp);
                  out += temp;
                  out += ", and bottom ";
                  this->bottom.to_string(temp);
                  out += temp;
                  return;
            }
         }
         //
         static OpcodeArgValue* factory(cobb::bitreader& stream) {
            return new OpcodeArgValueShape();
         }
   };
}