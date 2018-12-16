#include <thread>
#include <algorithm>
#include <vector>
#include <deque>
#include <random>
#include <utility>
#include <memory>
#include <cstdint>
//#include <fstream>

using vecsize_t = ::std::uint64_t;
using veciter_t = ::std::uint32_t *;

void gen_and_sort_section(const veciter_t &begin, const veciter_t &end, int seed)
{
   ::std::minstd_rand randgen(seed);
   ::std::generate(begin, end, randgen);
   ::std::sort(begin, end);
}

void merge_sections(const veciter_t &begin,
                    const veciter_t &middle,
                    const veciter_t &end,
                    ::std::thread thr1,
                    ::std::thread thr2)
{
   thr1.join();
   thr2.join();
   ::std::inplace_merge(begin, middle, end);
}

struct sort_thread {
   ::std::thread thr;
   const int part_begin;
   const int part_end;

   sort_thread(decltype(gen_and_sort_section) &func,
               const veciter_t &begin,
               const veciter_t &end,
               int seed,
               int part_begin,
               int part_end)
        : thr(func, begin, end, seed), part_begin(part_begin), part_end(part_end)
   {
   }
   sort_thread(decltype(merge_sections) &func,
               const veciter_t &begin,
               const veciter_t &middle,
               const veciter_t &end,
               ::std::thread thr1,
               ::std::thread thr2,
               int part_begin,
               int part_end)
        : thr(func, begin, middle, end, ::std::move(thr1), ::std::move(thr2)),
          part_begin(part_begin), part_end(part_end)
   {
   }
};

int main()
{
   using ::std::thread;
   using ::std::unique_ptr;
   using ::std::make_unique;
   using ::std::uint32_t;

   constexpr auto size = 1ULL << 31;
   auto huge_array = make_unique<uint32_t[]>(size);
   const int cpucount = ::std::max(thread::hardware_concurrency() / 2, 1U);
   const int sections = cpucount;
   const int secmarkers = sections + 1;
   auto partitions = make_unique<veciter_t[]>(secmarkers);

   partitions[0] = &(huge_array[0]);
   for (int i = 1; i < secmarkers; ++i) {
      auto const prev_part = partitions[i - 1];
      auto const remaining = secmarkers - i;
      partitions[i] = prev_part + (&(huge_array[size]) - prev_part) / remaining;
   }
   if (partitions[secmarkers - 1] != &(huge_array[size])) {
      return 1;
   }

   ::std::random_device seedmaker;
   ::std::deque<sort_thread> rthreads;
   for (int i = 0; i < sections; ++i)
   {
      rthreads.emplace_back(
         gen_and_sort_section, partitions[i], partitions[i + 1], seedmaker(),
         i, i + 1
         );
   }
   while (rthreads.size() >= 2) {
      if (rthreads.front().part_end >= sections) {
         rthreads.emplace_back(::std::move(rthreads.front()));
         rthreads.pop_front();
      } else {
         auto thr1{::std::move(rthreads.front())};
         rthreads.pop_front();
         auto thr2{::std::move(rthreads.front())};
         rthreads.pop_front();
         if (thr1.part_end != thr2.part_begin) {
            return 2;
         } else {
            rthreads.emplace_back(merge_sections,
                                  partitions[thr1.part_begin],
                                  partitions[thr1.part_end],
                                  partitions[thr2.part_end],
                                  ::std::move(thr1.thr),
                                  ::std::move(thr2.thr),
                                  thr1.part_begin,
                                  thr2.part_end);
         }
      }
   }
   if (rthreads.front().part_begin != 0) {
      return 3;
   } else if (rthreads.front().part_end != sections) {
      return 4;
   } else {
      rthreads.front().thr.join();
   }
   /*
   {
      ::std::ofstream of("/proc/self/fd/1");

      of << huge_array[0] << '\n';
      for (vecsize_t i = 1; i < huge_array.size(); ++i) {
         if (huge_array[i] < huge_array[i - 1]) {
            return 5;
         }
         of << huge_array[i] << '\n';
      }
   }
   */
   return 0;
}
