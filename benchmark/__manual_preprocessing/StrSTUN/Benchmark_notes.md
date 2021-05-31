# Benchmark Status


reason: Filter out boolean program (not supported), and repeated.

how many left:
- euphony: 90 (remove 10 booleans from original 100)
  - remove: 
    - `19274448`, 
    - `40498040`, 
    - `cell-contains-all-of-many-things`
    - `cell-contains-number`
    - `cell-contains-one-of-many-things`
    - `cell-contains-one-of-many-with-exclusions`
    - `cell-contains-some-words-but-not-others`
    - `cell-contains-specific-text`
    - `cell-equals-one-of-many-things`
    - `compare-two-strings`
  - 73/90 can be solved directly under termsize 9
  - After manually writing those 73 programs, 59/73 can be manually written below termsize 9
  - SynGuar run on those 59 programs.

- SynGuar should run on 59 programs. 
