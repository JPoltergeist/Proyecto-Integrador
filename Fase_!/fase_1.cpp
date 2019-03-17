#include <iostream>
#include <sstream>
#include <iterator>
#include <omp.h>

int count_words(std::string user_input);

int main(int argc, char *argv[])
{
  int thread_id , word_count;
  std::string user_input;

  #pragma omp parallel num_threads(2)
  {
    thread_id = omp_get_thread_num();
    if(thread_id == 0)
    {
      std::cout << "Digite la oración: ";
      std::getline(std::cin,user_input);
    }

    #pragma omp barrier
    if(thread_id == 1)
    {
      word_count = count_words(user_input);
      std::cout << user_input << " , " << word_count << std::endl;
    }

  }

}

int count_words(std::string user_input)
{
  std::stringstream stream(user_input);
  return  std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
}
