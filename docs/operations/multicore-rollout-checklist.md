# Multicore DSP Rollout Checklist

## Pre-Rollout Requirements
- [ ] Baseline stability achieved (no segfaults)
- [ ] All unit tests passing  
- [ ] Sequential CI suite green
- [ ] Performance regression < 3%
- [ ] RT-safety verified (no allocs/locks)

## Environment Setup
- [ ] SC_FORCE_SINGLECORE=1 testing completed
- [ ] SC_FORCE_MULTICORE=1 testing completed  
- [ ] HUD monitoring functional
- [ ] Performance counters operational

## Rollout Steps
1. **Phase 1**: Enable with ENABLE_MULTICORE_DSP=true
2. **Phase 2**: Monitor HUD for fallback counts
3. **Phase 3**: Performance validation
4. **Phase 4**: Full deployment

## Monitoring
- Watch for: Increased fallback counts, performance degradation, crashes
- Rollback trigger: Any RT violations or stability regressions

## Rollback Plan
1. Set ENABLE_MULTICORE_DSP=false
2. Rebuild and redeploy
3. Verify sequential performance restored