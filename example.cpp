#include "include/IndexAllocator.hpp"

int main()
{
     // Create allocator
     aZero::DS::IndexAllocator<uint64_t> allocator;

     // Allocate index (0) to "a"
     aZero::DS::IndexAllocator<uint64_t>::Index a = allocator.Allocate();

     // Allocate index (1) to "b"
     auto b = allocator.Allocate();

     a = std::move(b);

     // Free index assigned to "a" (0)
     allocator.Free(a);

     // Check if "a" is valid, which it isn't after ::Free()
     if (a == aZero::DS::IndexAllocator<uint64_t>::InvalidIndex)
     {
         std::cout << "'a' is invalid using the equality operator\n";
     }

     if (!a.IsValid())
     {
         std::cout << "'a' is invalid using ::IsValid()\n";
     }
     //

     // Double-free index assigned to "a" (0) which results in an early
return
     allocator.Free(a);

     // Free index assigned to "b" (1)
     allocator.Free(b);

     // Allocate the previously freed index (0) to "c"
     auto c = allocator.Allocate();

     // Free index assigned to "c" (0)
     allocator.Free(c);

     std::cout << "Current total allocated indices: " <<
allocator.GetCurrentMax() << "\n";

     std::cout << "Current number of indices that have been allocated
but then recycled: " << allocator.GetFreeCount() << "\n";

     return 0;
}
