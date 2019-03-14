#include <iostream>
#include <openmp>

int count_words(string user_input);

int main()
{
  int thread_id;
  bool waiting = true;
  string user_input;

  #pragma omp parallel num_threads(2)
  {
    thread_id = omp_get_thread_num();
    if(thread_id == 0)
    {
      std::cout << "Digite la oración: ";
      std::cin >> user_input;
    }
    #pragma omp barrier
    if(thread_id == 1)
    {
      count_words(user_input);
      std::cout << user_input << " , " << std::endl;
    }
  }

}

int count_words(string user_input)
{

}
