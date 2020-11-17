#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

int get_determinant(int** matrix, size_t dimension);
int get_cofactor(int** matrix, size_t dimension, size_t row, size_t column);

void get_cofactors_thread(int** matrix, int** result, const size_t dimension, const int thread_index, const int number_of_threads) {
	// go through all the elements the current thread has to process
	for (size_t i = thread_index; i < dimension * dimension; i += number_of_threads) {
		// put cofactor og the element to the cofactors matrix
		result[i / dimension][i % dimension] = get_cofactor(matrix, dimension, i / dimension, i % dimension);
	}
}

int get_cofactor(int** matrix, const size_t dimension, const size_t row, const size_t column) {
	// allocate the memory for the minor matrix
	int** minor = static_cast<int**>(malloc((dimension - 1) * sizeof(int*)));

	// allocate the memory for the minor matrix elements
	for (size_t i = 0; i < dimension - 1; i++) {
		minor[i] = static_cast<int*>(malloc(static_cast<int16_t>((dimension - 1) * sizeof(int))));
	}

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

int get_determinant(int** matrix, const size_t dimension) {
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


std::string matrix_to_string(int** matrix, size_t dimension) {
	std::string result;

	for (size_t i = 0; i < dimension; i++, result += "\n") {
		for (size_t j = 0; j < dimension; j++) {
			result += std::to_string(matrix[i][j]) + " ";
		}
	}

	return result;
}

bool try_get_integer_from(std::ifstream& stream, const int min, int* value) {
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

int main(int argc, char* argv[]) {
	std::ifstream in;

	in.open(argv[1]);
	if (!in.is_open()) {
		std::cout << "[error]: Incorrect input path." << std::endl;
		return 0;
	}

	int** matrix;			// initial matrix
	int** result_matrix;	// cofactors matrix
	int dimension;			// number of matrix rows and columns
	int threads_number;		// number of threads

	try
	{
		std::string line;

		// read the first line of a file containing the information about the threads number
		if (!try_get_integer_from(in, 1, &threads_number)) {
			write_result(argv[2], "[error]: Illegal number of threads.");
			return 0;
		}
		if (!try_get_integer_from(in, 2, &dimension)) {
			write_result(argv[2], "[error]: Illegal matrix dimension.");
			return 0;
		}

		// allocate the memory for the matrices
		matrix = static_cast<int**>(malloc(dimension * sizeof(int*)));
		result_matrix = static_cast<int**>(malloc(dimension * sizeof(int*)));

		// allocate the memory for the matrices elements
		for (int i = 0; i < dimension; i++) {
			matrix[i] = static_cast<int*>(malloc(dimension * sizeof(int)));
			result_matrix[i] = static_cast<int*>(malloc(dimension * sizeof(int)));
		}

		// read the lines with matrix data and initializing matrix elements
		int i;
		for (i = 0; i < dimension && !in.eof(); ++i)
		{
			std::getline(in, line);

			size_t column_index = 0;
			while (column_index + 1 != static_cast<size_t>(dimension) && line.find(' ') != std::string::npos) {
				const auto index = line.find(' ');

				matrix[i][column_index++] = std::stoi(line.substr(0, index));

				line = line.substr(index + 1);
			}

			matrix[i][column_index++] = std::stoi(line);

			if (column_index != static_cast<size_t>(dimension)) {
				write_result(argv[2], "[error]: Invalid matrix.");
				free(matrix);
				free(result_matrix);
				return 0;
			}
		}

		if (in.eof() && i != dimension) {
			write_result(argv[2], "[error]: Invalid matrix.");
			free(matrix);
			free(result_matrix);
			return 0;
		}
	}
	catch (...) {
		write_result(argv[2], "[error]: Invalid matrix.");
		free(matrix);
		free(result_matrix);
		return 0;
	}

	// setting threads
	std::vector<std::thread*> thr;

	for (size_t i = 0; i < static_cast<size_t>(threads_number); i++) {
		thr.push_back(new std::thread{ get_cofactors_thread, matrix, result_matrix, dimension, i, threads_number });
	}


	for (size_t i = 0; i < static_cast<size_t>(threads_number); i++) {
		thr[i]->join();
		delete thr[i];
	}

	// output the results
	write_result(argv[2], matrix_to_string(result_matrix, dimension));

	free(matrix);
	free(result_matrix);

	return 0;
}
