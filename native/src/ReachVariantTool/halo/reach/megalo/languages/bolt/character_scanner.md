# `character_scanner`

## What problem does this struct solve?

Typically, to iterate over a string's contents and parse it, you'd use a 
for loop.

```c++
for(size_t i = 0; i < text.size(); ++i) {
   auto c = text[i];
   //
   // ...
}
```

If you want to implement some sort of object to manage scanning a string and extracting a variety of tokens, having that object use a for loop would be the intuitively obvious choice.

```c++
struct my_scanner {
   QString text;
   size_t  pos;

   void scan() {
      for(; pos < text.size(); ++pos) {
         auto c = text[pos];
         //
         // ...
      }
   }
};
```

You could then split off the logic for extracting different kinds of tokens, writing different member functions for each.

```c++
//
// We'll define a "word" as an uninterrupted set of alphabetical glyphs.
//
QString my_scanner::extract_word() {
   auto    prior = this->pos;
   QString out;

   for(; this->pos < this->text.size(); ++this->pos) {
      auto c = this->text[this->pos];
      if (c.isAlpha()) {
         out += c;
         continue;
      }
      break;
   }
                  
   return out;
}

//
// Let's have a function for numbers, too.
//
QString my_scanner::extract_integer() {
   auto    prior = this->pos;
   QString out;

   for(; this->pos < this->text.size(); ++this->pos) {
      auto c = this->text[this->pos];
      if (c.isDigit()) {
         out += c;
         continue;
      }
      break;
   }
                  
   return out;
}
```

That seems like a pretty good approach, right? But there's a subtle flaw. What happens if you have multiple kinds of tokens, and they appear right next to each other in the string?

Let's say you have a string consisting of words and numbers only, and you want to split the string into an ordered list of words and numbers. You might have your outer scan function do something like this:

```c++
QStringList my_scanner::extract_chunks() {
   QStringList chunks;

   for(; this->pos < this->text.size(); ++this->pos) {
      auto number = this->extract_integer();
      if (!number.isEmpty()) {
         chunks.append(number);
         continue;
      }
      //
      auto word = this->extract_word();
      assert(!word.isEmpty()); // unexpected symbol found?
      chunks.append(word);
   }

   return chunks;
}
```

So let's say you run that on the string `abc1def234ghi5`. The expected `chunks` would be

* `"abc"`
* `"1"`
* `"def"`
* `"234"`
* `"ghi"`
* `"5"`

Instead, however, you get this:

* `"abc"`
* `"def"`
* `"34"`
* `"hi"`

The characters following the end of a token are being skipped. The reason is the presence of nested loops. Let's step through the logic.

* The outer loop starts at the beginning of the string.
* We call `extract_word`. That function loops forward and stops upon encountering `'1'`, extracting `"abc"`. So, `text[pos] == '1'`.
* We commit the extracted word to our `chunks` list, and then `continue` to the next iteration...
* ...which causes `pos` to be incremented. We've skipped the `'1'`!

## How do we solve it?

Well, before we talk about the solution, it'll probably be easier to think in different terms.

## Functors instead of loops

Instead of *loops*, let's think in terms of *functors*.

```c++
struct my_scanner {
   QString text;
   size_t  pos;

   // Functors can return true to break, or false to continue.
   using functor_type = std::function<bool(QChar)>;

   void scan(functor_type functor) {
      for(; pos < text.size(); ++pos) {
         auto c = text[pos];
         if (functor(c))
            break;
      }
   }
};
```

With this approach, the other functions might look like this:

```c++
QString my_scanner::extract_word() {
   auto    prior = this->pos;
   QString out;

   this->scan([&out](QChar c) -> bool {
      if (c.isAlpha()) {
         out += c;
         return false; // continue
      }
      return true; // break
   });
                  
   return out;
}

QStringList my_scanner::extract_chunks() {
   QStringList chunks;

   this->scan([&chunks](QChar c) -> bool {
      auto number = this->extract_integer();
      if (!number.isEmpty()) {
         chunks.append(number);
         return false; // continue
      }
      //
      auto word = this->extract_word();
      assert(!word.isEmpty()); // unexpected symbol found?
      chunks.append(word);
     return false; // continue
   });
}
```

This code is functionally equivalent to the simpler version above, with for loops, and it has the exact same problems. There are some new things you can do with this code; for example, you could have the `scan` function handle more advanced logic (e.g. keeping track of line numbers; skipping content in code comments) without the functions for individual tokens needing to replicate that logic. More important to this discussion is that putting things in terms of lambdas allows us to abstract ourselves away from the underlying implementation, a bit.

Let's define a new concept: *consuming* parts of the string. To *consume* a part of the string is to advance the stream position (presuambly while reading the characters you're moving past), and in this case, that can only be done by running a character scan functor at least twice. The functors themselves don't consume anything; they are what is *used* to consume.

We can think of `extract_word` and `extract_integer` as consuming tokens (well-defined, non-context-dependent character patterns) within a string. We can also think of the character scan functor inside of `extract_chunks` as consuming characters, because it calls to `extract_word` and `extract_integer` &mdash; it runs nested character scans.

## Okay, so *now* how do we solve the problem?

First, we start by codifying the behavior of our functors. If a functor is used to consume glyphs, e.g. some sort of token, then it must stop iteration at the end of its token. The `extract_word` function must stop iteration at the end of the word, and the `extract_integer` function must stop iteration at the end of the number.

Of course, they already do that. The changes we need to make are to the scanning mechanism itself.

```c++
struct my_scanner {
   QString text;
   size_t  pos;

   // Functors can return true to break, or false to continue.
   using functor_type = std::function<bool(QChar)>;

   void scan(functor_type functor) {
      for(; pos < text.size(); ++pos) {
         auto c     = text[pos];
         auto prior = pos;
         if (functor(c))
            break;
         if (prior != pos) {
            //
            // The functor has consumed one or more glyphs and stopped 
            // at the end of the consumed token (i.e. it has run some 
            // sort of nested character scan; there's no other way it 
            // could have advanced the stream position on its own). 
            // If we take no further action, then incrementing (pos) 
            // will skip the character after the consumed token.
            //
            // Let's rewind the stream by one, so that when our loop 
            // hits its next iteration and we advance the stream by 
            // one, we'll read the character our functor stopped at 
            // after consuming content, i.e. the character after the 
            // consumed token.
            //
            --pos;
         }
      }
   }
};
```

With this change, our `extract_chunks` function produces the expected result. The `extract_chunks` function itself can be said to consume the whole string.