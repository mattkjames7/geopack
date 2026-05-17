# Test workflow

1. Build library first from repo root:
   - `make all`
2. Generate golden test data:
   - `cd test && make savetestdata`
3. Run gtest unit suite:
   - `cd test && make unit`

Golden files are written to `test/data/*.json` by `test/savetestdata/savetestdata`.
