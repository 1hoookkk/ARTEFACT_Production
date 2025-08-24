# Testing Strategy

## Run All Tests

```
ctest --test-dir build -C RelWithDebInfo --output-on-failure
```

## Filtered Runs

```
# RT-safety focused
ctest --test-dir build -C RelWithDebInfo -R "RT|Paint|Audio|processBlock|ThreadSafety"

# Specific test name
ctest --test-dir build -C RelWithDebInfo -R "TestPaintProducesAudio" --output-on-failure
```

## List Tests

```
ctest --test-dir build -N -C RelWithDebInfo
```

## Windows Helpers (optional)

- `scripts/test-and-notify.ps1`
  - `-RTSafetyOnly` to focus on RT tests
  - `-SpecificTest "NAME"` for a single test

