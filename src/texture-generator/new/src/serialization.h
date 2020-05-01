#include <iostream>
#include <memory>

void serialize(std::ostream& out, double* d, size_t size);
// std::unique_ptr<double> deserialize(std::istream& in, size_t& size);
std::unique_ptr<double> deserializeFromFile(const std::string& filename, size_t& size);

