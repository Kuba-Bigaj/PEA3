#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <vector>

class App {
	std::vector<double> change_times;
	std::vector<int> change_vals;


	//data storage
	std::string loaded_file = "None";
	int** matrix = nullptr;
	int size = 0;

	int save_counter = 0;

	//algorith-specific data storage
	int run_limit = 20;				//in seconds
	int population_size = 50;
	int mutation_method = 0;


	//static UI helper functions
	static int create_sub_menu(std::string top_banner, std::string options[], std::string bot_banner, int number, int def_option) // displays a submenu with options, returns the number chosen
	{
		int chosen_option = def_option;
		while (true)
		{
			system("cls");
			std::cout << top_banner;
			for (int i = 0; i < number; i++)
			{
				if (i == chosen_option)
				{
					std::cout << "==>";
				}
				std::cout << "\t" << options[i] << "\n";
			}
			std::cout << bot_banner;
			switch (_getch())
			{
			case 72:
				chosen_option--;
				break;
			case 80:
				chosen_option++;
				break;
			case '\r':
				return chosen_option;
			default:
				break;
			}
			chosen_option = (chosen_option < 0 ? 0 : (chosen_option >= number ? number - 1 : chosen_option));
		}
	}

	static int create_adaptive_sub_menu(std::string top_banner, std::string options[], std::string bot_banner[], int number, int def_option) // displays a submenu with options, returns the number chosen
	{
		int chosen_option = def_option;
		while (true)
		{
			system("cls");
			std::cout << top_banner;
			for (int i = 0; i < number; i++)
			{
				if (i == chosen_option)
				{
					std::cout << "==>";
				}
				std::cout << "\t" << options[i] << "\n";
			}
			std::cout << bot_banner[chosen_option];
			switch (_getch())
			{
			case 72:
				chosen_option--;
				break;
			case 80:
				chosen_option++;
				break;
			case '\r':
				return chosen_option;
			default:
				break;
			}
			chosen_option = (chosen_option < 0 ? 0 : (chosen_option >= number ? number - 1 : chosen_option));
		}
	}

	static bool test_input_validity(std::string err_message)
	{
		if (std::cin.fail())
		{
			std::cout << err_message << "\n";
			std::cin.clear();
			std::cin.ignore(1000, '\n');
			system("pause");
			return false;
		}
		return true;
	}

	//data manipulation functions
	void dealloc_matrix() //deallocates the matrix, sets size to 0
	{
		if (!matrix)
		{
			size = 0;
			return;
		}
		for (int i = 0; i < size; i++)
		{
			delete matrix[i];
		}
		delete matrix;
		matrix = nullptr;
		size = 0;
	}

	void alloc_matrix(int s) //allocates the matrix
	{
		size = s;
		matrix = new int* [s];
		for (int i = 0; i < s; i++)
		{
			matrix[i] = new int[s];
		}
	}

	void show_data()
	{
		std::cout << "Loaded file: " << loaded_file << "\nData size: " << size << "\n";
		system("pause");
	}

	std::string str_path(int* solution)
	{
		std::string output;
		output += std::to_string(solution[0]);
		for (int i = 1; i < size; i++)
		{
			output += " -> " + std::to_string(solution[i]);
		}
		output += "\n";
		return output;
	}

	void read_data_from_file(std::string filename) //TODO adapt
	{
		std::ifstream file(filename);
		std::string line;

		if (!file.is_open())
		{
			std::cout << "File open error!\n";
			system("pause");
			return;
		}

		std::getline(file, line);	//title

		loaded_file = line.substr(6, line.size());

		std::getline(file, line);	//type
		std::getline(file, line);	//comment
		std::getline(file, line);	//dimension

		int f_size = std::stoi(line.substr(11, line.size() - 1));

		std::getline(file, line);	//edge type
		std::getline(file, line);	//edge format
		std::getline(file, line);	//edge section



		dealloc_matrix();

		size = f_size;

		alloc_matrix(size);

		int val;
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				file >> val;
				if (file.fail())
				{
					std::cout << "Data read error!\n";
					break;
				}
				else
					matrix[i][j] = val;
			}
		}
		file.close();
		return;
	}

	void load_data()
	{
		std::string filename;
		std::cout << "Enter filename:\n";
		std::cin >> filename;
		if (test_input_validity("unsupported filename!\n"))
		{
			read_data_from_file(filename);
			std::cout << "Loaded data: \n";
			show_data();
		}
	}

	void copy_arr(int* src, int* dest)
	{
		for (int i = 0; i < size; i++)
		{
			dest[i] = src[i];
		}
	}

	void save_path_to_file(int* path)
	{
		std::ofstream file(loaded_file + "_" + std::to_string(save_counter) + "_results.txt");
		save_counter++;
		file << size << "\n";
		for (int i = 0; i < size; i++)
		{
			file << path[i] << " ";
		}
		file.close();
	}

	//generic algorithm helpers
	int path_len(int* solution)
	{
		int len = 0;
		for (int i = 1; i < size; i++)
		{
			int a = solution[i - 1];
			int b = solution[i];
			len += matrix[solution[i - 1]][solution[i]];
		}
		len += matrix[solution[size - 1]][solution[0]];
		return len;
	}

	//generates a random double between 0 and 1
	double generate_random_double()
	{
		return (double)((double)rand() / (double)RAND_MAX);
	}

	int* generate_random_path()
	{
		int* solution = new int[size];
		bool* vis = new bool[size];
		int cur_v = 0;

		for (int i = 0; i < size; i++)
			vis[i] = false;

		for (int i = 0; i < size; i++)
		{
			cur_v = rand() % size;
			while (vis[cur_v])
			{
				cur_v = (cur_v + 1) % size;
			}
			vis[cur_v] = true;
			solution[i] = cur_v;
		}

		delete vis;
		return solution;
	}

	//mutations
	void mutate_transposition(int* specimen, int a, int b)
	{
		std::swap(specimen[a], specimen[b]);
	}

	void mutate_inversion(int* specimen, int a, int b)
	{
		if (a == b)
			return;
		int min = a < b ? a : b;
		int max = a > b ? a : b;

		int* copy = new int[size];
		copy_arr(specimen, copy);

		for (int i = 0; i <= max - min; i++)
		{
			specimen[min + i] = copy[max - i];
		}

		delete copy;
	}

	void mutate(int* specimen)
	{
		int a = rand() % size, b = a + (rand() % (size - a));
		switch (mutation_method)
		{
		case 0:
			mutate_transposition(specimen, a, b);
			break;
		case 1:
			mutate_inversion(specimen, a, b);
			break;
		default:
			return;
		}

	}
	//crossovers

	int PMX_find_index(int* parent1, int* parent2, int a, int b, int p2_index)
	{
		int target_value = parent1[p2_index];
		int new_index = 0;
		for (int i = 0; i < size; i++)
		{
			if (parent2[i] == target_value)
			{
				new_index = i;
				break;
			}
		}

		if (a <= new_index && b > new_index)
		{
			new_index = PMX_find_index(parent1, parent2, a, b, new_index);
		}
		return new_index;
	}

	int* crossover_PMX(int* parent1, int* parent2, int a, int b) //returns one new child, whoose genes on positions <a, b) are the same as parent1
	{
		int* child = new int[size];
		bool* visited = new bool[size]; //cities already in the child
		for (int i = 0; i < size; i++)
		{
			visited[i] = false;
			child[i] = -1;
		}

		for (int i = a; i < b; i++)
		{
			child[i] = parent1[i];
			visited[parent1[i]] = true;
		}

		for (int i = a; i < b; i++)
		{
			if (visited[parent2[i]]) //skip cities already in the child
			{
				continue;
			}

			visited[parent2[i]] = true;
			child[PMX_find_index(parent1, parent2, a, b, i)] = parent2[i]; //find the correct index
		}

		for (int i = 0; i < size; i++)
		{
			if (!visited[parent2[i]])
			{
				visited[parent2[i]] = true;
				child[i] = parent2[i];
			}
		}

		delete visited;
		return child;
	}

	/*
		simple insertion sort
		solutions: [populations_size] x solutions of length [size]
		fitness: fitness values for the solutions, bigger is better
	*/
	void order_solutions(int** solutions, double* fitness)
	{
		int index;
		double max;

		for (int i = 0; i < population_size; i++)
		{
			fitness[i] = 1.0 / (double)path_len(solutions[i]);
		}


		for (int i = 0; i < population_size; i++)
		{
			max = -1e7;
			for (int j = i; j < population_size; j++)
			{
				if (fitness[j] > max)
				{
					max = fitness[j];
					index = j;
				}
			}
			std::swap(solutions[i], solutions[index]);
			std::swap(fitness[i], fitness[index]);
		}

		/*for (int i = 0; i < population_size; i++)
		{
			std::cout <<path_len(solutions[i])<<"\t"<< fitness[i] << "\t" << str_path(solutions[i]) << "\n";
		}*/
	}

	//algorithms
	int* greedy()
	{
		int* solution = new int[size];
		bool* vis = new bool[size];
		bool is_done = false;
		int cur_vert, min, min_v;

		cur_vert = 0;
		for (int i = 0; i < size; i++)
			vis[i] = false;

		for (int c = 0; !is_done; c++)
		{
			solution[c] = cur_vert;
			vis[cur_vert] = true;

			is_done = true;
			min = INT_MAX;
			min_v = 0;

			for (int i = 0; i < size; i++)
			{
				if (vis[i])
					continue;

				is_done = false;

				if (matrix[cur_vert][i] < min)
				{
					min = matrix[cur_vert][i];
					min_v = i;
				}
			}
			cur_vert = min_v;
		}
		return solution;
	}

	int* genetic()
	{
		//declare / init
		int** population = new int* [population_size];
		for (int i = 0; i < population_size; i++)
		{
			population[i] = generate_random_path();
		}
		double* fitness = new double[population_size];
		int* best_solution = new int[size];
		double best_fit = -1e7;
		int* child;
		int num_of_children = (int)(0.8 * population_size);
		double total_fitness, pointer_distance, offset;
		int* parents = new int[num_of_children];

		int num = 0;
		auto start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed;

		do
		{
			//evaluate solutions
			order_solutions(population, fitness);
			if (fitness[0] > best_fit) //save the best one
			{
				copy_arr(population[0], best_solution);
				best_fit = fitness[0];
			}

			//select parents using Stochastic Uniform Selection
			total_fitness = 0;
			for (int i = 0; i < population_size; i++)
			{
				total_fitness += fitness[i];
			}
			pointer_distance = total_fitness / num_of_children;
			offset = generate_random_double() * pointer_distance;
			for (int i = 0; i < num_of_children; i++)
			{
				int j = 0;
				double partial_sum = fitness[0];
				double goal = (pointer_distance * i) + offset;
				while (partial_sum < goal)
				{
					//somewhere here 
					j++;
					partial_sum += fitness[j];
				}
				if (j > 55)
				{
					std::cout << "Here\n";
					for (int h = 0; h < population_size; h++)
					{
						std::cout << fitness[h] << "\n";
					}
					std::cout<<1+2;

				}
				
				parents[i] = j;
			}

			//procreate among parents
			for (int i = 0; i < num_of_children; i++)
			{
				int a = rand() % size, b = a + (rand() % (size - a));
				int p1 = parents[i], p2 = rand() % population_size;
				child = crossover_PMX(population[p1], population[p2], a, b);

				//mutate
				if (generate_random_double() < 0.1)
				{
					mutate(child);
				}

				copy_arr(child, population[p1]);

				delete  child;
			}

			//succeed
			//succesion is applied through the death of parents and perserverence of the infertile

			elapsed = std::chrono::high_resolution_clock::now() - start;
		} while (elapsed.count() < run_limit);

		for (int i = 0; i < population_size; i++)
		{
			delete population[i];
		}
		delete population;
		delete fitness;
		delete parents;
		return best_solution;
	}

	//UI functions

	void set_stop_conditions()
	{
		int time_con;
		std::cout << "Enter a new time constraint (in seconds): ";
		std::cin >> time_con;
		if (test_input_validity("ERROR - not an integer!"))
			run_limit = time_con;

		system("cls");
		std::cout << "Time constraint: " << run_limit << " seconds\n";
		system("pause");
	}

	void read_path_from_file()
	{
		if (size == 0)
		{
			std::cout << "No data loaded!\n";
			system("pause");
			return;
		}

		std::string filename;
		std::cout << "Enter filename:\n";
		std::cin >> filename;
		if (!test_input_validity("Unsupported filename!\n"))
			return;

		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cout << "File open error!\n";
			system("pause");
			return;
		}

		int s;
		file >> s;

		if (s != size)
		{
			std::cout << "Size mismatch error!\n";
			file.close();
			system("pause");
			return;
		}

		int* path = new int[size];

		for (int i = 0; i < size; i++)
		{
			file >> path[i];
		}
		file.close();

		std::cout << "Read path has a length of " << path_len(path) << "\n";

		delete path;
		system("pause");
	}

public:
	void run()
	{
		std::string title = "MAIN MENU\n";
		std::string credits = "Kuba Bigaj 2023\n";
		std::string options[10] = { "Load data", "Show current data","Set stop conditions", "Simulated annealing", "[SA] Set cooling schedule","[SA] Set cooling coefficient", "Taboo Search", "[TS] Set neighbourhood definition", "Read path from text file",  "Exit" };
		int chosen_option = 0;

		while (true)
		{
			chosen_option = create_sub_menu(title, options, credits, 10, chosen_option);

			switch (chosen_option)
			{
			case 0:
				load_data();
				break;
			case 1:
				show_data();
				break;
			case 2:
				set_stop_conditions();
				break;
			case 3:
				system("pause");
				break;
			case 4:
				system("pause");
				break;
			case 5:
				system("pause");
				break;
			case 6:
				system("pause");
				break;
			case 7:
				system("pause");
				break;
			case 8:
				read_path_from_file();
				break;
			case 9:
				return;
			default:
				break;
			}
		}
	}

	void debug()
	{
		read_data_from_file("ftv55.atsp");
		//run_limit = 180;
		//std::cout << path_len(greedy()) << "\n";
		int* sol = genetic();
		//int* p1 = generate_random_path();
		//int* p2 = generate_random_path();
		//std::cout << str_path(crossover_PMX(p1, p2, 20, 48));
		std::cout << "path_len: " << path_len(sol) << "\n";
		std::cout << "path: " << str_path(sol) << "\n";
	}
};


int main(int argc, char* argv[])
{
	//srand(time(NULL)); //1705058225 - breaks
	srand(1705058225);
	App a;
	if (argc > 1)
	{
		if (strcmp(argv[1], "-t") == 0)
		{
			std::cout << "Test would be run...\nIf I had any!\n";
			system("pause");
			return 0;
		}
	}
	//a.run();
	a.debug();
	return 0;
}