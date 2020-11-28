#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <omp.h>

int64_t get_determinant(const std::vector<std::vector<int64_t>>& matrix, size_t dimension);
int64_t get_cofactor(const std::vector<std::vector<int64_t>>& matrix, size_t dimension, size_t row, size_t column);

void get_cofactors_thread(const std::vector<std::vector<int64_t>>& matrix, std::vector<std::vector<int64_t>>& result,
	const size_t dimension, const int64_t thread_index, const int64_t number_of_threads) {
	// go through all the elements the current thread has to process
	for (size_t i = thread_index; i < dimension * dimension; i += number_of_threads) {
		// put cofactor og the element to the cofactors matrix
		result[i / dimension][i % dimension] = get_cofactor(matrix, dimension, i / dimension, i % dimension);
	}
}

int64_t get_cofactor(const std::vector<std::vector<int64_t>>& matrix, const size_t dimension, const size_t row, const size_t column) {
	auto minor = std::vector<std::vector<int64_t>>(dimension - 1, std::vector<int64_t>(dimension - 1));

	// initialize minor matrix
	for (size_t i = 0; i < dimension; i++) {
		if (i == row) {
			continue;
		}
		for (size_t j = 0; j < dimension; j++) {
			if (j == column) {
				continue;
			}

			const auto x = i > row ? i - 1 : i;
			const auto y = j > column ? j - 1 : j;

			minor[x][y] = matrix[i][j];
		}
	}

	const auto sign = (row + column) % 2 == 0 ? 1 : -1;

	return sign * get_determinant(minor, dimension - 1);
}

int64_t get_determinant(const std::vector<std::vector<int64_t>>& matrix, const size_t dimension) {
	// the simplest cases
	if (dimension == 1) {
		return matrix[0][0];
	}
	if (dimension == 2) {
		return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
	}

	// calculating determinant recursively using cofactors
	auto determinant = 0;
	for (size_t i = 0; i < dimension; ++i) {
		determinant += matrix[i][0] * get_cofactor(matrix, dimension, i, 0);
	}

	return determinant;
}


std::string matrix_to_string(const std::vector<std::vector<int64_t>>& matrix, size_t dimension) {
	std::string result;

	for (size_t i = 0; i < dimension; i++, result += "\n") {
		for (size_t j = 0; j < dimension; j++) {
			result += std::to_string(matrix[i][j]) + " ";
		}
	}

	return result;
}

bool try_get_integer_from(std::ifstream& stream, const int64_t min, int64_t* value) {
	if (!stream.is_open() || stream.eof()) {
		return false;
	}

	std::string line;

	std::getline(stream, line);
	try {
		*value = std::stoi(line);
	}
	catch (...) {
		return false;
	}

	if (*value < min) {
		return false;
	}

	return true;
}

void write_result(const std::string& path, const std::string& data) {
	std::ofstream out;

	out.open(path);

	if (out.is_open()) {
		out << data;
		out.close();
		return;
	}
	std::cout << "[error]: Incorrect output path. Unable to write the result." << std::endl;
	std::cout << "Result:" << std::endl << data;
}

bool try_read_matrix(std::ifstream& in, int64_t& dimension, std::vector<std::vector<int64_t>>& matrix, std::string& output_message)
{
	if (!try_get_integer_from(in, 2, &dimension)) {
		output_message = "[error]: Illegal matrix dimension.";
		return false;
	}

	matrix = std::vector<std::vector<int64_t>>(dimension);

	std::string line;
	// reading the lines with matrix data and initializing matrix elements
	int64_t i;
	for (i = 0; i < dimension && !in.eof(); ++i)
	{
		std::getline(in, line);

		size_t column_index = 0;
		while (column_index + 1 != static_cast<size_t>(dimension) && line.find(' ') != std::string::npos) {
			const auto index = line.find(' ');

			matrix[i].push_back(std::stoi(line.substr(0, index)));

			line = line.substr(index + 1);
			column_index++;
		}

		matrix[i].push_back(std::stoi(line));
		column_index++;

		if (column_index != static_cast<size_t>(dimension)) {
			output_message = "[error]: Invalid matrix.";
			return false;
		}
	}

	if (in.eof() && i != dimension) {
		output_message = "[error]: Invalid matrix.";
		return false;
	}

	return true;
}


int main(int argc, char* argv[]) {
	std::ifstream in;

	in.open(argv[1]);
	if (!in.is_open()) {
		std::cout << "[error]: Incorrect input path." << std::endl;
		return 0;
	}

	std::vector<std::vector<int64_t>> matrix;		// initial matrix
	int64_t dimension;					// number of matrix rows and columns
	int64_t threads_number;				// number of threads

	try
	{
		std::string line;

		// read the first line of a file containing the information about the threads number
		if (!try_get_integer_from(in, 1, &threads_number)) {
			write_result(argv[2], "[error]: Illegal number of threads.");
			in.close();
			return 0;
		}

		std::string message;
		if (!try_read_matrix(in, dimension, matrix, message)) {
			write_result(argv[2], message);
			in.close();
			return 0;
		}
	}
	catch (...) {
		write_result(argv[2], "[error]: Invalid data.");
		in.close();
		return 0;
	}

	in.close();

	std::vector<std::vector<int64_t>> result_matrix = std::vector<std::vector<int64_t>>(dimension, std::vector<int64_t>(dimension));

#pragma omp parallel
	{
#pragma omp for
		for (int64_t i = 0; i < threads_number; i++) {
			get_cofactors_thread(matrix, result_matrix, dimension, i, threads_number);
		}
	}

	// output the results
	write_result(argv[2], matrix_to_string(result_matrix, dimension));

	return 0;
}
