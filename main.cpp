#include <iostream>
#include <vector>
#include <utility>

/* Tiny Ranges Implementation by Furkan KIRAC
 * as part of CS409/509 - Advanced C++ Programming course in Ozyegin University
 * Supports transforming and filtering ranges and,
 * to<CONTAINER>() method for eagerly rendering the range into a CONTAINER.
 * Date: 20191222
 */

// predicates
namespace predicates
{
    auto less_than = [](auto threshold) { return [=](auto value) { return value < threshold; }; };
    auto greater_than = [](auto threshold) { return [=](auto value) { return value > threshold; }; };
    auto all_of = [](auto ... preds) { return [=](auto value) { return (preds(value) && ...); }; };
}

namespace actions
{
    auto multiply_by = [](auto coef) { return [=](auto value) { return value * coef; }; };
    auto if_then = [](auto predicate, auto action) { return [=](auto value) { if(predicate(value)) value = action(value); return value; }; };

}

namespace views
{
    auto ints = [](int k=0) { return [k]() mutable { return k++; }; };
    auto odds = []() { return [k=1]() mutable { auto r = k; k += 2; return r; }; };
}

namespace algorithms
{
    // ---[ RANGE implementation
    template<typename Iterator>
    struct Range
    {
        using LazyIterator = Iterator; // required for accessing the used Iterator type from other locations
        Iterator m_begin;
        Iterator m_end;
        auto begin() { return m_begin; }
        auto end() { return m_end; }
        const auto  begin() const  { return m_begin; }
        const auto end() const{ return m_end; }
    };

    template<typename Iterator>
    Range(Iterator, Iterator) -> Range<Iterator>;

    // ---[ TRANSFORM implementation
    template<typename Iterator, typename Callable>
    struct TransformingIterator : Iterator
    {
        using OriginalIterator = Iterator; // required for accessing the used Iterator type from other locations

        Callable callable;

        TransformingIterator(Iterator iterator, Callable callable) : Iterator{iterator}, callable{callable} { }
        Iterator & get_orig_iter() const { return ((Iterator&)*this); }
        double operator*() { return callable(*get_orig_iter()); }
    };

    auto transform = [](auto action) {
        return [=](auto& container) {
            using Container = std::decay_t<decltype(container)>;
            using Iterator = typename Container::iterator;
            using IteratorX = TransformingIterator<Iterator, decltype(action)>;
            return Range{IteratorX{container.begin(), action}, IteratorX{container.end(), action}};
        };
    };

    // ---[ FILTER implementation: implement your "Filtering Iterator" here

    template<typename Iterator, typename Callable>
    struct FilterIterator : Iterator
    {
        using OriginalIterator = Iterator; // required for accessing the used Iterator type from other locations
        Callable callable;
        Iterator end;
        Iterator begin;
        FilterIterator(Iterator iterator, Callable callable, Iterator end) : Iterator{iterator}, callable{callable},end{end},begin{iterator} { }
        Iterator& get_orig_iter() { return ((Iterator&)*this); }

        auto operator *(){
            auto& it = get_orig_iter();

            //std::cout << *end << " hello " << std::endl;

            if(get_orig_iter() < end){
                if(callable(*it)) {
                    return *it;
                }
                else{
                    it++;
                    std::cout << "lms" << std::endl;
                    return this->operator*();

                }
            }
            else{
                it++;
                std::cout << "lms" << std::endl;
                return this->operator*();
            }
            return *get_orig_iter();
        }

        auto operator !=(const Iterator& end_iter) {
//          //  std::cout << "get origin iter points to " << *get_orig_iter() << std::endl;
 //          toDO NOT WORKING PART

//            if(get_orig_iter() == end_iter-1){
//                if(callable(*iter)) {
//                    std::cout << "lalalala" << std::endl;
//                    return true;
//                }
//                else {
//                    std::cout << "lalalala" << std::endl;
//                    return false;
//                }
//            }
//            else if (get_orig_iter() < end_iter && callable(*iter)) {
//               std::cout << "burak00" << std::endl;
//                return true;
//            }
//            else {
//                std::cout << "falseeeeeeeeee" << std::endl;
//                return false;
//
//            }
            if(get_orig_iter() == end_iter){
                return false;
            }
            if(callable(*get_orig_iter())){
                return true;
            }
            if(get_orig_iter() < end_iter && !callable(*get_orig_iter())){
                ++get_orig_iter();
                return (*this) != end_iter;
            }

        }
    };

    auto filter = [](auto action){
        return [=](auto& container){
            using Container = std::decay_t<decltype(container)>;
            using Iterator = typename  Container::iterator;
            using IteratorX = FilterIterator<Iterator , decltype(action) >;
            return Range{IteratorX{container.begin(), action,container.end()}, IteratorX{container.end() , action,container.end()}};

        };
    };

    // ---[ TO implementation: implement your "render into a container" method here
    template <template<typename ...> typename Container>
    auto to(){
        return [&](const auto &range){
            const auto origin_iter = range.begin().get_orig_iter();
            Container<std::decay_t<decltype(*origin_iter)>> container;
            for(auto item : range){
                container.push_back(item);
            }
            return container;
        };
    };

}

template<typename CONTAINER, typename RANGE>
auto operator |(CONTAINER&& container, RANGE&& range) { return range(std::forward<CONTAINER>(container)); }

using namespace predicates;
using namespace actions;
using namespace algorithms;




int main(int argc, char* argv[])
{
    auto burak = "burak";
    auto new_line = [] { std::cout << std::endl; };
    // todo this code will be deleted after the tests are done
    std::vector<double> testVector;
    size_t a = 1000000;
    for (size_t i = 0;i<a;i++){
        int b = rand() % 20 + 1;
        testVector.push_back(b);
    }
    std::cout << "testVector is created" << std::endl;

    auto v = std::vector<double>{};
    auto odd_gen = views::odds();
    for(int i=0; i<5; ++i)
        v.push_back(odd_gen() * 2.5);
    // v contains {2.5, 7.5, 12.5, 17.5, 22.5} here
    auto range = v | transform(if_then(all_of(greater_than(2.0), less_than(15.0)), multiply_by(20)));
    for(auto a : range) // transformation is applied on the range as the for loop progresses
        std::cout << a << std::endl;
    // original v is not changed. prints {50.0, 150.0, 250.0, 17.5, 22.5}

    new_line();
    std::vector<double> testVector2;
    for(int i = 0;i<5;i++){
        testVector2.push_back(1.0);
    }
    std::vector<double> testVector3{1};
    std::vector<double> testVector4{1,2,3,4,5,15,16,18,1,2};
   v.push_back(1);
   v.push_back(1);
   // std::cout <<   "burak" << std::endl;
//   auto it = v.begin();
//   std::advance(it,v.size());
//   for (auto it = v.begin();it!=v.end();it++)
//   {
//
//       std::cout << &it <<" it " << &v.end() << " v.end() "<< std::endl ;
//   }
  //  auto r =  v | filter(greater_than(15));
//    for(auto it = r.begin() ; it != r.end(); ++it){
//        std::cout << *it << std::endl;
//        std::cout << &it <<"   " <<&r.begin().get_orig_iter() << std::endl;
//    }
    for(auto a : v | filter(greater_than(15))) { // filter is applied lazily as the range is traversed
        std::cout << a << std::endl;
    }
    // prints 17.5 and 22.5 to the console

    new_line();
    auto u = std::vector<int>{10, 20, 30};

    auto u_transformed = u | transform(multiply_by(2)) | to<std::vector>();
    for(auto a : u_transformed) // u_transformed is an std::vector<int> automatically because of to<std::vector>
        std::cout << a << std::endl;

    //todo some shit to make this other shit work
//    auto k = std::vector<int>{10,20,30} | transform(multiply_by(2)) | to<std::vector>();
//    for(auto a : k){
//        std::cout << a << std::endl;
//    }
//    return 0;
//
}
