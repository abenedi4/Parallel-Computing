#ifndef __SEQ_LOOP_H
#define __SEQ_LOOP_H
//#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
class SeqLoop {
public:
  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel
  void parfor (size_t beg, size_t end, size_t inc,
	       std::function<void(int)> f) {
    for (size_t i=beg; i<end; i+= inc) {
      f(i);
    }
  }

  /// @brief execute the function f multiple times with different
  /// parameters possibly in parallel
  ///
  /// f will be executed multiple times with parameters starting at
  /// beg, no greater than end, in inc increment. These execution may
  /// be in parallel.
  ///
  /// Each thread that participate in the calculation will have its
  /// own TLS object.
  ///
  /// Each thread will execute function before prior to any
  /// calculation on a TLS object.
  ///
  /// Each thread will be passed the TLS object it executed before on.
  ///
  /// Once the iterations are complete, each thread will execute after
  /// on the TLS object. No two thread can execute after at the same time.
  template<typename TLS>
  void parfor ( size_t increment,
		int nbthreads,
		int n,
	       std::function<void(TLS&)> before,
	       std::function<void(int, TLS&)> f,
	       std::function<void(TLS&)> after
	       ) {
    
    TLS tls[nbthreads];
    std::vector<std::thread> threads;
    size_t numiter = n/nbthreads;
   


    for (int j = 0; j < nbthreads; ++j) {
      before(std::ref(tls[j]));
    }
    //create threads and partition work
    for (int j = 0; j < nbthreads; ++j) {
    //before method

      threads.emplace_back(std::thread([&](size_t beg, size_t end, TLS& tls) {
					 for (size_t i=beg; i<end; i+= increment){					   
					   f(i, tls);
					  
					 }
				       }, (j*numiter), ((j+1)==nbthreads) ? n : ((j * numiter) + numiter), std::ref(tls[j])));


    }

    
    //wait for threads to finish
    for (auto& thread : threads){
      thread.join();
    }

    for (int k = 0; k < nbthreads; ++k){
      after(std::ref(tls[k]));
    }
  }
  


  
};




#endif
