# partial-vector

An implementation of segmented `vector` sequence container.
Compared to the standard vector, segmented design provides faster inserts and removals from random locations
for a long vector of the order of millions of elements.
Under the hood, `partial-vector` manages a variable number of regular `std::vector`s.
