#include <cstdlib>
#include "SharedPtr.h"
#include <cassert> 
#include <utility>

struct TestObject {
   int value;
};

int main() {

   SharedPtr<int> ptr(new int(42));

   assert(ptr);
   assert(ptr.get() != nullptr);
   assert(*ptr == 42);
   assert(ptr.useCount() == 1);

   SharedPtr<int> copy(ptr);
   assert(ptr.useCount() == 2);
   assert(copy.useCount() == 2);

   SharedPtr<int> destination(new int(10));
   SharedPtr<int> source(new int(20));

   destination = std::move(source);

   assert(*destination == 20);
   assert(destination.useCount() == 1);
   assert(!source);
   assert(source.get() == nullptr);

   SharedPtr<int> resetPtr(new int(10));

   resetPtr.reset();
   assert(!resetPtr);
   assert(resetPtr.get() == nullptr);
   assert(resetPtr.useCount() == 0);

   resetPtr.reset(new int(30));
   assert(resetPtr);
   assert(*resetPtr == 30);
   assert(resetPtr.useCount() == 1);

   resetPtr.reset(resetPtr.get());

   assert(resetPtr);
   assert(*resetPtr == 30);
   assert(resetPtr.useCount() == 1);

   SharedPtr<int> made = makeSharedBasic<int>(50);
   assert(made);
   assert(*made == 50);
   assert(made.useCount() == 1);

   SharedPtr<int> first(new int(1));
   SharedPtr<int> second(new int(2));
   assert(*first == 1);
   assert(*second == 2);
   first.swap(second);
   assert(*first == 2);
   assert(*second == 1);

   SharedPtr<TestObject> object(new TestObject{7});
   assert(object->value == 7);

   SharedPtr<int> assigned;
   assigned = ptr;
   assert(assigned == ptr);
   assert(assigned.useCount() == 3);

   SharedPtr<int> moveSource(new int(60));
   SharedPtr<int> moveTarget(std::move(moveSource));
   assert(*moveTarget == 60);
   assert(!moveSource);
   assert(moveTarget.useCount() == 1);

   SharedPtr<int> same(first);
   assert(same == first);
   assert(same.useCount() == 2);



   return EXIT_SUCCESS;
}
