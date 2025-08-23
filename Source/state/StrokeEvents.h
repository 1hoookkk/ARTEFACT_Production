#pragma once
#include <atomic>

struct Stroke {
  float x, y;       // 0..1 canvas coords (y=1 top / high freq)
  float pressure;   // -> snap sigma + EMU predrive ceiling
  float hue;        // -> LP24↔BP12 blend
  float size;       // -> stereo spread
  float speed;      // -> harmonic attack
  float dir;        // -> phase lead/lag
  float bright;     // -> comb richness
};

template <size_t N>
struct SPSCRing {
  Stroke buf[N]; 
  std::atomic<size_t> w{0}, r{0};
  
  bool push (const Stroke& s) { 
    auto n=(w.load(std::memory_order_relaxed)+1)%N;
    if(n==r.load(std::memory_order_acquire)) return false; 
    buf[w.load()]=s; 
    w.store(n,std::memory_order_release); 
    return true; 
  }
  
  bool pop  (Stroke& s) { 
    auto rr=r.load(std::memory_order_relaxed);
    if (rr==w.load(std::memory_order_acquire)) return false; 
    s=buf[rr]; 
    r.store((rr+1)%N,std::memory_order_release); 
    return true; 
  }
};