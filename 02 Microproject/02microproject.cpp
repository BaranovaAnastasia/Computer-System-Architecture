#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <vector>
#include <ctime>
#include <string>

int currently_read_count = 0;
int currently_write_count = 0;

// access to the readers and writers counters
sem_t reader_count_sem, writer_count_sem;
//access to the database
sem_t reader_access, writer_access;

const int database_size = 10;
int database[10];

int iterations;

int start_time;


void* write(void* i)
{
	const int index = *static_cast<int*>(i);

	for (int j = 0; j < iterations; ++j)
	{
		std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index) + " started.\n";


		// registration of a new writer
		sem_wait(&writer_count_sem);		// waiting for an access to the currently_write_count variable

		currently_write_count++;			// adding a writer

		if (currently_write_count == 1) {	// if its the first writer
			std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index) + " is waiting for readers to finish.\n";
			sem_wait(&reader_access);		// waiting for an access from readers and blocking it for them
			std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index) + " blocked an access for readers.\n";
		}

		sem_post(&writer_count_sem);		// releasing an access to the currently_write_count variable


		// critical section
		std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index) + " is waiting for an access to the database.\n";
		sem_wait(&writer_access);			// waiting for an access to write and blocking it for others

		// do writing
		const int index_in_database = rand() % database_size;
		database[index_in_database] += 1;
		std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index)
			+ " wrote " + std::to_string(database[index_in_database])
			+ " to the " + std::to_string(index_in_database) + " element of a database.\n";

		sem_post(&writer_access);			// releasing an access for the other writers
		// end of critical section


		// end of the writer's work
		sem_wait(&reader_count_sem);		// waiting for an access to the currently_write_count variable

		currently_write_count--;			// removing a writer

		if (currently_write_count == 0) {	// if its a last writer
			std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index) + " released an access for readers.\n";
			sem_post(&reader_access);		// releasing an access for the readers
		}

		sem_post(&reader_count_sem);		// releasing an access to the currently_write_count variable


		std::cout << "Time " + std::to_string(clock() - start_time) + ". Writer " + std::to_string(index) + " finished their work.\n";
	}

	return nullptr;
}

void* read(void* i)
{
	const int index = *static_cast<int*>(i);

	for (int j = 0; j < iterations; ++j)
	{
		std::cout << "Time " + std::to_string(clock() - start_time) + ". Reader " + std::to_string(index) + " started.\n";

		std::cout << "Time " + std::to_string(clock() - start_time) + ". Reader " + std::to_string(index) + " is waiting for an access to read.\n";
		sem_wait(&reader_access);			// waiting for an access to read

		// registration of a new reader
		sem_wait(&reader_count_sem);		// waiting for an access to the currently_read_count variable

		currently_read_count++;				// adding a reader

		if (currently_read_count == 1) {	// if its the first reader
			sem_wait(&writer_access);		// waiting for the access from writers and blocking it
			std::cout << "Time " + std::to_string(clock() - start_time) + ". Reader " + std::to_string(index) + " blocked an access for writers.\n";
		}

		sem_post(&reader_count_sem);		// releasing an access to the currently_read_count variable
		sem_post(&reader_access);			// releasing an access to read


		// critical section
		//do reading
		const int index_in_database = rand() % database_size;
		std::cout << "Time " + std::to_string(clock() - start_time) + ". Reader " + std::to_string(index)
			+ " read value " + std::to_string(database[index_in_database])
			+ " at the " + std::to_string(index_in_database) + " element of a database.\n";
		// end of the critical section


		// the end of the work of the writer
		sem_wait(&reader_count_sem);		// waiting for an access to the currently_read_count variable

		currently_read_count--;				// removing a reader

		if (currently_read_count == 0) {	// if its the last
			std::cout << "Time " + std::to_string(clock() - start_time) + ". Reader " + std::to_string(index) + " released an access for writers.\n";
			sem_post(&writer_access);		// releasing an access to write
		}

		sem_post(&reader_count_sem);		// releasing an access to the currently_read_count variable


		std::cout << "Time " + std::to_string(clock() - start_time) + ". Reader " + std::to_string(index) + " finished their work.\n";
	}


	return nullptr;
}

int main(int argc, char* argv[])
{
	try {
		iterations = std::stoi(argv[1]);
		if (iterations <= 0) {
			throw std::logic_error("Illegal number of iterations");
		}
	}
	catch (...) {
		std::cout << "Incorrect number of iterations \'" << argv[1] << "\'. Mast be a positive integer.";
		return -1;
	}

	sem_init(&reader_access, 0, 1);
	sem_init(&writer_access, 0, 1);
	sem_init(&reader_count_sem, 0, 1);
	sem_init(&writer_count_sem, 0, 1);

	for (int& i : database) {
		i = 0;
	}

	std::vector<pthread_t> readers(4);
	std::vector<pthread_t> writers(4);
	std::vector<int> indices = { 1, 2, 3, 4 };

	start_time = clock();

	for (int i = 0; i < 4; ++i) {
		pthread_create(&readers[i], nullptr, read, static_cast<void*>(&indices[i]));
		pthread_create(&writers[i], nullptr, write, static_cast<void*>(&indices[i]));
	}


	for (int i = 0; i < 4; ++i) {
		pthread_join(writers[i], nullptr);
		pthread_join(readers[i], nullptr);
	}

	return 0;
}
